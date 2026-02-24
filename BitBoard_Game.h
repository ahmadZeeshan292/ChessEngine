#pragma once

#include<SFML/Graphics.hpp>

using namespace std;

class ChessBoard_BitBoard;

class Game {
private:
	sf::RenderWindow window;
	sf::Event e;
	sf::View view;

	vector<sf::Sprite> tileLayout;
	bool updateState;

public:
	Game(ChessBoard_BitBoard* a);

	void Initialize(ChessBoard_BitBoard* a);
	void Update(ChessBoard_BitBoard* a);
	void InitializeBoard(ChessBoard_BitBoard* a);
	void updatePieceCordinates(ChessBoard_BitBoard* a, sf::Event& event, int& pieceType, sf::Sprite*& draggingSprite, bool& isDragging, sf::Vector2i& dragOrigin, uint64_t legalMoves);
	/*void UpdatePieces();
	void highLightMoves(Piece* selectedPiece, sf::Vector2i& dragOrigin, const vector<sf::Vector2i>& legalMoves);*/
	void PieceDragLogic(ChessBoard_BitBoard* a, sf::Event& event, int& selectedPiece, bool& isDragging, sf::Vector2i& dragOrigin);
	void UpdatePieces(ChessBoard_BitBoard* a);
};
