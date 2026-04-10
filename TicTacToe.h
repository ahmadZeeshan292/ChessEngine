#pragma once
#include <vector>

using namespace std;

class TicTacToe {

public:
	int row_count;
	int col_count;
	int action_size;

	TicTacToe();
	vector<vector<int>> get_initial_state() {
		return vector<vector<int>>(row_count, vector<int>(col_count, 0));
	}
	vector<vector<int>> get_next_state(vector<vector<int>> state, int action, int player);
	vector<int> get_valid_moves(vector<vector<int>> state);
	bool check_win(vector<vector<int>> state, int action);
	pair<int, bool> get_value_and_terminated(vector<vector<int>> state, int action);
	int get_opponent(int player);
	int get_opponent_value(int value);
	vector<vector<int>> change_perspective(vector<vector<int>> state, int player);

};
