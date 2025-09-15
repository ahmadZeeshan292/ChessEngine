#pragma once

#include <vector>
#include<SFML/Graphics.hpp>

namespace MoveGenerator {
    std::vector<sf::Vector2i> rookMoves(const sf::Vector2i& from);
    std::vector<sf::Vector2i> bishopMoves(const sf::Vector2i& from);
    std::vector<sf::Vector2i> knightMoves(const sf::Vector2i& from);
    std::vector<sf::Vector2i> QueenMoves(const sf::Vector2i& from);
    std::vector<sf::Vector2i> PawnMoves(const sf::Vector2i& from, bool hasMoved);
    std::vector<sf::Vector2i> KingMoves(const sf::Vector2i& from);
}
