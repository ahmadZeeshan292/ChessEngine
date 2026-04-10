#include "BB_MoveGenerator.h"
#include "BitBoard_ChessBoard_visuals.h"
#include "Utility_BB.h"
#include "BB_MagicAlgorithm.h"
#include "BitBoard.h"
#include <iostream>

uint64_t MoveGenerator::PawnMoves(uint64_t board, uint64_t bitPosition, uint64_t allyBoard, uint64_t CheckBoard, uint8_t pinMap[64], bool player)
{
	uint64_t m = 0x00ULL;

	if (CheckBoard == DOUBLE_CHECK) { cout << "KING IN DOUBLE CHECK ONLY KING MOVES ALLOWED" << endl; return 0x00ULL; }

	if (pinMap[bitPosition] != NO_PIN) {
		cout << "PAWN IS PINNED TO SQUARE: " << unsigned(pinMap[bitPosition]) << endl;
		uint64_t pinRay = computeCheckRay(pinMap[bitPosition], bitPosition) & m & ~(1ULL << bitPosition);
		printBitBoard(pinRay | (1ULL << pinMap[bitPosition]));
		return pinRay != 0x00ULL ? pinRay | (1ULL << pinMap[bitPosition]) : pinRay;
	}

	bool firstMove = player == idx(Turn::WHITE) ? (bitPosition >= 8 && bitPosition <= 15) : (bitPosition >= 48 && bitPosition <= 55);
	cout << "FIRST MOVE FOR THIS PAWN: " << (firstMove ? "YES" : "NO") << " BitPositon: " << bitPosition << endl;

	// Moves for Pawns
	m = player ? ((1ULL << bitPosition) >> 8) : ((1ULL << bitPosition) << 8); // single push
	if (firstMove) m |= player ? ((1ULL << bitPosition) >> 16) : ((1ULL << bitPosition) << 16); // double push

	m |= computePawnAttacks(bitPosition, player) & (board & allyBoard);
	printBitBoard(m);

	m = (m & allyBoard) ^ m;
	printBitBoard(m);

	return CheckBoard == 0x00ULL ? m : m & CheckBoard;
}

uint64_t MoveGenerator::rookMoves(uint64_t board, uint64_t bitPosition, uint64_t allyBoard, uint64_t CheckBoard, uint8_t pinMap[64]) {

	if (CheckBoard == DOUBLE_CHECK) { cout << "KING IN DOUBLE CHECK ONLY KING MOVES ALLOWED" << endl; return 0x00ULL; }

	if (pinMap[bitPosition] != NO_PIN) {
		cout << "ROOK IS PINNED TO SQUARE: " << unsigned(pinMap[bitPosition]) << endl;
		uint64_t pinRay = computeCheckRay(pinMap[bitPosition], bitPosition) & rookAttackMask(bitPosition, true) & ~(1ULL << bitPosition);
		printBitBoard(pinRay | (1ULL << pinMap[bitPosition]));
		return pinRay != 0x00ULL ? pinRay | (1ULL << pinMap[bitPosition]) : pinRay;
	}
	
	uint64_t occupany = board & rookAttackMask(bitPosition, false);
	uint64_t m = computeRookMoves(occupany, bitPosition);

	m = (m & allyBoard) ^ m;
	printBitBoard(m);

	cout << "PRINTING CHECKBOARD FOR ROOK MOVES" << endl;
	printBitBoard(CheckBoard);

	return CheckBoard == 0x00ULL ? m : m & CheckBoard;
}

uint64_t MoveGenerator::bishopMoves(uint64_t board, uint64_t bitPosition, uint64_t allyBoard, uint64_t CheckBoard, uint8_t pinMap[64])
{
	if (CheckBoard == DOUBLE_CHECK) { cout << "KING IN DOUBLE CHECK ONLY KING MOVES ALLOWED" << endl; return 0x00ULL; }

	if (pinMap[bitPosition] != NO_PIN) {
		cout << "BISHOP IS PINNED TO SQUARE: " << unsigned(pinMap[bitPosition]) << endl;
		uint64_t pinRay = computeCheckRay(bitPosition, pinMap[bitPosition]) & bishopAttackMask(bitPosition, true) & ~(1ULL << bitPosition);
		printBitBoard(pinRay | (1ULL << pinMap[bitPosition]));
		return pinRay != 0x00ULL ? pinRay | (1ULL << pinMap[bitPosition]) : pinRay;
	}

	uint64_t occupany = board & bishopAttackMask(bitPosition, false);
	uint64_t m = computeBishopMoves(occupany, bitPosition);

	m = (m & allyBoard) ^ m;
	printBitBoard(m);

	// preprocess stage
	// 1. rn the blocker are mentioned but we dont know which is an ally or enemey remove allies 
	// 2. implement conditions for check and pins 
	// 3. try to feed required boards rathe then the entire game state 

	return CheckBoard == 0x00ULL ? m : m & CheckBoard;
}

