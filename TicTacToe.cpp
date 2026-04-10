#include "TicTacToe.h"
#include "numeric"
TicTacToe::TicTacToe()
{
	row_count = 3;
	col_count = 3;
	action_size = row_count * col_count;
}

vector<vector<int>> TicTacToe::get_next_state(vector<vector<int>> state, int action, int player)
{
	int row = action / col_count;
	int col = action % col_count;

	state[row][col] = player;
	return state;
}

vector<int> TicTacToe::get_valid_moves(vector<vector<int>> state)
{
	vector<int> valid_moves;
	for (int i = 0; i < row_count; i++)
	{
		for (int j = 0; j < col_count; j++)
		{
			valid_moves.push_back(state[i][j] == 0);
		}
	}

	return valid_moves;

}

bool TicTacToe::check_win(vector<vector<int>> state, int action)
{
	if (action == -1) {
		return false; 
	}
	int row = action / col_count;
	int col = action % col_count;

	int player = state[row][col];

	return (state[row][0] == player && state[row][1] == player && state[row][2] == player) ||
		(state[0][col] == player && state[1][col] == player && state[2][col] == player) ||
		(row == col && state[0][0] == player && state[1][1] == player && state[2][2] == player) ||
		(row + col == 2 && state[0][2] == player && state[1][1] == player && state[2][0] == player);
}

pair<int, bool> TicTacToe::get_value_and_terminated(vector<vector<int>> state, int action)
{
	if (check_win(state, action)) {
		return { 1, true }; // Current player wins
	}

	vector<int> moves = get_valid_moves(state);

	if (accumulate(moves.begin(), moves.end(), 0) == 0) {
		return { 0, true }; // Draw
	}

	return { 0, false }; // Game continues
}

int TicTacToe::get_opponent(int player)
{
	return player == 1 ? -1 : 1;
}

int TicTacToe::get_opponent_value(int value)
{
	return -value;
}

vector<vector<int>> TicTacToe::change_perspective(vector<vector<int>> state, int player)
{
	for (int i = 0; i < row_count; i++)
	{
		for (int j = 0; j < col_count; j++)
		{
			state[i][j] *= player;
		}
	}

	return state;

}
