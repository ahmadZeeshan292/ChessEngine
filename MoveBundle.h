#pragma once

class Piece;  

#include <vector>      
#include <SFML/System.hpp>  

struct MoveBundle {
    Piece* piece;                     
    std::vector<sf::Vector2i> moves;  

    MoveBundle(Piece* _piece, std::vector<sf::Vector2i> _moves) : piece(_piece), moves(_moves){}
};
