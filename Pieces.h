#pragma once

#include"MoveGenerator.h"
#include<string.h>
#include "Enums.h"

using namespace std;

class Piece {
public:
	sf::Sprite pieceSprite;
	sf::Texture pieceTexture;

	sf::Vector2f Coordinates;

	Turn color;
	ChessPiece pieceType;

	// pinning piece position and placement
	std::pair<sf::Vector2i, sf::Vector2i> PinningPiece;   
	bool CanMove;      // tell if piece can move

	Piece(string path, Turn color_, ChessPiece Type);

	Piece(const Piece& piece);

	virtual sf::Sprite& GetSprite() { return pieceSprite; }

	const Turn getColor() const { return color; }

	virtual vector<sf::Vector2i> legalMoves(const sf::Vector2i& from) = 0;
};
