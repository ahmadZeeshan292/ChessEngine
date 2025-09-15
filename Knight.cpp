
#include "Pieces.h"
#include "Enums.h"

class Knight : Piece {
public:
    std::vector<sf::Vector2i> legalMoves(
        const sf::Vector2i& from,
        const std::vector<std::vector<Piece*>>& board) override
};