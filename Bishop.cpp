#include "Bishop.h"
#include "ChessBoard.h"
std::vector<sf::Vector2i> Bishop::legalMoves(const sf::Vector2i& from) // 0, 3
{
    if (ChessBoard::Chessboard->board[from.x][from.y]->pieceType == ChessPiece::BISHOP)
        return MoveGenerator::bishopMoves(from);

    return vector<sf::Vector2i>();
}

Bishop::Bishop(string path, Turn color_) : Piece(path, color_, ChessPiece::BISHOP) {}

