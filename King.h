#pragma once

#include "Pieces.h"

class King : public Piece {
public:
	pair<sf::Vector2i, sf::Vector2i> CheckingPieces;

	bool inCheck;
	bool doubleCheck;

	std::vector<sf::Vector2i> legalMoves(const sf::Vector2i& from, GameState state);

	King(string path, Turn _color);

	void beingCheck(pair<sf::Vector2i, sf::Vector2i> selected);

	void checkKing(sf::Vector2i start, sf::Vector2i dir);

};