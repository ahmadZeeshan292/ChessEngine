#pragma once
#include <vector>
#include "IGame.h"

using namespace std;

class TicTacToe: public IGame<vector<vector<int>>, vector<int>, vector<vector<vector<float>>>> {

public:
	int row_count;
	int col_count;
	int action_size;

	TicTacToe();
	vector<vector<int>> get_initial_state() override {
		return vector<vector<int>>(row_count, vector<int>(col_count, 0));
	}

	string get_name() override {
		return "TicTacToe";
	}
	vector<vector<int>> get_next_state(vector<vector<int>>& state, int action, int player) override;
	vector<int> get_valid_moves(vector<vector<int>> state) override;
	bool check_win(vector<vector<int>> state, int action) override;
	pair<int, bool> get_value_and_terminated(vector<vector<int>>& state, int& action) override;
	int get_opponent(int player) override;
	int get_opponent_value(int value);
	vector<vector<int>> change_perspective(vector<vector<int>> state, int player) override;
	vector<vector<vector<float>>> get_encoded_state(vector<vector<int>> state) override;

};
