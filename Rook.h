#pragma once

#include "Pieces.h"

class Rook : public Piece {
public:
	std::vector<sf::Vector2i> legalMoves(const sf::Vector2i& from, GameState state) override;

	Rook(string path, Turn color_);
};
