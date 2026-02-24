#include "ChessBoard.h"

std::vector<sf::Vector2i> Pawn::legalMoves(const sf::Vector2i& from, GameState state)
{
	if (ChessBoard::Chessboard->board[from.x][from.y]->pieceType == ChessPiece::PAWN)
		return MoveGenerator::PawnMoves(from, this->Moved, this->enPassantTarget, state);
	return vector<sf::Vector2i>();
}

Pawn::Pawn(string path, Turn color_) : Piece(path, color_, ChessPiece::PAWN)
{
	Moved = false;
}
