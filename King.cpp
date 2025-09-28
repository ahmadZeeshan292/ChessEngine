#include "ChessBoard.h"

std::vector<sf::Vector2i> King::legalMoves(const sf::Vector2i& from, GameState state)
{
	if(ChessBoard::Chessboard->board[from.x][from.y]->pieceType == ChessPiece::KING)
		return MoveGenerator::KingMoves(from);

	return std::vector<sf::Vector2i>();
}

King::King(string path, Turn _color) : Piece(path, _color, ChessPiece::KING) 
{
	inCheck = false;
	CheckingPieces = vector<pair<sf::Vector2i, sf::Vector2i>>();
}

void King::beingCheck(vector<pair<sf::Vector2i, sf::Vector2i>> selected)
{
	for (auto& pieces : selected) {
		CheckingPieces.push_back(pieces);
	}
}

