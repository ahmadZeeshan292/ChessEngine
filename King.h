#pragma once

#include "Pieces.h"

class King : public Piece {
	bool inCheck;
	vector<sf::Vector2i> CheckingPieces;
public:

	std::vector<sf::Vector2i> legalMoves(const sf::Vector2i& from);

	King(string path, Turn _color);

	void beingCheck(vector<sf::Vector2i> selected);

	void setInCheck(bool val) { inCheck = val; }

};