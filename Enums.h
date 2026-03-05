#pragma once

enum class Turn: bool {
	WHITE,
	BLACK
};

enum class ChessPiece : int {
	PAWN,
	KING,
	QUEEN,
	BISHOP,
	KNIGHT,
	ROOK
};

enum class GameState : bool {
	GAME,
	PLAYER
};

enum class Attacker : int {
	LONG_RANGE,
	SHORT_RANGE
};

enum CastlingRights : uint8_t {
	NO_CASTLING = 0,
	WHITE_KING_SIDE = 1 << 0,  // 0001
	WHITE_QUEEN_SIDE = 1 << 1,  // 0010
	BLACK_KING_SIDE = 1 << 2,  // 0100
	BLACK_QUEEN_SIDE = 1 << 3   // 1000
};

constexpr int idx(Turn t) {
	return static_cast<int>(t);
}

constexpr int idx(ChessPiece p) {
	return static_cast<int>(p);
}


