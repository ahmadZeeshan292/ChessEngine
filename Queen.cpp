#include "Queen.h"
#include "ChessBoard.h"

std::vector<sf::Vector2i> Queen::legalMoves(const sf::Vector2i& from)
{
    if(ChessBoard::Chessboard->board[from.x][from.y]->pieceType == ChessPiece::QUEEN)
        return MoveGenerator::QueenMoves(from);

    return vector <sf::Vector2i> ();
}

Queen::Queen(string path, Turn color_) : Piece(path, color_, ChessPiece::QUEEN) {}
