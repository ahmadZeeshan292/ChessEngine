#pragma once

#include <cstdint>
class ChessBitBoards;

namespace MoveGenerator {

	uint64_t PawnMoves(uint64_t board, uint64_t bitPosition, uint64_t allyBoard, uint64_t CheckBoard, uint8_t pinMap[64], bool player);
	uint64_t rookMoves(uint64_t board, uint64_t bitPosition, uint64_t allyBoard, uint64_t CheckBoard, uint8_t pinMap[64]);
	uint64_t bishopMoves(uint64_t board, uint64_t bitPosition, uint64_t allyBoard, uint64_t CheckBoard, uint8_t pinMap[64]);
	uint64_t kingMoves(uint64_t attackingMask, uint64_t bitPos, uint64_t allyBoard, bool king, uint8_t castlingRights, bool player);
	uint64_t knightMoves(uint64_t bitPos, uint64_t allyBoard, uint64_t CheckBoard, uint8_t pinMap[64]);
	uint64_t queenMoves(uint64_t board, uint64_t bitPos, uint64_t allyBoard, uint64_t CheckBoard, uint8_t pinMap[64]);

	// functions to update gameState (Needed to detected if king is in check after player performs an move)
	// returns an bitBoard in which pieces can move only to the lighted squares to defend the king
	// this is stored as Board[player][7] 

	uint64_t PawnCheck(uint64_t board, uint64_t enemyKingPos, uint64_t bitPosition, uint64_t allyBoard, uint8_t(&pinMap)[64], bool player);
	uint64_t rookCheck(uint64_t board, uint64_t enemyKingPos, uint64_t bitPosition, uint64_t allyBoard, uint8_t(&pinMap)[64]);
	uint64_t bishopCheck(uint64_t board, uint64_t enemyKingPos, uint64_t bitPosition, uint64_t allyBoard, uint8_t(&pinMap)[64]);
	uint64_t knightCheck(uint64_t board, uint64_t enemyKingPos, uint64_t bitPos, uint64_t allyBoard, uint8_t(&pinMap)[64]);
	uint64_t queenCheck(uint64_t board, uint64_t enemyKingPos, uint64_t bitPos, uint64_t allyBoard, uint8_t(&pinMap)[64]);

	// an intermediate function to print bitBoard
	void printBitBoard_MG(uint64_t bb);
	uint64_t handleCastling(uint64_t& allyBoard, uint8_t& castlingRight, bool& player, uint64_t& attackingMask, uint8_t bitPos);

}