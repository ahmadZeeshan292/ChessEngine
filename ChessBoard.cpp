
#include "ChessBoard.h"
#include "Knight.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "Pawn.h"

ChessBoard::ChessBoard() : HEIGHT(800), WIDTH(800), board(8, vector<Piece*>(8, nullptr)) {
	Chessboard = this;

    blackTileTexture.loadFromFile("Images/square gray dark _2x.png");
    whiteTileTexture.loadFromFile("Images/square gray light _2x.png");
    highlightTileTexture.loadFromFile("Images/square brown light_2x.png");

    BlackTile.setTexture(blackTileTexture);
    WhiteTile.setTexture(whiteTileTexture);
    highlightTile.setTexture(highlightTileTexture);

    BlackTile.setScale(
        float(WIDTH / 8) / blackTileTexture.getSize().x,
        float(HEIGHT / 8) / blackTileTexture.getSize().y
    );

    WhiteTile.setScale(
        float(WIDTH / 8) / whiteTileTexture.getSize().x,
        float(HEIGHT / 8) / whiteTileTexture.getSize().y
    );

    highlightTile.setScale(
        float(WIDTH / 8) / whiteTileTexture.getSize().x,
        float(HEIGHT / 8) / whiteTileTexture.getSize().y
    );

    InitializeBoard();
    player = Turn::WHITE;
}

void ChessBoard::InitializeBoard()
{
    /*board[0][0] = new Rook("Images/w_rook.png", Turn::WHITE);
    board[0][1] = new Knight("Images/w_knight.png", Turn::WHITE);
    board[0][2] = new Bishop("Images/w_bishop.png", Turn::WHITE);
    board[0][3] = new Queen("Images/w_queen.png", Turn::WHITE);
    board[0][4] = new Queen("Images/w_queen.png", Turn::WHITE);
    board[0][5] = new Bishop("Images/w_bishop.png", Turn::WHITE);
    board[0][6] = new Knight("Images/w_knight.png", Turn::WHITE);
    board[0][7] = new Rook("Images/w_rook.png", Turn::WHITE);

    for (int i = 0;i < 8; i++) {
        board[1][i] = new Pawn("Images/w_pawn.png", Turn::WHITE);
    }*/

    board[4][4] = new Knight("Images/w_knight.png", Turn::WHITE);

    /*board[7][0] = new Rook("Images/b_rook.png", Turn::BLACK);
    board[7][1] = new Knight("Images/b_knight.png", Turn::BLACK);
    board[7][2] = new Bishop("Images/b_bishop.png", Turn::BLACK);
    board[7][3] = new Queen("Images/b_Queen.png", Turn::BLACK);
    board[7][4] = new Queen("Images/b_Queen.png", Turn::BLACK);
    board[7][5] = new Bishop("Images/b_bishop.png", Turn::BLACK);
    board[7][6] = new Knight("Images/b_knight.png", Turn::BLACK);
    board[7][7] = new Rook("Images/b_rook.png", Turn::BLACK);

    for (int i = 0;i < 8; i++) {
        board[6][i] = new Pawn("Images/b_pawn.png", Turn::BLACK);
    }*/
}

bool ChessBoard::InBounds(sf::Vector2i coordinates)
{
    return (coordinates.x >= 0 && coordinates.x < 8 && coordinates.y >= 0 && coordinates.y < 8);
}

bool ChessBoard::Player()
{
    return player == Turn::WHITE;
}



ChessBoard* ChessBoard::Chessboard = nullptr;