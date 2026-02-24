#pragma once

#include "Utility_Class.h"
#include<vector>
#include "Pieces.h"
#include "Bishop.h"
#include "Rook.h"
#include "Queen.h"
#include "Pawn.h"
#include "King.h"
#include "Knight.h"

class ChessBoard {
public:
	int HEIGHT;
	int WIDTH;

	vector<vector<Piece*>> board;
	Turn player;

	static ChessBoard* Chessboard;

	sf::Sprite WhiteTile;
	sf::Sprite BlackTile;
	sf::Sprite highlightTile;

	sf::Texture blackTileTexture;
	sf::Texture whiteTileTexture;
	sf::Texture highlightTileTexture;

	std::pair<King*, sf::Vector2i> WhiteKing;
	std::pair<King*, sf::Vector2i> BlackKing;

	ChessBoard();

	void InitializeBoard();

	static bool InBounds(sf::Vector2i coordinates);

	Turn Player();

	vector<sf::Vector2i> GenerateAllLegalMoves(Turn color_, GameState state);

	bool KinginPath(sf::Vector2i sourcePiece, sf::Vector2i destinationPiece, sf::Vector2i placement);

	string getBoardState();

	void loadDefaultPosition();
};
