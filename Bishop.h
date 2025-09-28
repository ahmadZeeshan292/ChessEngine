#pragma once

#include "Pieces.h"

class Bishop : public Piece {
	sf::Vector2i Pinning;
public:
	std::vector<sf::Vector2i> legalMoves(const sf::Vector2i& from, GameState state) override;

	Bishop(string path, Turn color_);
};
