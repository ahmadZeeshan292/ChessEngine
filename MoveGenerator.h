#pragma once

#include <vector>
#include "Enums.h"
#include<SFML/Graphics.hpp>

class King;

namespace MoveGenerator {

    std::vector<sf::Vector2i> rookMoves(const sf::Vector2i& from, GameState state);

    std::vector<sf::Vector2i> bishopMoves(const sf::Vector2i& from, GameState state);

    std::vector<sf::Vector2i> knightMoves(const sf::Vector2i& from);

    std::vector<sf::Vector2i> QueenMoves(const sf::Vector2i& from, GameState state);

    std::vector<sf::Vector2i> PawnMoves(const sf::Vector2i& from, bool hasMoved);
    std::vector<sf::Vector2i> PawnAttackMoves(const sf::Vector2i& from);

    std::vector<sf::Vector2i> KingMoves(const sf::Vector2i& from);
    std::vector<sf::Vector2i> KingPossibleMoves(const sf::Vector2i& from);

    void pinPieceLogic(sf::Vector2i from, sf::Vector2i pos, sf::Vector2i placement, ChessPiece type);
    std::vector<sf::Vector2i> longRangeAttackerMoves(const std::vector<sf::Vector2i> offsets, const sf::Vector2i& from, ChessPiece type, GameState state);

    std::vector<sf::Vector2i> PieceBlockLogic(const std::vector<sf::Vector2i> offsets, const sf::Vector2i& from, Turn& pieceColor, King* pieceKing);

    std::vector<sf::Vector2i> FinalWorkFlow(const std::vector<sf::Vector2i> offsets, const sf::Vector2i& from, ChessPiece type, GameState state, Attacker attackingType);

    std::vector<sf::Vector2i> applyPinConstraints(const std::vector<sf::Vector2i> offsets, const sf::Vector2i& from, Turn& pieceColor);
}