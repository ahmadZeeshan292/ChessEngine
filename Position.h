#pragma once

#include <cstdint>

struct UndoPosition {
	uint64_t zobrishHash;
	uint16_t lastMove;
	uint8_t castlingRights;
	uint8_t halfMoveClock;
	uint8_t capturedPiece;
	int8_t enPassantSquare;
};

// enPassantSquare
// 0000__0000
// can be 0-7 where -1 indictaes no en-passant

// castlingRights
// 0000__0000
// 4 bits redundant | 4 bits for castling rights (KQkq) 

// LastMove 
// 0000__000000__000000
// Piece type (4 bits) | From square (6 bits) | To square (6 bits)

// halfMoveClock
// 00__000000
// 6 bits half move clock, 2 bits redundant

// captured
// 0000__0000
// 4 bits redundant | 4 bits for captured piece type (0 if no capture