#pragma once

#include<SFML/Graphics.hpp>
#include <vector>
#include "Pieces.h"

using namespace std;

class Game {
private:
	sf::RenderWindow window;
	sf::Event e;
	sf::View view;

	vector<sf::Sprite> tileLayout;

public:
	Game();

	void Initialize();
	void Update();
	void InitializeBoard();
	void updatePieceCordinates(sf::Event& event, bool& isDragging, Piece*& selectedPiece, sf::Vector2i dragOrigin);
	void UpdatePieces();
};
