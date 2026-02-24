#include "BitBoard_ChessBoard_visuals.h"
#include <iostream>
ChessBoard_BitBoard::ChessBoard_BitBoard() : HEIGHT(800), WIDTH(800) {

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

    player = Turn::WHITE;

	InitializeMapping();
}

void ChessBoard_BitBoard::InitializeMapping()
{
    for (int color = 0; color < 2; color++) {
        for (int piece = 0; piece < 6; piece++) {
			int idx = color == 1 ? piece + 6 : piece;
            spriteBitBoardMap[idx] = Sprite_BitBoard_Mapping(Board.Boards[color][piece]->filepath, Board.Boards[color][piece]->board);
        }
    }
}
