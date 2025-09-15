#include "SFML/Graphics.hpp"
#include "Game.h"
#include "ChessBoard.h"

int main() {

	ChessBoard* Chessboard = new ChessBoard();

	Game* game = new Game();
	game->Update();
}
