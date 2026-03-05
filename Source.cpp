
#include "BitBoard_Game.h"
#include "BitBoard_ChessBoard_visuals.h"
#include "BB_MagicAlgorithm.h"
#include <iostream>
int main() {
	
	forwardMagic();
	ChessBoard_BitBoard* bitBoard = new ChessBoard_BitBoard();

	Game* game = new Game(bitBoard);
	game->Update(bitBoard);

	

	//printBitBoard(compute_occupany(90, rookAttackMask(27), count_ones(rookAttackMask(27))));
	//printBitBoard(att_mask);
	//printBitBoard(att_mask);
}
