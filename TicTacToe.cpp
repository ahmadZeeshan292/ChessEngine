#include "TicTacToe.h"
#include "numeric"
TicTacToe::TicTacToe() : IGame(3, 3, 9)
{
	row_count = 3;
	col_count = 3;
	action_size = row_count * col_count;
}

vector<vector<int>> TicTacToe::get_next_state(vector<vector<int>>& state, int action, int player)
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

pair<float, bool> TicTacToe::get_value_and_terminated(vector<vector<int>>& state, int& action)
{
	if (check_win(state, action)) {
		return { 1.0, true }; // Current player wins
	}

	vector<int> moves = get_valid_moves(state);

	if (accumulate(moves.begin(), moves.end(), 0) == 0) {
		return { 0.0, true }; // Draw
	}

	return { 0.0, false }; // Game continues
}

int TicTacToe::get_opponent(int player)
{
	return -player;
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

vector<vector<vector<float>>> TicTacToe::get_encoded_state(vector<vector<int>> state)
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

	return {player0_state, avaliable_move_state, player1_state};
	
}

vector<vector<vector<vector<float>>>> TicTacToe::get_encoded_state_parallel(const vector<vector<vector<int>>>& states)
{
	int batch = states.size();
	vector<vector<vector<vector<float>>>> batch_encoded(batch);

	for (int b = 0; b < batch; b++) {
		const auto& state = states[b];

		vector<vector<vector<float>>> encoded_state(3, vector<vector<float>>(row_count, vector<float>(col_count, 0.0f)));

		for (int i = 0; i < row_count; i++) {
			for (int j = 0; j < col_count; j++) {
				int val = state[i][j];
				encoded_state[0][i][j] = (val == 1) ? 1.0f : 0.0f;
				encoded_state[1][i][j] = (val == -1) ? 1.0f : 0.0f;
				encoded_state[2][i][j] = (val == 0) ? 1.0f : 0.0f;
			}
		}
		batch_encoded[b] = std::move(encoded_state);
	}

	return batch_encoded;
}
