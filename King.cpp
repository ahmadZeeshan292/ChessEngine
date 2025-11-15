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
	CheckingPieces = pair<sf::Vector2i, sf::Vector2i>();
}

void King::beingCheck(pair<sf::Vector2i, sf::Vector2i> selected)
{
	CheckingPieces = selected;
}

void King::checkKing(sf::Vector2i start, sf::Vector2i dir)
{
	CheckingPieces = pair<sf::Vector2i, sf::Vector2i>{start, dir};
	ChessBoard::Chessboard->BlackKing.first->inCheck = true;
}

