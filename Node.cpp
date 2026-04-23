//#include "Node.h"
//
//#include <numeric>
//#include <random>
//
//
//Node::Node(TicTacToe game, vector<vector<int>> state, Node* parent, int action, float prior) : game(game), state(state), parent(parent), action(action), prior(prior)
//{
//	/*expandable_moves = game.get_valid_moves(state);*/
//	visit_count = 0;
//	value_count = 0;
//}
//
//bool Node::is_fully_expanded()
//{
//	return !children.empty();
//}
//
//Node* Node::select()
//{
//	Node* bestChild = nullptr;
//	// Use double to match the UCB math precision
//	double best_ucb = -std::numeric_limits<double>::infinity();
//
//	if (children.empty()) {
//		return nullptr; // This should be handled by your while loop!
//	}
//
//	for (auto child : children) {
//		double ucb = get_ucb(*child);
//
//		if (ucb > best_ucb) {
//			best_ucb = ucb;
//			bestChild = child;
//		}
//	}
//
//	return (bestChild != nullptr) ? bestChild : children[0];
//}
//
//double Node::get_ucb(Node& child) {
//	double q_value = 0.0;
//	if (child.visit_count != 0)
//		q_value = 1 - (((double)child.value_count / child.visit_count) + 1) / 2.0;
//
//	return q_value + C * child.prior * (sqrt((double)visit_count) / (child.visit_count + 1));
//}
//
//int Node::select_random_move(vector<int> array)
//{
//	vector<int> indices;
//	for (int i = 0; i < array.size(); i++) {
//		if (array[i] == 1) {
//			indices.push_back(i);
//		}
//	}
//
//	if (!indices.empty()) {
//		random_device rd;
//		mt19937 gen(rd());
//		uniform_int_distribution<> dist(0, indices.size() - 1);
//
//		return indices[dist(gen)];
//	}
//	return -1;
//}
//
//void Node::expand(vector<float> policy)
//{
//	Node* child = nullptr;
//	for (int i = 0; i < policy.size(); i++) {
//		if (policy[i] > 0.0) {
//
//			vector<vector<int>> childState = state;
//			childState = game.get_next_state(childState, i, 1);
//			childState = game.change_perspective(childState, -1); // Change perspective for the opponent
//
//			child = new Node(game, childState, this, i, policy[i]);
//			children.push_back(child);
//		}
//	}
//}
//
//int Node::simulate()
//{
//	pair<int, bool> result = game.get_value_and_terminated(state, action);
//	int value = game.get_opponent_value(result.first);
//
//	if (result.second)
//		return value;
//
//	vector<vector<int>> rolloutState = state;
//	int rolloutPlayer = 1;
//
//	while (true) {
//		vector<int> valid_moves = game.get_valid_moves(rolloutState);
//		int action = select_random_move(valid_moves);
//
//		rolloutState = game.get_next_state(rolloutState, action, rolloutPlayer);
//		result = game.get_value_and_terminated(rolloutState, action);
//
//		if (result.second) {
//			if (rolloutPlayer == -1)
//				value = game.get_opponent_value(result.first);
//			return result.first;
//		}
//
//		rolloutPlayer = game.get_opponent(rolloutPlayer);
//	}
//}
//
//void Node::backpropagate(float value)
//{
//	value_count += value;
//	visit_count++;
//
//	value = game.get_opponent_value(value); // Change perspective for the opponent
//	if (parent != nullptr) {
//		parent->backpropagate(value);
//	}
//}
