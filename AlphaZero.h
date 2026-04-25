#pragma once

#include "TicTacToe.h"
#include "ResNet_Model.h"
#include "MCTS.h"
#include <random>

#define selfPlay_iterations 500
#define num_epochs 4
#define num_iterations 8
#define batch_size 128
#define temperature 1.25 //  temperature > 1 means more exploration, temperature < 1 means more exploitation

struct MemoryEntry {
	vector<vector<int>> state;
	vector<float> action_probs;
	int player;

	MemoryEntry(vector<vector<int>> state, vector<float> action_probs, int player) : state(state), action_probs(action_probs), player(player) {}
};

struct Experience {
	vector<vector<vector<float>>> encoded_state;
	vector<float> action_probs;
	float reward;
	Experience(vector<vector<vector<float>>> state, vector<float> action_probs, float reward) : encoded_state(state), action_probs(action_probs), reward(reward) {}
};


template<typename State, typename MoveContainer, typename EncodedState>
class AlphaZero {
	IGame<State, MoveContainer, EncodedState>& game;
	ResNet model;
	torch::optim::Adam optimizer;
	MCTS<State, MoveContainer, EncodedState> mcts;

	static mt19937& get_rng() {
		static mt19937 gen(random_device{}());
		return gen;
	}

public:
	AlphaZero(IGame<State, MoveContainer, EncodedState>& game, ResNet& model);

	vector<Experience> self_play();
	void train(vector<Experience>& flat_memory);
	void learn();
};

int getRandomAction(int actionSize, vector<float>& action_probs, mt19937& gen) {
	discrete_distribution<> dist(action_probs.begin(), action_probs.end());
	return dist(gen);
}

template<typename State, typename MoveContainer, typename EncodedState>
AlphaZero<State, MoveContainer, EncodedState>::AlphaZero(IGame<State, MoveContainer, EncodedState>& game, ResNet& model) : game(game), model(model), mcts(game, model), optimizer(model.parameters(), torch::optim::AdamOptions(0.001).weight_decay(0.001)) {}

template<typename State, typename MoveContainer, typename EncodedState>
vector<Experience> AlphaZero<State, MoveContainer, EncodedState>::self_play()
{
	vector<MemoryEntry> memory;
	int player = 1;
	vector<vector<int>> state = game.get_initial_state();

	int move_count = 0;
	while (true) {
		vector<vector<int>> neutral_state = game.change_perspective(state, player);
		vector<float> action_probs = mcts.search(neutral_state);

		memory.push_back(MemoryEntry(neutral_state, action_probs, player));

		vector<float> temperatured_probs = action_probs;
		float sum = 0.0f;
		for (auto& prob : temperatured_probs) {
			prob = pow(prob, 1.0f / temperature);
			sum += prob;
		}
		for (auto& prob : temperatured_probs)
			prob /= sum;

		int action = getRandomAction(game.action_size, temperatured_probs, get_rng());

		state = game.get_next_state(state, action, player);

		auto [value, is_terminal] = game.get_value_and_terminated(state, action);

		if (is_terminal) {

			vector<Experience> experience;
			experience.reserve(memory.size());

			for (auto& entry : memory) {
				int outcome = entry.player == player ? value : game.get_opponent_value(value);
				experience.push_back(Experience(game.get_encoded_state(entry.state), entry.action_probs, outcome));
			}

			return experience;
		}

		player = game.get_opponent(player);
		move_count++;
	}
}

template<typename State, typename MoveContainer, typename EncodedState>
void AlphaZero<State, MoveContainer, EncodedState>::train(vector<Experience>& flat_memory)
{
	for (int idx = 0; idx < (int)flat_memory.size(); idx += batch_size) {
		int end = min((int)flat_memory.size(), idx + batch_size);
		int current_batch_size = end - idx;

		vector<vector<vector<vector<float>>>> states;
		vector<vector<float>> action_probs;
		vector<float> rewards;
		states.reserve(current_batch_size);
		action_probs.reserve(current_batch_size);
		rewards.reserve(current_batch_size);

		for (int i = idx; i < end; i++) {
			states.push_back(flat_memory[i].encoded_state);
			action_probs.push_back(flat_memory[i].action_probs);
			rewards.push_back(flat_memory[i].reward);
		}

		std::vector<torch::Tensor> tensors;
		tensors.reserve(current_batch_size);
		for (int i = 0; i < (int)states.size(); i++) {
			tensors.push_back(convert_to_tensor(states[i], model.device).squeeze(0));
		}
		torch::Tensor state_tensor = torch::stack(tensors).to(torch::kFloat32);

		int prob_size = (int)action_probs[0].size();
		std::vector<float> flat_action_probs;
		flat_action_probs.reserve((size_t)current_batch_size * prob_size);
		for (const auto& probs : action_probs) {
			flat_action_probs.insert(flat_action_probs.end(), probs.begin(), probs.end());
		}

		torch::Tensor policy_targets = torch::from_blob(
			flat_action_probs.data(), { current_batch_size, prob_size }, torch::kFloat32)
			.clone().to(model.device);

		torch::Tensor value_target = torch::from_blob(
			rewards.data(), { current_batch_size, 1 }, torch::kFloat32)
			.clone().to(model.device);

		auto [out_policy, out_value] = model.forward(state_tensor);

		if (out_value.sizes().size() == 1) {
			out_value = out_value.unsqueeze(1);
		}

		torch::Tensor policy_loss = torch::nn::functional::kl_div(
			torch::log_softmax(out_policy, 1),
			policy_targets,
			torch::nn::functional::KLDivFuncOptions().reduction(torch::kBatchMean)
		);

		torch::Tensor value_loss = torch::nn::functional::mse_loss(out_value, value_target);

		torch::Tensor loss = policy_loss + value_loss;

		std::cout << "policy_loss: " << policy_loss.item<float>()
			<< " value_loss: " << value_loss.item<float>()
			<< " total_loss: " << loss.item<float>() << std::endl;

		optimizer.zero_grad();
		loss.backward();
		optimizer.step();
	}
}

template<typename State, typename MoveContainer, typename EncodedState>
void AlphaZero<State, MoveContainer, EncodedState>::learn()
{
	mt19937& gen = get_rng();

	for (int i = 0; i < num_iterations; i++) {
		cout << "\nIteration " << i + 1 << "/" << num_iterations << endl;

		vector<Experience> flat_memory;

		model.eval();
		{
			torch::NoGradGuard no_grad;
			for (int j = 0; j < selfPlay_iterations; j++) {
				cout << "Self-play game " << j + 1 << "/" << selfPlay_iterations << endl;
				vector<Experience> game_exp = self_play();
				flat_memory.insert(flat_memory.end(),
					make_move_iterator(game_exp.begin()),
					make_move_iterator(game_exp.end()));
			}
		}

		shuffle(flat_memory.begin(), flat_memory.end(), gen);

		for (int j = 0; j < num_epochs; j++) {
			cout << "Training epoch " << j + 1 << "/" << num_epochs << endl;
			model.train();
			train(flat_memory);
		}

		cout << "Saving Model" << endl;
		torch::serialize::OutputArchive archive;
		model.save(archive);
		archive.save_to("model_" + to_string(i) + game.get_name() + ".pt");

		torch::serialize::OutputArchive opt_archive;
		optimizer.save(opt_archive);
		opt_archive.save_to("optimizer_" + to_string(i) + game.get_name() + ".pt");
	}
}
