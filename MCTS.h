#pragma once
#include <vector>
#include <numeric>
#include <random>

#include "TicTacToe.h"
using namespace std;

#define NUM_SEARCHES 100000
#define C 1.41 // Exploration parameter

class Node {
public:
	TicTacToe game;
	vector<vector<int>> state;

	Node* parent;
	int action;

	vector<Node*> children;
	vector<int> expandable_moves;

	int visit_count;
	int value_count;

	Node(TicTacToe game, vector<vector<int>> state, Node* parent=nullptr, int action=-1): game(game), state(state), parent(parent), action(action) {
		expandable_moves = game.get_valid_moves(state);
		visit_count = 0;
		value_count = 0;
	}

	// Needed for Selection Phase to check if an node is fully expanded 
	bool is_fully_expanded() {
		return accumulate(expandable_moves.begin(), expandable_moves.end(), 0) == 0 && !children.empty();
	}

	Node* select() {
		Node* bestChild = nullptr;
		// Use double to match the UCB math precision
		double best_ucb = -std::numeric_limits<double>::infinity();

		if (children.empty()) {
			return nullptr; // This should be handled by your while loop!
		}

		for (auto child : children) {
			double ucb = get_ucb(*child);

			if (ucb > best_ucb) {
				best_ucb = ucb;
				bestChild = child;
			}
		}

		return (bestChild != nullptr) ? bestChild : children[0];
	}

	/* ucb formula = Q(s, a) + c * sqrt(log(N(s)) / N(s, a))
	c is the exploration parameter, commonly set to sqrt(2), decides the balance between exploration and exploitation*/
	
	/*desirable to have an q_value between 0 - 1 to make it probabalistic
	n/v + 1 / 2 to make it between 0-1, where n is the value count and v is the visit count*/

	/*the 1 being substracted is to make it so that a higher q_value is better, since the parant branch would want an
	opponent to have a lower q_value, and the child branch would want a higher q_value*/
	double get_ucb(Node& child) {
		double q_value = 1.0 - ((double)child.value_count / child.visit_count + 1.0) / 2.0;
		return q_value + C * sqrt(log(visit_count) / child.visit_count);
	}

	int select_random_move(vector<int> array) {
		vector<int> indices;
		for (int i = 0; i < array.size(); i++) {
			if (array[i] == 1) {
				indices.push_back(i);
			}
		}

		if (!indices.empty()) {
			random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<> dist(0, indices.size() - 1);

			return indices[dist(gen)];
		}
		return -1;
	}

	Node* expand() {
		int action = select_random_move(expandable_moves);
		if (action < 0 || action > 8) {
			cout << "ACTION IS INVALID" << endl;
		}
		expandable_moves[action] = 0; // Mark the move as used

		vector<vector<int>> childState = state;
		childState = game.get_next_state(childState, action, 1);
		childState = game.change_perspective(childState, -1); // Change perspective for the opponent

		Node* child = new Node(game, childState, this, action);
		children.push_back(child);

		return child;
	}

	int simulate(){
		pair<int, bool> result = game.get_value_and_terminated(state, action);
		int value = game.get_opponent_value(result.first); 

		if (result.second)
			return value;

		vector<vector<int>> rolloutState = state;
		int rolloutPlayer = 1;

		while (true) {
			vector<int> valid_moves = game.get_valid_moves(rolloutState);
			int action = select_random_move(valid_moves);

			rolloutState = game.get_next_state(rolloutState, action, rolloutPlayer);
			result = game.get_value_and_terminated(rolloutState, action);

			if (result.second) {
				if (rolloutPlayer == -1)
					value = game.get_opponent_value(result.first);
				return result.first;
			}

			rolloutPlayer = game.get_opponent(rolloutPlayer);
		}
	}

	void backpropagate(int value) {
		value_count += value;
		visit_count++;

		value = game.get_opponent_value(value); // Change perspective for the opponent
		if (parent != nullptr) {
			parent->backpropagate(value);
		}
	}

};


class MCTS {
	TicTacToe game;

public:
	MCTS(TicTacToe game) : game(game) {}
	vector<float> search(vector<vector<int>> state);
};


inline vector<float> MCTS::search(vector<vector<int>> state)
{
	Node* root = new Node(game, state);

	for (int i = 0; i < NUM_SEARCHES; i++) {

		//cout << "Search " << i + 1 << endl;
		Node* node = root;

		// Selection Phase
		while (node->is_fully_expanded()) { // goes down as far as the terminal state
			node = node->select();
		}

		int value = 0;
		bool is_terminal = 0; // if terminal it means opponent has won not the player of the node

		{
			pair<int, bool> result = game.get_value_and_terminated(node->state, node->action);
			value = result.first;
			value = game.get_opponent_value(value); // since the value is from the perspective of the opponent, we need to get the opponent value to get the value from the perspective of the player of the node
			is_terminal = result.second;
		}

		// Expansion Phase
		if (!is_terminal) {
		    node = node->expand(); // expands one of the expandable moves randomly and returns the new node

			/*cout << "Expansion Phase: Expanded action " << node->action << endl;*/
			value = node->simulate(); // simulates a random playout from the new node and returns the value of the terminal state

			/*cout << "Simulation Phase: Simulated value " << value << endl;*/
		}

		// Backpropagation Phase

		node->backpropagate(value); // backpropagates the value up the tree, updating the visit count and value count of each node

		//cout << "Backpropagation Phase: Backpropagated value " << node->action << endl;
	}

	vector<float> action_probabilities(game.action_size, 0);
	float sum = 0;

	for (auto child : root->children) {
		action_probabilities[child->action] = (float)child->visit_count; // the probability of selecting an action is proportional to the visit count of the child node
		sum += child->visit_count;
	}

	for (auto child : root->children)
		action_probabilities[child->action] /= sum; // normalize the probabilities

	return action_probabilities;
}

