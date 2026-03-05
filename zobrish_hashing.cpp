#include "zobrish_hashing.h"
#include <random>
#include "Enums.h"

ZobrishHashing::ZobrishHashing()
{
	init_zobrish_table();
}

uint64_t ZobrishHashing::create_random_number()
{
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<uint64_t> dis;

	return dis(gen);
}

void ZobrishHashing::init_zobrish_table()
{
	for (int i = 0; i < 2; i++) { // Turn
		for (int j = 0; j < 6; j++) { // pieceType
			for (int k = 0; k < 64; k++) { // Square
				zobrishTable[i * 6 * 64 + j * 64 + k] = create_random_number();
			}
		}
	}

	for (int i = 0; i < 4; i++) { // Castling rights
		zobrishCastling[i] = create_random_number();
	}

	for (int i = 0; i < 8; i++) { // En passant file
		zobrishEnPassant[i] = create_random_number();
	}
}

void ZobrishHashing::updateHash(bool player, uint8_t pieceType, uint8_t square, bool normal, bool castling, uint8_t castling_type, bool capture, uint8_t enemyPieceType, uint8_t sqaure)
{
	if (normal) {
		if (capture)
			currentHash ^= zobrishTable[!player * 6 * 64 + enemyPieceType * 64 + sqaure]; // remove captured piece from hash

		currentHash ^= zobrishTable[player * 6 * 64 + pieceType * 64 + square]; // add moving piece to hash
	}
	else {
		if (castling) {
			currentHash ^= zobrishCastling[castling_type]; 
		}
		else { // en passant
			currentHash ^= zobrishEnPassant[sqaure % 8]; // remove en passant file from hash
			currentHash ^= zobrishTable[!player * 6 * 64 + idx(ChessPiece::PAWN) * 64 + sqaure - (player ? 8 : -8)]; // remove captured pawn from hash
		}
	}

}


