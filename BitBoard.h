#pragma once

#include<SFML/Graphics.hpp>
#include <cstdint>
#include "Enums.h"
#include <string>
#include <unordered_map>

static constexpr uint8_t NO_PIN = 64;
static constexpr uint64_t DOUBLE_CHECK = 0xFFFFFFFF77777777;
static constexpr uint64_t STATEMATE = 0x77777777FFFFFFFFULL;
static constexpr uint64_t CHECKMATE = 0xFFFFFFFFFFFFFFFFULL;

using namespace std;

struct BitBoard {
	uint64_t board;

	string filepath;

	BitBoard(string filepath, uint64_t val) : board(val){
		if (filepath == "") return;
		this->filepath = filepath;
	}
};

class ChessBitBoards {
public:
	BitBoard* Boards[2][8];
	uint8_t pinMap[64];


	ChessBitBoards();
	void Initialize_BitBoards();

	pair<uint8_t, uint8_t> UpdateBoards(uint64_t toMask, bool player);
	pair<uint8_t, uint8_t> makeMove(uint64_t fromMask, uint64_t toMask, int pieceType, bool player);
	void UpdateGameState(bool player);

	bool Terminal();
	uint64_t LegalMoves(uint64_t enemyKingPos, uint64_t bitPos, int pieceType, bool player, bool gameState);
	uint64_t computeAttackingMask(uint64_t bitPos, bool player, bool gameState, bool detCanMove);
};
