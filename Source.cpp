
#include "BitBoard_Game.h"
#include "BitBoard_ChessBoard_visuals.h"
#include "BB_MagicAlgorithm.h"
#include <iostream>
int main() {


	
		uint64_t att_mask = rookLegalMoves(0x01000000000000E7ULL, 0);
		printBitBoard(att_mask);
	
	forwardMagic();

	

	find_mf_index();

	uint64_t o = 0x008000000000007EULL;
	printBitBoard(rookLegalMoves(o, 8));
	//ChessBoard* Chessboard = new ChessBoard();
	ChessBoard_BitBoard* bitBoard = new ChessBoard_BitBoard();

	Game* game = new Game(bitBoard);
	game->Update(bitBoard);

	

	//printBitBoard(compute_occupany(90, rookAttackMask(27), count_ones(rookAttackMask(27))));
	//printBitBoard(att_mask);
	//printBitBoard(att_mask);
}
