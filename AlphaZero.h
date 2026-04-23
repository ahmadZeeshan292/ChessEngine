#pragma once

#include "TicTacToe.h"
#include "ResNet_Model.h"
#include "MCTS.h"

#define selfPlay_iterations 500
#define num_epochs 4
#define num_iterations 3
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

public:
	AlphaZero(IGame<State, MoveContainer, EncodedState> &game, ResNet &model);

	vector<Experience> self_play();
	void train(vector<vector<Experience>>&memory);
	void learn();
};

#include "AlphaZero.h"
#include <random>


int getRandomAction(int actionSize, vector<float>& action_probs) {
	random_device rd;
	mt19937 gen(rd());
	discrete_distribution<> dist(action_probs.begin(), action_probs.end());
	return dist(gen);
}

void shuffle_memory(vector<vector<Experience>>& memory) {
	random_device rd;
	mt19937 gen(rd());
	shuffle(memory.begin(), memory.end(), gen);
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

		for (auto& prob : temperatured_probs) {
			prob = pow(prob, 1.0 / temperature);
		}

		//cout << endl;

		float sum = 0;

		for (auto& prob : temperatured_probs)
			sum += prob;
		for (auto& prob : temperatured_probs)
			prob /= sum;

		int action = getRandomAction(game.action_size, temperatured_probs);

		state = game.get_next_state(state, action, player);

		auto [value, is_terminal] = game.get_value_and_terminated(state, action);

		if (is_terminal) {

			//cout << "Player " << player << " takes action " << action << " with value " << value << ". Terminal State reached after: " << move_count << endl;
			vector<vector<int>> final_neutral_state = game.change_perspective(state, player);
			vector<float> dummy_probs(game.action_size, 1.0 / game.action_size); // Uniform is safer than all zeros

			memory.push_back(MemoryEntry(final_neutral_state, dummy_probs, player));

			vector<Experience> experience;

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
void AlphaZero<State, MoveContainer, EncodedState>::train(vector<vector<Experience>>& memory)
{
	shuffle_memory(memory);

	for (int idx = 0; idx < memory.size(); idx += batch_size) {
		vector<vector<Experience>> sample(memory.begin() + idx, min(memory.begin() + memory.size(), memory.begin() + idx + batch_size));

		vector<vector<vector<vector<float>>>> states;
		vector<vector<float>> action_probs;
		vector<float> rewards;

		for (auto& experience : sample) {
			for (auto& entry : experience) {
				states.push_back(entry.encoded_state);
				action_probs.push_back(entry.action_probs);
				rewards.push_back(entry.reward);
			}
		}

		std::vector<torch::Tensor> tensors;
		for (int i = 0; i < states.size(); i++) {
			tensors.push_back(convert_to_tensor(states[i], model.device).squeeze(0));
		}

		torch::Tensor state_tensor = torch::stack(tensors).to(torch::kFloat32);

		// Policy targets: [batch, action_size]
		std::vector<float> flat_action_probs;
		for (const auto& probs : action_probs) {
			flat_action_probs.insert(flat_action_probs.end(), probs.begin(), probs.end());
		}

		torch::Tensor policy_targets = torch::from_blob(
			flat_action_probs.data(), { (int)action_probs.size(), (int)action_probs[0].size() }, torch::kFloat32).
			clone().to(model.device);

		// Value targets: [batch, 1]
		torch::Tensor value_target = torch::from_blob(rewards.data(), { (int)rewards.size(), 1 }, torch::kFloat32)
			.clone()
			.to(model.device);

		auto [out_policy, out_value] = model.forward(state_tensor);

		// Ensure out_value is [batch, 1]
		if (out_value.sizes().size() == 1) {
			out_value = out_value.unsqueeze(1);
		}

		// Policy loss: negative log likelihood with log_softmax
		torch::Tensor policy_loss = torch::nn::functional::kl_div(
			torch::log_softmax(out_policy, 1),
			policy_targets,
			torch::nn::functional::KLDivFuncOptions().reduction(torch::kBatchMean)
		);

		// Value loss: MSE
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
	for (int i = 0; i < num_iterations; i++) {

		vector<vector<Experience>> memory; // training data from self play
		cout << "\nIteration " << i + 1 << "/" << num_iterations << endl;

		model.eval();
		for (int j = 0; j < selfPlay_iterations; j++) {
			cout << "Self-play game " << j + 1 << "/" << selfPlay_iterations << endl;
			memory.push_back(self_play());
		}

		for (int j = 0; j < num_epochs; j++) {
			cout << "Training epoch " << j + 1 << "/" << num_epochs << endl;
			model.train();
			train(memory);
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


