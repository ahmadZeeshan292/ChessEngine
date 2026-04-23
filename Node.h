#pragma once

#include <vector>
#include "TicTacToe.h"
#include <random>
using namespace std;

#define NUM_SEARCHES 400
#define C 2
#define DIRICHLET_EPSILON 0.25
#define DIRICHLET_ALPHA 0.3

static constexpr int SQRT_TABLE_SIZE = 8192;

inline float fast_sqrt(int n)
{
	static const auto table = []() {
		array<float, SQRT_TABLE_SIZE> t{};
		for (int i = 0; i < SQRT_TABLE_SIZE; ++i)
			t[i] = std::sqrt(static_cast<float>(i));
		return t;
		}();
	return (n < SQRT_TABLE_SIZE) ? table[n] : std::sqrt(static_cast<float>(n));
}


template<typename State, typename MoveContainer, typename EncodedState>
class Node {
public:
	IGame<State, MoveContainer, EncodedState>& game;
	vector<vector<int>> state;

	Node* parent;
	int action;

	vector<Node*> children;
	float prior;

	int visit_count;
	int value_count;

	Node(IGame<State, MoveContainer, EncodedState>& game, vector<vector<int>> state, Node* parent = nullptr, int action = -1, float prior = 0.0);
	bool is_fully_expanded();
	Node* select();
	double get_ucb(Node& child);
	int select_random_move(const vector<int> &array);
	void expand(vector<float> policy);
	int simulate();
	void backpropagate(float value);
	~Node();
};

template<typename State, typename MoveContainer, typename EncodedState>
Node<State, MoveContainer, EncodedState>::~Node()
{
	for (Node* child : children)
		delete child;
}

template<typename State, typename MoveContainer, typename EncodedState>
Node<State, MoveContainer, EncodedState>::Node(IGame<State, MoveContainer, EncodedState>& game, vector<vector<int>> state, Node* parent, int action, float prior)
	: game(game), state(state), parent(parent), action(action), prior(prior)
{
	visit_count = 0;
	value_count = 0;
}

template<typename State, typename MoveContainer, typename EncodedState>
bool Node<State, MoveContainer, EncodedState>::is_fully_expanded()
{
	return !children.empty();
}

template<typename State, typename MoveContainer, typename EncodedState>
Node<State, MoveContainer, EncodedState>* Node<State, MoveContainer, EncodedState>::select() 
{
	Node* bestChild = nullptr;
	double best_ucb = -std::numeric_limits<double>::infinity();

	if (children.empty()) {
		return nullptr;
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

template<typename State, typename MoveContainer, typename EncodedState>
double Node<State, MoveContainer, EncodedState>::get_ucb(Node& child)
{
	double q_value = 0.0;
	if (child.visit_count != 0)
		q_value = 1 - (((double)child.value_count / child.visit_count) + 1) / 2.0;

	return q_value+ C * child.prior * (fast_sqrt(visit_count) / static_cast<double>(child.visit_count + 1));
}

template<typename State, typename MoveContainer, typename EncodedState> 
int Node<State, MoveContainer, EncodedState>::select_random_move(const vector<int> &array)
{
	static thread_local vector<int> indices;
	for (int i = 0; i < array.size(); i++) {
		if (array[i] == 1) {
			indices.push_back(i);
		}
	}

	if (indices.empty()) return -1;

	uniform_int_distribution<int> dist(0, static_cast<int>(indices.size()) - 1);
	return indices[dist(rng())];
}

template<typename State, typename MoveContainer, typename EncodedState>
void Node<State, MoveContainer, EncodedState>::expand(vector<float> policy)
{
	int count = 0;
	for (float p : policy) if (p > 0.0f) ++count;
	children.reserve(count);

	for (int i = 0; i < policy.size(); i++) {
		if (policy[i] > 0.0f) {
			vector<vector<int>> childState = state;
			childState = game.get_next_state(childState, i, 1);
			childState = game.change_perspective(childState, -1);

			children.push_back(new Node(game, move(childState), this, i, policy[i]));
		}
	}
}

template<typename State, typename MoveContainer, typename EncodedState>
int Node<State, MoveContainer, EncodedState>::simulate()
{
	pair<int, bool> result = game.get_value_and_terminated(state, action);
	int value = game.get_opponent_value(result.first);

	if (result.second)
		return value;

	vector<vector<int>> rolloutState = state;
	int rolloutPlayer = 1;

	while (true) {
		vector<int> valid_moves = game.get_valid_moves(rolloutState);
		int rolloutAction = select_random_move(valid_moves); 

		rolloutState = game.get_next_state(rolloutState, rolloutAction, rolloutPlayer);
		result = game.get_value_and_terminated(rolloutState, rolloutAction);

		if (result.second) {
			return result.first;
		}

		rolloutPlayer = game.get_opponent(rolloutPlayer);
	}
}

template<typename State, typename MoveContainer, typename EncodedState>
void Node<State, MoveContainer, EncodedState>::backpropagate(float value)
{
	Node* node = this;

	while (node != nullptr) {
		node->value_count += value;
		node->visit_count += 1;

		value = node->game.get_opponent_value(value);
		node = node->parent;
	}
}