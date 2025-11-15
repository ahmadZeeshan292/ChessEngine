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


