#pragma once

#include "Pieces.h"

class Bishop : public Piece {
public:
	std::vector<sf::Vector2i> legalMoves(const sf::Vector2i& from) override;

	Bishop(string path, Turn color_);
};
