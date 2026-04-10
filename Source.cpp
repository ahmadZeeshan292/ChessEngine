
#include "BitBoard_Game.h"
#include "BitBoard_ChessBoard_visuals.h"
#include "BB_MagicAlgorithm.h"
#include <iostream>
#include <intrin.h>

#include "TicTacToe.h"
#include "algorithm"
#include "MCTS.h"
using namespace std;

int main() {

	TicTacToe game;
	int player = 1;

	MCTS mcts(game);
	int action;

	vector<vector<int>> state = game.get_initial_state();

	while (true) {

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				cout << state[i][j] << " ";
			}
			cout << endl;
		}

		if (player == 1) {
			vector<int> valid_moves = game.get_valid_moves(state);

			cout << "Valid moves: ";
			for (int i = 0; i < game.action_size; i++) {
				if (valid_moves[i]) {
					cout << i << " ";
				}
			}

			cout << endl << player << ": ";
			cin >> action;

			if (valid_moves[action] == 0) {
				cout << "Invalid move. Try again." << endl;
				continue;
			}
		}
		else {
			vector<vector<int>> neutralState = game.change_perspective(state, player);
			vector<float> mcts_probs = mcts.search(neutralState);

			action = -1;
			float best_prob = -1;
			for (int i = 0; i < mcts_probs.size(); i++) {
				if (mcts_probs[i] > best_prob) {
					action = i;
					best_prob = mcts_probs[i];
				}
			}

		}
		state = game.get_next_state(state, action, player);
		pair<int, bool> result = game.get_value_and_terminated(state, action);

		if (result.second) {
			if (result.first == 1) {
				cout << "Player " << player << " wins!" << endl;
			}
			else {
				cout << "It's a draw!" << endl;
			}
			break;
		}

		player = game.get_opponent(player);
		cout << "\n\n";
		
	}

	/*forwardMagic();
	ChessBoard_BitBoard* bitBoard = new ChessBoard_BitBoard();

	Game* game = new Game(bitBoard);
	game->Update(bitBoard);*/

	

	//printBitBoard(compute_occupany(90, rookAttackMask(27), count_ones(rookAttackMask(27))));
	//printBitBoard(att_mask);
	//printBitBoard(att_mask);
}
