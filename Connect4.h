#pragma once

#include "IGame.h"
#include <vector>

using namespace std;

class Connect4 : public IGame<vector<vector<int>>, vector<int>, vector<vector<vector<float>>>> {
public:

	int row_count;
	int col_count;
	int action_size;
	int in_a_row;

	Connect4();
	~Connect4() = default;

	string get_name() override {
		return "Connect4";
	}
	vector<vector<int>> get_initial_state() override;
	vector<vector<int>> get_next_state(vector<vector<int>>& state, int action, int player) override;
	vector<int> get_valid_moves(vector<vector<int>> state) override;
	bool check_win(vector<vector<int>> state, int action) override;
	pair<float, bool> get_value_and_terminated(vector<vector<int>>& state, int& action) override;
	int get_opponent(int player) override;
	int get_opponent_value(int value);
	vector<vector<int>> change_perspective(vector<vector<int>> state, int player) override;
	vector<vector<vector<float>>> get_encoded_state(vector<vector<int>> state) override;
	vector<vector<vector<vector<float>>>> get_encoded_state_parallel(const vector<vector<vector<int>>>& states) override;
};

