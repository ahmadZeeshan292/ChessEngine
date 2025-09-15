#pragma once
#include "Pieces.h"
class Pawn : public Piece {
	bool Moved;
public:
	std::vector<sf::Vector2i> legalMoves(const sf::Vector2i& from) override;

	Pawn(string path, Turn color_);
};
