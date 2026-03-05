#pragma once

#include<SFML/Graphics.hpp>
#include "Enums.h"
#include "BitBoard.h"
#include "Sprite_BitBoard_Mapping.h"

class ChessBoard_BitBoard {
public:
	int HEIGHT;
	int WIDTH;

	Turn player;

	ChessBitBoards Board;

	sf::Sprite WhiteTile;
	sf::Sprite BlackTile;
	sf::Sprite highlightTile;

	sf::Texture blackTileTexture;
	sf::Texture whiteTileTexture;
	sf::Texture highlightTileTexture;

	unordered_map<int, Sprite_BitBoard_Mapping> spriteBitBoardMap;

	ChessBoard_BitBoard();

	void InitializeMapping();

	void updateMapping();

	static bool InBounds(sf::Vector2i coordinates);

	Turn Player();

	void loadDefaultPosition();
};
