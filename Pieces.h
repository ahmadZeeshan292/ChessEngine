#pragma once

#include<SFML/Graphics.hpp>
#include<string.h>
#include<vector>

using namespace std;

class Piece {
public:
	sf::Sprite pieceSprite;
	sf::Texture pieceTexture;
	sf::Vector2f Coordinates;

	Piece(string path);

	Piece(const Piece& piece);

	virtual sf::Sprite& GetSprite() { return pieceSprite; }

	virtual vector<sf::Vector2i> legalMoves() = 0;
};
