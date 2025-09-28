#pragma once

#include "Pieces.h"

class Queen : public Piece {
	sf::Vector2i Pinning;
public:
	std::vector<sf::Vector2i> legalMoves(const sf::Vector2i& from, GameState state) override;

	Queen(string path, Turn color_);
};
