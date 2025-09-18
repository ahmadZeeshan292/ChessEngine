#include "MoveGenerator.h"
#include "ChessBoard.h"
#include <iostream>


std::vector<sf::Vector2i> MoveGenerator::rookMoves(const sf::Vector2i& from)
{
	std::vector<sf::Vector2i> moves;

	static const vector<sf::Vector2i> offset = {
		{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
	};

	// if piece absolute pinned return empty set
	if (!ChessBoard::Chessboard->board[from.x][from.y]->CanMove) {
		return std::vector<sf::Vector2i>();
	}

	// if piece relative pinned return all cells from - to 
	if (ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece != std::pair<sf::Vector2i, sf::Vector2i>()) {
		sf::Vector2i pos;
		sf::Vector2i to = ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece.first;
		sf::Vector2i placement = ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece.second;

		pinPieceLogic(from, pos, placement, false);

		pos = from + placement;
		while (pos != to + placement) {
			moves.push_back(pos);
			pos += placement;
		}

		return moves;
	}

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

				pinPieceLogic(from, pos, placements, true);

				break;
			}
		}
	}
	return moves;
}

void MoveGenerator::pinPieceLogic(sf::Vector2i from, sf::Vector2i pos, sf::Vector2i placements, bool type)
{
	sf::Vector2i npos = pos + placements;

	sf::Vector2i enemyKing = ChessBoard::Chessboard->board[from.x][from.y]->getColor() == Turn::BLACK ? ChessBoard::Chessboard->BlackKing.second : ChessBoard::Chessboard->WhiteKing.second;

	/*

	Loop checks first if king lies in the current path
	if it does check the path if its obstructed by any piece
	if piece found other than king break loop
	else if king found without obstruction
	give the other piece pinnning piece position and pinning placement

	*/
	if (ChessBoard::Chessboard->KinginPath(from, pos, placements)) {
		while (ChessBoard::InBounds(npos)) {
			if (ChessBoard::Chessboard->board[npos.x][npos.y]) {
				if (ChessBoard::Chessboard->board[npos.x][npos.y]->pieceType == ChessPiece::KING && ChessBoard::Chessboard->board[from.x][from.y]->getColor() != ChessBoard::Chessboard->board[npos.x][npos.y]->getColor()) {
					if (ChessBoard::Chessboard->board[from.x][from.y]->pieceType == ChessPiece::ROOK || ChessBoard::Chessboard->board[from.x][from.y]->pieceType == ChessPiece::QUEEN) {
						std::cout << "PIECE PINNED!!!!" << pos.x << ", " << pos.y << " | " << placements.x << ", " << placements.y << endl;
						ChessBoard::Chessboard->board[pos.x][pos.y]->PinningPiece = std::pair<sf::Vector2i, sf::Vector2i>(sf::Vector2i({ from }), sf::Vector2i({ -placements }));
					}
				}
				else {
					break;
				}
			}
			npos = npos + placements;
		}
	}
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

std::vector<sf::Vector2i> MoveGenerator::PawnAttackMoves(const sf::Vector2i& from)
{
	std::vector<sf::Vector2i> moves;

	int direction = (ChessBoard::Chessboard->board[from.x][from.y]->getColor() == Turn::WHITE ? 1 : -1);

	bool enemyPieceRightDiagonal = ChessBoard::InBounds(sf::Vector2i({ from.x + direction, from.y + direction }));

	bool enemyPieceLeftDiagonal = ChessBoard::InBounds(sf::Vector2i({ from.x + direction, from.y - direction }));

	if (enemyPieceRightDiagonal)
		moves.push_back(sf::Vector2i({ from.x + direction, from.y + direction }));

	if (enemyPieceLeftDiagonal)
		moves.push_back(sf::Vector2i({ from.x + direction, from.y - direction }));

	return moves;
}

std::vector<sf::Vector2i> MoveGenerator::KingMoves(const sf::Vector2i& from)
{
	vector<sf::Vector2i> moves;

	vector<sf::Vector2i> offsets = {
		{1, 0}, {-1, 0}, {1, 1}, {-1, -1}, 
		{0, 1}, {0, -1}, {1, -1}, {-1, 1}
	};

	Turn color = ChessBoard::Chessboard->board[from.x][from.y]->color == Turn::BLACK ? Turn::WHITE : Turn::BLACK;

	const vector<sf::Vector2i>& seenBlocks = ChessBoard::Chessboard->GenerateAllLegalMoves(color);

	for (auto& move : offsets) {
		sf::Vector2i pos = from + move;

		if (ChessBoard::InBounds(pos)) {
			if ((ChessBoard::Chessboard->board[pos.x][pos.y] && color == ChessBoard::Chessboard->board[pos.x][pos.y]->color) || !ChessBoard::Chessboard->board[pos.x][pos.y]) {
				if (find(seenBlocks.begin(), seenBlocks.end(), pos) == seenBlocks.end())
					moves.push_back(pos);
			}
		}
	}

	return moves;
}

std::vector<sf::Vector2i> MoveGenerator::KingPossibleMoves(const sf::Vector2i& from)
{
	vector<sf::Vector2i> moves;

	vector<sf::Vector2i> offsets = {
		{1, 0}, {-1, 0}, {1, 1}, {-1, -1},
		{0, 1}, {0, -1}, {1, -1}, {-1, 1}
	};

	for (auto& move : offsets) {
		sf::Vector2i pos = from + move;

		if (ChessBoard::InBounds(pos) && !ChessBoard::Chessboard->board[pos.x][pos.y]) {
				moves.push_back(pos);
		}
	}

	return moves;
}

