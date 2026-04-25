#include <iostream>

#include "AlphaZeroParallel.h"
#include "Connect4.h"

//#include "MCTS.h"

using namespace std;

void print_Board(vector<vector<int>> board) {
	for (int i = 0; i < board.size(); i++) {
		for (int j = 0; j < board[i].size(); j++) {
			cout << (board[i][j] == 0 ? "_" : board[i][j] == -1 ? "X" : "0") << " ";
		}
		cout << endl;
	}
}

//void run_game() {
//
//	torch::Device device = torch::cuda::is_available() ? torch::Device(torch::kCUDA) : torch::Device(torch::kCPU);
//
//	Connect4 game;
//	int player = 1;
//
//	auto Model = std::make_shared<ResNet>(game, 9, 128, device);
//
//	torch::load(Model, "model_3Connect4.pt");
//
//	MCTS mcts(game, *Model);
//	int action;
//
//	vector<vector<int>> state = game.get_initial_state();
//
//	while (true) {
//
//		print_Board(state);
//
//		if (player == 1) {
//			vector<int> valid_moves = game.get_valid_moves(state);
//
//			cout << "Valid moves: ";
//			for (int i = 0; i < game.action_size; i++) {
//				if (valid_moves[i]) {
//					cout << i << " ";
//				}
//			}
//
//			cout << endl << player << ": ";
//			cin >> action;
//
//			if (valid_moves[action] == 0) {
//				cout << "Invalid move. Try again." << endl;
//				continue;
//			}
//		}
//		else {
//			vector<vector<int>> neutralState = game.change_perspective(state, player);
//			vector<float> mcts_probs = mcts.search(neutralState);
//
//			action = -1;
//			float best_prob = -1;
//			for (int i = 0; i < mcts_probs.size(); i++) {
//				if (mcts_probs[i] > best_prob) {
//					action = i;
//					best_prob = mcts_probs[i];
//				}
//			}
//
//			for (int i = 0; i < mcts_probs.size(); i++) {
//				cout << "Action: " << i << " , Prob: " << mcts_probs[i] << endl;
//			}
//
//			cout << endl;
//
//		}
//		state = game.get_next_state(state, action, player);
//		pair<int, bool> result = game.get_value_and_terminated(state, action);
//
//		if (result.second) {
//			if (result.first == 1) {
//				cout << "Player " << player << " wins!" << endl;
//			}
//			else {
//				cout << "It's a draw!" << endl;
//			}
//			break;
//		}
//
//		player = game.get_opponent(player);
//		cout << "\n\n";
//	}
//}

void learn() {
	torch::Device device = torch::cuda::is_available()
		? torch::Device(torch::kCUDA)
		: torch::Device(torch::kCPU);

	if (device.is_cuda())
		cout << "Using GPU" << endl;
	else
		cout << "Using CPU" << endl;

	Connect4 game;

	auto model = std::make_shared<ResNet>(game, 9, 128, device);

	AlphaZeroParallel alphaZero(game, *model);
	alphaZero.learn();
}

void test() {
	torch::Device device = torch::cuda::is_available() ? torch::Device(torch::kCUDA) : torch::Device(torch::kCPU);

	TicTacToe game;
	int player = 1;

	auto Model = std::make_shared<ResNet>(game, 4, 64, device);
	vector<vector<int>> state = game.get_initial_state();

	for (int i = 0; i < 3; i++) {
		cout << "Loaded Model epoch " << i << endl;
		torch::load(Model, "model_" + to_string(i) + ".pt");
		Model->eval();

		auto output = evaluate_model(*Model, game.get_encoded_state(state));

		cout << "Model Evaluation of the position: " << output.first << endl;
		for (int i = 0; i < output.second.size(); i++) {
			cout << "Action " << i << ": " << output.second[i] << endl;
		}
	}
}

int main() {

	torch::manual_seed(0); // same random numbers for every compilation, for testing purposes
	learn();

}