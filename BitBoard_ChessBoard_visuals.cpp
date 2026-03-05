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

// function to update sprite mapping after every move
// according to the changes inflicted in the chessBoard
// mainly for castling and en-passant where multiple pieces can move and we need to update the mapping for all of them
void ChessBoard_BitBoard::updateMapping()
{
	for (int color = 0; color < 2; color++) {
        for (int piece = 0; piece < 6; piece++) {

            if (piece == idx(ChessPiece::ROOK)) {

                cout << "Updating Mapping for " << (color == 1 ? "WHITE " : "BLACK ") << " PIECE TYPE: " << piece << endl;

                int index = color == 1 ? piece + 6 : piece;

                uint64_t board = Board.Boards[color][piece]->board;
                int oldbitPos = 0;
                int newbitPos = 0;

                for (auto item : spriteBitBoardMap[index].sprite_map) {
                    if (!((1ULL << item.first) & board)) {
                        oldbitPos = item.first;
                    }
                }

                for (int i = 0; i < 64; i++) {
                    if (board & 1ULL) {
                        if (spriteBitBoardMap[index].sprite_map.find(i) == spriteBitBoardMap[index].sprite_map.end()) {
                            newbitPos = i;
                        }
                    }
                    board = board >> 1;
                }

                unordered_map<uint64_t, Sprite_Table*>& pieceMap = spriteBitBoardMap[index].sprite_map;

                if (oldbitPos != newbitPos) {
                    pieceMap[newbitPos] = std::move(pieceMap[oldbitPos]);
                    pieceMap.erase(oldbitPos);
                    pieceMap[newbitPos]->sprite.setPosition((newbitPos / 8 + 1) * (WIDTH / 8), (newbitPos % 8) * (HEIGHT / 8));
                }
            }
        }
	}
}


