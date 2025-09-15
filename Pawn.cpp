#include "Pawn.h"
#include "ChessBoard.h"
#include <iostream>

std::vector<sf::Vector2i> Pawn::legalMoves(const sf::Vector2i& from)
{
	cout << from.x << ',' << from.y << endl;
	if (ChessBoard::Chessboard->board[from.x][from.y]->pieceType == ChessPiece::PAWN)
		return MoveGenerator::PawnMoves(from, this->Moved);
	return vector<sf::Vector2i>();
}

Pawn::Pawn(string path, Turn color_) : Piece(path, color_, ChessPiece::PAWN)
{
	Moved = false;
}
