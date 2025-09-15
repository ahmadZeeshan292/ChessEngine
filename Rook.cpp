#include "Rook.h"

std::vector<sf::Vector2i> Rook::legalMoves(const sf::Vector2i& from)
{
	return MoveGenerator::rookMoves(from);
}

Rook::Rook(string path, Turn color_) : Piece(path, color_, ChessPiece::ROOK) {}
