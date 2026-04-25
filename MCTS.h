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

	auto root_owner = make_unique<Node<State, MoveContainer, EncodedState>>(game, state);
	Node<State, MoveContainer, EncodedState>* root = root_owner.get();
	root->visit_count = 1;

	auto [root_value, root_policy] = evaluate_model(model, game.get_encoded_state(root->state));

	apply_dirichlet_noise(root_policy, DIRICHLET_ALPHA, DIRICHLET_EPSILON, model.device);
	vector<int> valid_moves = game.get_valid_moves(root->state);

	mask_and_normalize(root_policy, valid_moves);

	root->expand(root_policy);

	for (int i = 0; i < NUM_SEARCHES; i++) {
		Node<State, MoveContainer, EncodedState>* node = root;

		while (node->is_fully_expanded()) {
			node = node->select();
		}

		auto [value, is_terminal] = game.get_value_and_terminated(node->state, node->action);
		value = game.get_opponent_value(value);

		// Expansion Phase
		if (!is_terminal) {

			auto [eval_value, policy] = evaluate_model(model, game.get_encoded_state(node->state));
			value = eval_value;

			valid_moves = game.get_valid_moves(node->state);
			mask_and_normalize(policy, valid_moves);

			node->expand(policy);
		}

		node->backpropagate(value);
	}

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

	return action_probabilities;
}