uint64_t MoveGenerator::kingMoves(uint64_t attackingMask, uint64_t bitPos, uint64_t allyBoard, bool king, uint8_t castlingRights, bool player)
{
	uint64_t m = computeKingMoves(bitPos);
	cout << "KING ATTACKINGMASK!!!!!!!!!!!!!!" << endl;
	printBitBoard(attackingMask);
	m = king ? m : ((m & (allyBoard | attackingMask)) ^ m) | handleCastling(allyBoard, castlingRights, player, attackingMask, bitPos);
	cout << "KING MOVES" << endl;
	printBitBoard(m);
	return m;

}

uint64_t MoveGenerator::knightMoves(uint64_t bitPos, uint64_t allyBoard, uint64_t CheckBoard, uint8_t pinMap[64])
{

	if (CheckBoard == DOUBLE_CHECK) { cout << "KING IN DOUBLE CHECK ONLY KING MOVES ALLOWED" << endl; return 0x00ULL; }

	if (pinMap[bitPos] != NO_PIN) {
		cout << "KNIGHT IS PINNED TO SQUARE: " << unsigned(pinMap[bitPos]) << endl;

		return computeKnightMoves(bitPos) & pinMap[bitPos];
	}

	uint64_t m = computeKnightMoves(bitPos);
	m = (m & allyBoard) ^ m;
	printBitBoard(m);

	return CheckBoard == 0x00ULL ? m : m & CheckBoard;
}

uint64_t MoveGenerator::queenMoves(uint64_t a, uint64_t bitPos, uint64_t allyBoard, uint64_t CheckBoard, uint8_t pinMap[64])
{
	cout << "COMPUTING QUEEN MOVES" << endl;
	if (CheckBoard == 0xFFFFFFFF77777777ULL) { cout << "KING IN DOUBLE CHECK ONLY KING MOVES ALLOWED"; return 0x00ULL; }

	uint64_t m = rookMoves(a, bitPos, allyBoard, CheckBoard, pinMap) | bishopMoves(a, bitPos, allyBoard, CheckBoard, pinMap);
	return m;
}

uint64_t MoveGenerator::rookCheck(uint64_t board, uint64_t enemyKingPos, uint64_t bitPosition, uint64_t allyBoard, uint8_t(&pinMap)[64])
{
	// all occupancies for rook moves from the bitPosition
	uint64_t checkRay = computeCheckRay(bitPosition, enemyKingPos) & rookAttackMask(bitPosition, true);
	uint64_t isPinned = checkRay & (board ^ allyBoard) & ~(1ULL << enemyKingPos); // only consider pieces between rook and king

	// check if only one piece is blocking the ray between rook and king
	if (isPinned != 0 && (isPinned & (isPinned - 1)) == 0) {
		cout << "ROOK PIN DETECTED!!!!!!!!!!!!!!" << endl;
		printBitBoard(isPinned);
		pinMap[uint8_t(pop_lsb(isPinned))] = bitPosition;
		printBitBoard(checkRay);
	}

	cout << "OCCUPANY FOR ROOK CHECK: " << endl;
	uint64_t m = rookMoves(board, bitPosition, allyBoard, 0x00ULL, pinMap);

	if (m & (1ULL << enemyKingPos)) 
		cout << "ROOK CHECK DETECTED!!!!!!!!!!!!!!" << endl; 

	return m & (1ULL << enemyKingPos) ? checkRay : 0x00ULL;
}

uint64_t MoveGenerator::bishopCheck(uint64_t board, uint64_t enemyKingPos, uint64_t bitPosition, uint64_t allyBoard, uint8_t(&pinMap)[64])
{
	uint64_t checkRay = computeCheckRay(bitPosition, enemyKingPos) & bishopAttackMask(bitPosition, true);
	uint64_t isPinned = checkRay & (board ^ allyBoard) & ~(1ULL << enemyKingPos); // only consider pieces between rook and king

	// check if only one piece is blocking the ray between rook and king (Piece is pinning some piece to the king)
	if (isPinned != 0 && (isPinned & (isPinned - 1)) == 0) {
		cout << "BISHOP PIN DETECTED!!!!!!!!!!!!!!" << endl;
		printBitBoard(isPinned);
		pinMap[uint8_t(pop_lsb(isPinned))] = bitPosition;
		printBitBoard(checkRay);
	}

	cout << "OCCUPANY FOR BISHOP CHECK: " << endl;
	uint64_t m = bishopMoves(board, bitPosition, allyBoard, 0x00ULL, pinMap);

	return m & (1ULL << enemyKingPos) ? checkRay : 0x00ULL;
}

