#pragma once
#include "Pieces.h"

class Pawn : public Piece {
	bool Moved;
	sf::Vector2i enPassantTarget;
public:
	std::vector<sf::Vector2i> legalMoves(const sf::Vector2i& from, GameState state) override;

	Pawn(string path, Turn color_);

	inline void setMoved(bool _moved) { Moved = _moved; }

	inline bool hasMoved() const { return Moved; }

	inline sf::Vector2i getEnPassantTarget() const { return enPassantTarget; }

	inline void setEnPassantTarget(sf::Vector2i target) { enPassantTarget = target; }
};
