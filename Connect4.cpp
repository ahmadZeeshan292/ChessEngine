#include "Connect4.h"
#include "numeric"

Connect4::Connect4() : IGame(6, 7, 7)
{
	row_count = 6;
	col_count = 7;
	action_size = col_count;
	in_a_row = 4;
}

vector<vector<int>> Connect4::get_initial_state()
{
	return vector<vector<int>>(row_count, vector<int>(col_count, 0));
}

vector<vector<int>> Connect4::get_next_state(vector<vector<int>>& state, int action, int player)
{
	int row =-1;

	for(int i = row_count - 1; i >= 0; i--){
		if (state[i][action] == 0) {
			row = i;
			break;
		}
	}

	state[row][action] = player;
	return state;

}

vector<int> Connect4::get_valid_moves(vector<vector<int>> state)
{
	vector<int> valid_moves(col_count, 0);
	for (int i = 0; i < col_count; i++) {
		if (state[0][i] == 0) {
			valid_moves[i] = 1;
		}
	}

	return valid_moves;
}

bool Connect4::check_win(vector<vector<int>> state, int action)
{
	vector<pair<int, int>> offsets = { {0, 1}, {1, 0}, {1, 1}, {1, -1} };

	int row = -1;
	int col = action;

	for (int i = 0; i < row_count; i++) {
		if (state[i][col] != 0) {
			row = i;
			break;
		}
	}
	int player = state[row][action];

	for (auto&[x, y] : offsets) {
		int count = 1;
		for (int d = -1; d <= 1; d += 2) {
			int r = row + d * x;
			int c = col + d * y;
			while (r >= 0 && r < row_count && c >= 0 && c < col_count && state[r][c] == player) {
				count++;
				r += d * x;
				c += d * y;
			}
		}
		if (count >= in_a_row) {
			return true;
		}
	}

	return false;
}

pair<int, bool> Connect4::get_value_and_terminated(vector<vector<int>>& state, int& action)
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

int Connect4::get_opponent(int player)
{
	return -player;
}

int Connect4::get_opponent_value(int value)
{
	return -value;
}

vector<vector<int>> Connect4::change_perspective(vector<vector<int>> state, int player)
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

vector<vector<vector<float>>> Connect4::get_encoded_state(vector<vector<int>> state)
{
	vector<vector<float>> player1_state(row_count, vector<float>(col_count, 0));
	vector<vector<float>> player0_state(row_count, vector<float>(col_count, 0));
	vector<vector<float>> avaliable_move_state(row_count, vector<float>(col_count, 0));

	for (int i = 0; i < row_count; i++) {
		for (int j = 0; j < col_count; j++) {
			if (state[i][j] == 1) {
				player1_state[i][j] = 1.0;
			}
			else if (state[i][j] == -1) {
				player0_state[i][j] = 1.0;
			}
			else {
				avaliable_move_state[i][j] = 1.0;
			}
		}
	}

	return { player0_state, avaliable_move_state, player1_state };
}
