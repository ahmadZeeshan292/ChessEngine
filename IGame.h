#pragma once

#include <string>
#include <utility> 

template<typename State, typename MoveContainer, typename EncodedState>
class IGame {
public:
	int row_count;
	int col_count;
	int action_size;

	IGame(int r, int c, int a)
		: row_count(r), col_count(c), action_size(a) {
	}

	virtual std::string get_name() = 0;
	virtual State get_initial_state() = 0;
	virtual State get_next_state(State& state, int action, int player) = 0;
	virtual MoveContainer get_valid_moves(State state) = 0;
	virtual bool check_win(State state, int action) = 0;
	virtual std::pair<int, bool> get_value_and_terminated(State& state, int& action) = 0;
	virtual int get_opponent(int player) = 0;
	virtual int get_opponent_value(int value) = 0;
	virtual State change_perspective(State state, int player) = 0;
	virtual EncodedState get_encoded_state(State state) = 0;
	virtual ~IGame() = default;
};
