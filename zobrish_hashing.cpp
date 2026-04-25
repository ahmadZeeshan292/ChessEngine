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

	for (int i = 0; i < 16; i++) { // Castling rights
		zobrishCastling[i] = create_random_number();
	}

	for (int i = 0; i < 8; i++) { // En passant file
		zobrishEnPassant[i] = create_random_number();
	}

	turnHash = create_random_number();
	currentHash = zobrishCastling[15];
}

void ZobrishHashing::make(bool player, uint8_t pieceType, uint8_t square)
{
	currentHash ^= turnHash;
	currentHash ^= zobrishTable[player * 6 * 64 + pieceType * 64 + square];
}

// used for captures doesnt update turnHash
void ZobrishHashing::unmake(bool player, uint8_t pieceType, uint8_t square)
{
	currentHash ^= zobrishTable[player * 6 * 64 + pieceType * 64 + square];
}

void ZobrishHashing::castling(uint8_t new_castling_rights, uint8_t previous_castling_rights)
{
	//currentHash ^= turnHash;
	currentHash ^= zobrishCastling[previous_castling_rights];
	currentHash ^= zobrishCastling[new_castling_rights];
}