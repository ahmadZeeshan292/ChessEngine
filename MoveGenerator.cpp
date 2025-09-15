#include "MoveGenerator.h"
#include "ChessBoard.h"


std::vector<sf::Vector2i> MoveGenerator::rookMoves(const sf::Vector2i& from)
{
	std::vector<sf::Vector2i> moves;

	static const vector<sf::Vector2i> offset = {
		{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
	};


	for (auto& placements : offset) {
		sf::Vector2i pos = from;
		while (true) {
			pos += placements;
			if (!ChessBoard::InBounds(pos))
				break;

			if (!ChessBoard::Chessboard->board[pos.x][pos.y]) {
				moves.push_back(pos);
				continue;
			}

			if (ChessBoard::Chessboard->board[pos.x][pos.y]->getColor() == ChessBoard::Chessboard->board[from.x][from.y]->getColor())
				break;

			if (ChessBoard::Chessboard->board[pos.x][pos.y]->getColor() != ChessBoard::Chessboard->board[from.x][from.y]->getColor()) {
				moves.push_back(pos);
				break;
			}
		}
	}
	return moves;
}

std::vector<sf::Vector2i> MoveGenerator::bishopMoves(const sf::Vector2i& from)
{
	std::vector<sf::Vector2i> moves;

	static const std::vector<sf::Vector2i> directions = {
		 {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
	};

	for (auto dir : directions) {
		sf::Vector2i pos = from;

		while (true) {
			pos += dir;

			if (!ChessBoard::InBounds(pos))
				break;

			if (!ChessBoard::Chessboard->board[pos.x][pos.y]) {
				moves.push_back(pos);
				continue;
			}

			if (ChessBoard::Chessboard->board[pos.x][pos.y]->getColor() == ChessBoard::Chessboard->board[from.x][from.y]->getColor())
				break;

			if (ChessBoard::Chessboard->board[pos.x][pos.y]->getColor() != ChessBoard::Chessboard->board[from.x][from.y]->getColor()) {
				moves.push_back(pos);
				break;
			}
		}
	}

	return moves;
}

std::vector<sf::Vector2i> MoveGenerator::knightMoves(const sf::Vector2i& from)
{
	static const std::vector<sf::Vector2i> offsets = {
			{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
			{1, 2}, {1, -2}, {-1, 2}, {-1, -2}
	};

	std::vector<sf::Vector2i> moves;
	for (auto& off : offsets) {
		sf::Vector2i to = from + off;
		if (ChessBoard::InBounds(sf::Vector2i(to))) {
			if (!ChessBoard::Chessboard->board[to.x][to.y] || ChessBoard::Chessboard->board[to.x][to.y]->getColor() != ChessBoard::Chessboard->board[from.x][from.y]->getColor())
				moves.push_back(to);
		}
	}
	return moves;
}

std::vector<sf::Vector2i> MoveGenerator::QueenMoves(const sf::Vector2i& from)
{
	vector<sf::Vector2i> rook = MoveGenerator::rookMoves(from);
	vector<sf::Vector2i> bishop = MoveGenerator::bishopMoves(from);

	for (auto move : rook) {
		if (find(bishop.begin(), bishop.end(), move) == bishop.end())
			bishop.push_back(move);
	}

	return bishop;
}

std::vector<sf::Vector2i> MoveGenerator::PawnMoves(const sf::Vector2i& from, bool hasMoved)
{
	// missing en-passant and bounds for now
	std::vector<sf::Vector2i> moves;

	int direction = (ChessBoard::Chessboard->board[from.x][from.y]->getColor() == Turn::WHITE ? 1 : -1);

	if (!hasMoved) {
		if(!ChessBoard::Chessboard->board[from.x + direction][from.y])
			moves.push_back(sf::Vector2i({ from.x + direction, from.y }));

		if (!ChessBoard::Chessboard->board[from.x + 2 * direction][from.y] && !ChessBoard::Chessboard->board[from.x + direction][from.y])
			moves.push_back(sf::Vector2i({ from.x + 2 * direction , from.y }));
	}
	else if (hasMoved && !ChessBoard::Chessboard->board[from.x + direction][from.y]) {
		moves.push_back(sf::Vector2i({ from.x + direction, from.y}));
	}

	bool enemyPieceRightDiagonal = ChessBoard::InBounds(sf::Vector2i({from.x + direction, from.y + direction})) && ChessBoard::Chessboard->board[from.x + direction][from.y + direction] 
		&& ChessBoard::Chessboard->board[from.x + direction][from.y + direction]->getColor() != ChessBoard::Chessboard->board[from.x][from.y]->getColor();

	bool enemyPieceLeftDiagonal = ChessBoard::InBounds(sf::Vector2i({ from.x + direction, from.y - direction })) && ChessBoard::Chessboard->board[from.x + direction][from.y - direction] 
		&& ChessBoard::Chessboard->board[from.x + direction][from.y - direction]->getColor() != ChessBoard::Chessboard->board[from.x][from.y]->getColor();

	if (enemyPieceRightDiagonal)
		moves.push_back(sf::Vector2i({ from.x + direction, from.y + direction }));

	if (enemyPieceLeftDiagonal)
		moves.push_back(sf::Vector2i({ from.x + direction, from.y - direction }));

	return moves;
}

std::vector<sf::Vector2i> MoveGenerator::KingMoves(const sf::Vector2i& from)
{
	return std::vector<sf::Vector2i>();
}
