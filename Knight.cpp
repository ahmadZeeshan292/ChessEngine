#include "ChessBoard.h"

std::vector<sf::Vector2i> Knight::legalMoves(const sf::Vector2i& from, GameState state)
{
    if (ChessBoard::Chessboard->board[from.x][from.y]->pieceType == ChessPiece::KNIGHT)
        return MoveGenerator::knightMoves(from, state);
    return vector<sf::Vector2i>();
}

Knight::Knight(std::string path, Turn color_) : Piece(path, color_, ChessPiece::KNIGHT) {}