uint64_t MoveGenerator::knightCheck(uint64_t board, uint64_t enemyKingPos, uint64_t bitPos, uint64_t allyBoard, uint8_t(&pinMap)[64])
{
	uint64_t m = computeKnightMoves(bitPos);
	m = (m & allyBoard) ^ m;

	if (m & 1ULL << enemyKingPos) { cout << "KNIGHT CHECK DETECTED!!!!!!!!!!!!!!" << endl; printBitBoard(1ULL << enemyKingPos); }

	return m & 1ULL << enemyKingPos ? 1ULL << bitPos  : 0x00ULL;
}

uint64_t MoveGenerator::PawnCheck(uint64_t board, uint64_t enemyKingPos, uint64_t bitPosition, uint64_t allyBoard, uint8_t(&pinMap)[64], bool player)
{
	uint64_t m = computePawnAttacks(bitPosition, player) & board & allyBoard;

	if (m & 1ULL << enemyKingPos) { cout << "PAWN CHECK DETECTED!!!!!!!!!!!!!!" << endl; printBitBoard(1ULL << enemyKingPos); }

	return m & 1ULL << enemyKingPos ? 1ULL << bitPosition : 0x00ULL;
}

uint64_t MoveGenerator::queenCheck(uint64_t board, uint64_t enemyKingPos, uint64_t bitPos, uint64_t allyBoard, uint8_t(&pinMap)[64])
{
	return bishopCheck(board, enemyKingPos, bitPos, allyBoard, pinMap) | rookCheck(board, enemyKingPos, bitPos, allyBoard, pinMap);
}

void MoveGenerator::printBitBoard_MG(uint64_t bb)
{
	printBitBoard(bb);
}

uint64_t MoveGenerator::handleCastling(uint64_t& allyBoard, uint8_t& castlingRights, bool& player, uint64_t& attackingMask, uint8_t bitPos)
{
	uint64_t m = 0x00ULL;

	if (player == idx(Turn::WHITE)) {

		if ((1ULL << bitPos & attackingMask) == 0) {

			if ((allyBoard & 0x000000000000000EULL) == 0 && (attackingMask & 0x000000000000000CULL) == 0 && castlingRights & CastlingRights::WHITE_QUEEN_SIDE) {
				m |= 0x0000000000000004ULL;
				cout << "WHITE QUEEN SIDE CASTLING AVAILABLE!!!!!!!!!!!!!!" << endl;
			}

			if ((allyBoard & 0x0000000000000060ULL) == 0 && (attackingMask & 0x0000000000000060ULL) == 0 && castlingRights & CastlingRights::WHITE_KING_SIDE) {
				m |= 0x0000000000000040ULL;
				cout << "WHITE KING SIDE CASTLING AVAILABLE!!!!!!!!!!!!!!" << endl;
			}
		}
		else {
			cout << "WHITE KING IN CHECK CANT CASTLE!!!!!!!!!!" << endl;
		}
	}

	else {
		if ((1ULL << bitPos & attackingMask) == 0) {

			if ((allyBoard & 0x0E00000000000000ULL) == 0 && (attackingMask & 0x0C00000000000000ULL) == 0 && castlingRights & CastlingRights::BLACK_QUEEN_SIDE) {
				m |= 0x0400000000000000ULL;
				cout << "BLACK QUEEN SIDE CASTLING AVAILABLE!!!!!!!!!!!!!!" << endl;
			}

			if ((allyBoard & 0x6000000000000000ULL) == 0 && (attackingMask & 0x6000000000000000ULL) == 0 && castlingRights & CastlingRights::BLACK_KING_SIDE) {
				m |= 0x4000000000000000ULL;
				cout << "BLACK KING SIDE CASTLING AVAILABLE!!!!!!!!!!!!!!" << endl;
			}
		}
		else {
			cout << "BLACK KING IN CHECK CANT CASTLE!!!!!!!!!!" << endl;
		}
	}

	return m;
}
