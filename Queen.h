#pragma once

#include "Pieces.h"

class Queen : public Piece {
public:
	std::vector<sf::Vector2i> legalMoves(const sf::Vector2i& from) override;

	Queen(string path, Turn color_);
};
