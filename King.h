#pragma once

#include "Pieces.h"

class King : public Piece {
	vector<pair<sf::Vector2i, sf::Vector2i>> CheckingPieces;
public:
	bool inCheck;

	std::vector<sf::Vector2i> legalMoves(const sf::Vector2i& from, GameState state);

	King(string path, Turn _color);

	void beingCheck(vector<pair<sf::Vector2i, sf::Vector2i>> selected);
};