#include "ChessBoard.h"

std::vector<sf::Vector2i> Rook::legalMoves(const sf::Vector2i& from)
{
	
	if (ChessBoard::Chessboard->board[from.x][from.y]->pieceType == ChessPiece::ROOK) {
		return MoveGenerator::rookMoves(from);
	}
	return vector<sf::Vector2i>();
}

Rook::Rook(string path, Turn color_) : Piece(path, color_, ChessPiece::ROOK) {}
