#pragma once

#include<SFML/Graphics.hpp>
#include<vector>
#include "Pieces.h"
#include "Enums.h"

class ChessBoard {
private:
	Turn player;
public:
	int HEIGHT;
	int WIDTH;

	vector<vector<Piece*>> board;

	static ChessBoard* Chessboard;

	sf::Sprite WhiteTile;
	sf::Sprite BlackTile;

	sf::Texture blackTileTexture;
	sf::Texture whiteTileTexture;

	ChessBoard();

	void InitializeBoard();

	bool Player();
};
