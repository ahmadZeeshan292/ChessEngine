#pragma once

#include <cstdint>

class ZobrishHashing {

	uint64_t zobrishTable[768]; // [player][pieceType][square] + 2 for castling + 8 for en passant
	uint64_t zobrishCastling[4]; // castling rights
	uint16_t zobrishEnPassant[8]; // en passant file

	uint64_t currentHash; // current hash of the position

	uint64_t create_random_number();

public:
	ZobrishHashing();
	void init_zobrish_table();

	void updateHash(bool player, uint8_t pieceType, uint8_t square, bool normal, bool castling = false, uint8_t castling_type = 0, bool capture = false, uint8_t enemyPieceType = 0, uint8_t sqaure = 0);
	uint64_t getHash() const { return currentHash; }
	
};
