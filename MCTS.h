#pragma once

#include <vector>
using namespace std;

#include "TicTacToe.h"
#include "ResNet_Model.h"

#include "Node.h"

template<typename State, typename MoveContainer, typename EncodedState>
class MCTS {
	IGame<State, MoveContainer, EncodedState>& game;
	ResNet model;

	void mask_and_normalize(vector<float>& policy, const vector<int>& valid_moves) {
		float sum = 0.0f;
		for (int i = 0; i < (int)policy.size(); i++) {
			policy[i] *= valid_moves[i];
			sum += policy[i];
		}
		if (sum > 0.0f) {
			float inv_sum = 1.0f / sum;
			for (auto& p : policy) p *= inv_sum;
		}
	}

public:
	MCTS(IGame<State, MoveContainer, EncodedState>& game, ResNet& model) : game(game), model(model) {}
	vector<float> search(vector<vector<int>> state);
};

void apply_dirichlet_noise(vector<float>& policy, float alpha, float epsilon, torch::Device& device) {
	int64_t size = (int64_t)policy.size();
	torch::Tensor concentration = torch::full({ size }, alpha,
		torch::TensorOptions().dtype(torch::kFloat32).device(device));
	torch::Tensor gamma_samples = at::_standard_gamma(concentration);
	torch::Tensor dirichlet = gamma_samples / gamma_samples.sum();

	const float* dir_ptr = dirichlet.data_ptr<float>();
	float keep = 1.0f - epsilon;
	for (int i = 0; i < (int)policy.size(); i++) {
		policy[i] = keep * policy[i] + epsilon * dir_ptr[i];
	}
}

template<typename State, typename MoveContainer, typename EncodedState>
vector<float> MCTS<State, MoveContainer, EncodedState>::search(vector<vector<int>> state)
{
	torch::NoGradGuard no_grad;

	// FIX #1: unique_ptr owns the tree — automatically freed when search() returns, no leak
	auto root_owner = make_unique<Node<State, MoveContainer, EncodedState>>(game, state);
	Node<State, MoveContainer, EncodedState>* root = root_owner.get();
	root->visit_count = 1;

	// Evaluate root node once
	auto [root_value, root_policy] = evaluate_model(model, game.get_encoded_state(root->state));

	// FIX #7: apply Dirichlet directly on vector — eliminates from_blob + clone tensor roundtrip
	apply_dirichlet_noise(root_policy, DIRICHLET_ALPHA, DIRICHLET_EPSILON, model.device);

	// FIX #4: get_valid_moves called once for root, reused in expansion below
	vector<int> valid_moves = game.get_valid_moves(root->state);

	// FIX #3: mask + normalize via shared helper — no duplicated block
	mask_and_normalize(root_policy, valid_moves);

	root->expand(root_policy);

	for (int i = 0; i < NUM_SEARCHES; i++) {
		Node<State, MoveContainer, EncodedState>* node = root;

		// Selection Phase — walk down until a non-fully-expanded node
		while (node->is_fully_expanded()) {
			node = node->select();
		}

		auto [value, is_terminal] = game.get_value_and_terminated(node->state, node->action);
		value = game.get_opponent_value(value);

		// Expansion Phase
		if (!is_terminal) {
			// FIX #2: evaluate_model only called once per search iteration (not redundantly at root)
			// FIX #6: structured binding avoids extra vector copy
			auto [eval_value, policy] = evaluate_model(model, game.get_encoded_state(node->state));
			value = eval_value;

			// FIX #4: valid_moves fetched only for this specific node state
			valid_moves = game.get_valid_moves(node->state);

			// FIX #3: same helper reused — no copy-pasted masking/normalization block
			mask_and_normalize(policy, valid_moves);

			node->expand(policy);
		}

		// Backpropagation Phase
		node->backpropagate(value);
	}

	// FIX #5: single pass — collect visit counts and accumulate sum simultaneously
	vector<float> action_probabilities(game.action_size, 0.0f);
	float sum = 0.0f;
	for (auto& child : root->children) {
		float vc = (float)child->visit_count;
		action_probabilities[child->action] = vc;
		sum += vc;
	}
	if (sum > 0.0f) {
		float inv_sum = 1.0f / sum;
		for (auto& child : root->children)
			action_probabilities[child->action] *= inv_sum;
	}

	// root_owner destructor fires here — FIX #1: entire tree freed with no manual delete
	return action_probabilities;
}