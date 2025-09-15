
#include "Pieces.h"
#include "Enums.h"

class Knight : public Piece {
public:
    std::vector<sf::Vector2i> legalMoves(const sf::Vector2i& from) override;

    Knight(string path, Turn color_);
};
