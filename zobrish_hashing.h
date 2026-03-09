#pragma once

#include <cstdint>

class ZobrishHashing {

	uint64_t turnHash;
	uint64_t zobrishTable[768]; // [player][pieceType][square] + 2 for castling + 8 for en passant
	uint64_t zobrishCastling[16]; // castling rights
	uint16_t zobrishEnPassant[8]; // en passant file

	uint64_t currentHash; // current hash of the position

	uint64_t create_random_number();

public:
	ZobrishHashing();
	void init_zobrish_table();

	uint64_t getHash() const { return currentHash; }

	void make(bool player, uint8_t pieceType, uint8_t square);
	void unmake(bool player, uint8_t pieceType, uint8_t square);
	void castling(uint8_t new_castling_rights, uint8_t previous_castling_rights);

};

// What is zobrish:
// Zobrish is way to represent an chess position as an single 64 bit integer.

// Why do it?
// the alternative is to use an string as an char is an 256 bits thus is quite expensive

/* How does it work ?
   Zobrish does two things:
		i) it keeps tracks of piece location
		ii) it keeps tracks of rights such as castling and en-passant
*/

/* Why keep track of rights?
	In chess a position in which u can castle is different from one where u cant
	in this aspect we need an way to distinguish between the two, thus we need to 
	incorpoarte rights too to get the true real position
*/

/* Why 16 indexes for castling rights?
	We have 4 castling rights (white king side, white queen side, black king side, black queen side) 
	thus we can represent all possible combinations of castling rights with 16 indexes (2^4 = 16)
*/

/* What it means to get rid castling rights?
	Say white's queen side rook is captured, let this castling type have the index 1 given to it 
	initially we have all 4 castling right available thus we have in binary 1111 
	now currently the hash is set to castling rights[1111] which is all right available 
	but after removing the right of WQSC we have 1110 thus we set the position to hash
	castling rights[1110]
*/
