#include "MoveGenerator.h"
#include "ChessBoard.h"
#include <iostream>


std::vector<sf::Vector2i> MoveGenerator::rookMoves(const sf::Vector2i& from, GameState state)
{

	const vector<sf::Vector2i> offset = {
		{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
	};

	King* pieceKing = ChessBoard::Chessboard->board[from.x][from.y]->color == Turn::BLACK ? ChessBoard::Chessboard->BlackKing.first : ChessBoard::Chessboard->WhiteKing.first;

	return longRangeAttackerMoves(offset, from, ChessPiece::ROOK, state);
}

std::vector<sf::Vector2i> MoveGenerator::bishopMoves(const sf::Vector2i& from, GameState state)
{
	std::vector<sf::Vector2i> moves;

	static const std::vector<sf::Vector2i> offsets = {
		 {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
	};

	return longRangeAttackerMoves(offsets, from, ChessPiece::BISHOP, state);
}

std::vector<sf::Vector2i> MoveGenerator::knightMoves(const sf::Vector2i& from, GameState state)
{
	static const std::vector<sf::Vector2i> offsets = {
			{2, 1}, {2, -1}, {-2, 1}, {-2, -1},
			{1, 2}, {1, -2}, {-1, 2}, {-1, -2}
	};

	/*if (ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece != std::pair<sf::Vector2i, sf::Vector2i>()) {
		for (auto& off : offsets) {
			sf::Vector2i to = from + off;
			if (ChessBoard::InBounds(sf::Vector2i(to))) {
				if (off == ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece.first) {
					return vector<sf::Vector2i>{{off}};
				}
			}
		}
		return vector<sf::Vector2i>{{}};
	}*/

	std::vector<sf::Vector2i> moves;
	Turn pieceColor = ChessBoard::Chessboard->board[from.x][from.y]->color;
	King* pieceKing = pieceColor == Turn::BLACK ? ChessBoard::Chessboard->BlackKing.first : ChessBoard::Chessboard->WhiteKing.first;


	for (auto& off : offsets) {
		sf::Vector2i to = from + off;

		if (pieceKing && pieceKing->inCheck) {
			if (pieceKing->CheckingPieces.first == to) {
				moves.push_back(to);
			}
		}

		else if (ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece != std::pair<sf::Vector2i, sf::Vector2i>()) {
			if (to == ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece.first) {
				return vector<sf::Vector2i>{to};
			}
		}

		else if (ChessBoard::InBounds(sf::Vector2i(to))) {
			if (!ChessBoard::Chessboard->board[to.x][to.y])
				moves.push_back(to);
			else if (ChessBoard::Chessboard->board[to.x][to.y]->getColor() != ChessBoard::Chessboard->board[from.x][from.y]->getColor()) {
				if (ChessBoard::Chessboard->board[to.x][to.y]->pieceType == ChessPiece::KING) {
					King* opposingKing = pieceColor == Turn::WHITE ? ChessBoard::Chessboard->BlackKing.first : ChessBoard::Chessboard->WhiteKing.first;

					if (opposingKing) {
						opposingKing->beingCheck(pair<sf::Vector2i, sf::Vector2i>{from, sf::Vector2i(-1, -1)});
						opposingKing->inCheck = true;
						opposingKing->checkmate = true;
					}
				}
				moves.push_back(to);
			}
		}
	}

	if (pieceKing) {
		if (pieceKing->inCheck) {
			if (state == GameState::GAME && !moves.empty())
				pieceKing->checkmate = false;
			return moves;
		}
	}

	return moves;
}

std::vector<sf::Vector2i> MoveGenerator::QueenMoves(const sf::Vector2i& from, GameState state)
{
	vector<sf::Vector2i> rook = MoveGenerator::rookMoves(from, state);
	vector<sf::Vector2i> bishop = MoveGenerator::bishopMoves(from, state);

	for (auto move : rook) {
		if (find(bishop.begin(), bishop.end(), move) == bishop.end())
			bishop.push_back(move);
	}

	return bishop;
}

std::vector<sf::Vector2i> MoveGenerator::PawnMoves(const sf::Vector2i& from, bool hasMoved, const sf::Vector2i& enpassantTarget, GameState state)
{
	// missing en-passant and bounds for now
	std::vector<sf::Vector2i> moves;
	int direction = (ChessBoard::Chessboard->board[from.x][from.y]->getColor() == Turn::WHITE ? 1 : -1);

	vector<sf::Vector2i> moveOffsets = { {direction, 0} };
	vector<sf::Vector2i> attackOffsets = { {direction, -direction}, {direction, direction } };
	Turn pieceColor = ChessBoard::Chessboard->board[from.x][from.y]->color;
	King* pieceKing = pieceColor == Turn::BLACK ? ChessBoard::Chessboard->BlackKing.first : ChessBoard::Chessboard->WhiteKing.first;

	for (auto& move : attackOffsets) {
		sf::Vector2i pos = move + from;

		if (pieceKing && pieceKing->inCheck) {
			if (pieceKing->CheckingPieces.first == pos) {
				moves.push_back(pos);
			}
		}

		else if (ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece != std::pair<sf::Vector2i, sf::Vector2i>()) {
			if (pos == ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece.first) {
				return vector<sf::Vector2i>{pos};
			}
		}

		else {
			if (ChessBoard::InBounds(pos) && ChessBoard::Chessboard->board[pos.x][pos.y] 
				&& ChessBoard::Chessboard->board[pos.x][pos.y]->getColor() != ChessBoard::Chessboard->board[from.x][from.y]->getColor()) {

				if (ChessBoard::Chessboard->board[pos.x][pos.y]->pieceType == ChessPiece::KING) {
					King* opposingKing = pieceColor == Turn::WHITE ? ChessBoard::Chessboard->BlackKing.first : ChessBoard::Chessboard->WhiteKing.first;

					if (opposingKing) {
						opposingKing->beingCheck(pair<sf::Vector2i, sf::Vector2i>{from, { -1, -1 }});
						opposingKing->inCheck = true;
						opposingKing->checkmate = true;
					}
				}

				moves.push_back(sf::Vector2i(pos));
			}
		}
	}

	if (pieceKing) {
		if (pieceKing->inCheck) {
			if (state == GameState::GAME && !moves.empty())
				pieceKing->checkmate = false;
			return moves;
		}
	}

	if (ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece != std::pair<sf::Vector2i, sf::Vector2i>())
		return moves;

	for (auto& move : moveOffsets) {
		sf::Vector2i pos = move + from;

		if (!ChessBoard::Chessboard->board[pos.x][pos.y])
			moves.push_back(sf::Vector2i(pos));

		if (!hasMoved) {
			pos = move * 2 + from;
			if (!ChessBoard::Chessboard->board[pos.x][pos.y]) {
				moves.push_back(sf::Vector2i(pos));
			}
		}
	}

	if (enpassantTarget != sf::Vector2i())
		moves.push_back(sf::Vector2i{ enpassantTarget.x + direction, enpassantTarget.y });

	return moves;
}

std::vector<sf::Vector2i> MoveGenerator::PawnAttackMoves(const sf::Vector2i& from)
{
	std::vector<sf::Vector2i> moves;
	int direction = (ChessBoard::Chessboard->board[from.x][from.y]->getColor() == Turn::WHITE ? 1 : -1);

	bool enemyPieceRightDiagonal = ChessBoard::InBounds(sf::Vector2i({ from.x + direction, from.y + direction }));
	bool enemyPieceLeftDiagonal = ChessBoard::InBounds(sf::Vector2i({ from.x + direction, from.y - direction }));

	if (ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece != std::pair<sf::Vector2i, sf::Vector2i>()) {
		if (ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece.first == sf::Vector2i({ from.x + direction, from.y + direction })) {
			return vector<sf::Vector2i>((from.x + direction, from.y + direction ));
		}

		if (ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece.first == sf::Vector2i({ from.x + direction, from.y - direction })) {
			return vector<sf::Vector2i>((from.x + direction, from.y - direction ));
		}
	}

	if (enemyPieceRightDiagonal)
		moves.push_back(sf::Vector2i({ from.x + direction, from.y + direction }));

	if (enemyPieceLeftDiagonal)
		moves.push_back(sf::Vector2i({ from.x + direction, from.y - direction }));

	return moves;
}

std::vector<sf::Vector2i> MoveGenerator::KingMoves(const sf::Vector2i& from, GameState state)
{
	if (!ChessBoard::Chessboard->board[from.x][from.y]) {
		return std::vector<sf::Vector2i>();
	}
	vector<sf::Vector2i> moves;

	vector<sf::Vector2i> offsets = {
		{1, 0}, {-1, 0}, {1, 1}, {-1, -1}, 
		{0, 1}, {0, -1}, {1, -1}, {-1, 1}
	};

	Turn color = ChessBoard::Chessboard->board[from.x][from.y]->color == Turn::BLACK ? Turn::WHITE : Turn::BLACK;
	
	/*implement if color say opposite and king is in check return all pieces and there moves intersecting, blocking or taking that piece else no other piece is allowed to move*/
	/*if (color == Turn::BLACK) {
		if (ChessBoard::Chessboard->WhiteKing.first->inCheck) {
			const vector<sf::Vector2i>& seenBlocks = ChessBoard::Chessboard->GenerateAllLegalMoves(Turn::WHITE);
		}
	}
	else {
		if (ChessBoard::Chessboard->BlackKing.first->inCheck) {
			const vector<sf::Vector2i>& seenBlocks = ChessBoard::Chessboard->GenerateAllLegalMoves(Turn::BLACK);
		}
	}*/
	const vector<sf::Vector2i>& seenBlocks = ChessBoard::Chessboard->GenerateAllLegalMoves(color, state);
	King* pieceKing = color == Turn::BLACK ? ChessBoard::Chessboard->BlackKing.first : ChessBoard::Chessboard->WhiteKing.first;

	/*implement the check logic here have to call generate all legalmoves for the same color and check if an piece can 
		i)   intersect
		ii)  block
		iii) take
	the checking piece blocks are possible for all pieces except knight
	*/

	for (auto& move : offsets) {
		sf::Vector2i pos = from + move;

		if (ChessBoard::InBounds(pos)) {
			if ((ChessBoard::Chessboard->board[pos.x][pos.y] && color == ChessBoard::Chessboard->board[pos.x][pos.y]->color) || !ChessBoard::Chessboard->board[pos.x][pos.y]) {
				if (find(seenBlocks.begin(), seenBlocks.end(), pos) == seenBlocks.end())
					moves.push_back(pos);
			}
		}
	}

	if (!moves.empty()) {
		pieceKing->checkmate = false;
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

std::vector<sf::Vector2i> MoveGenerator::longRangeAttackerMoves(const std::vector<sf::Vector2i> offset, const sf::Vector2i& from, ChessPiece type, GameState state)
{
	Turn pieceColor = ChessBoard::Chessboard->board[from.x][from.y]->color;
	King* pieceKing = pieceColor == Turn::BLACK ? ChessBoard::Chessboard->BlackKing.first : ChessBoard::Chessboard->WhiteKing.first;

	if (pieceKing) {
		if (pieceKing->inCheck) {
			auto moves = MoveGenerator::PieceBlockLogic(offset, from, pieceColor, pieceKing);
			if (state == GameState::GAME && !moves.empty()) {
				pieceKing->checkmate = false;
			}
			return moves;
		}
		else if (pieceKing->doubleCheck) 
			return vector<sf::Vector2i>();
	}

	std::vector<sf::Vector2i> moves;

	if (state == GameState::PLAYER) {
		// if piece relative pinned return all cells from - to 
		if (ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece != std::pair<sf::Vector2i, sf::Vector2i>()) {
			return MoveGenerator::applyPinConstraints(offset, from, pieceColor);
		}
	}
	else if (ChessBoard::Chessboard->board[from.x][from.y]->pinnedPiece != sf::Vector2i()) {
		sf::Vector2i idx = ChessBoard::Chessboard->board[from.x][from.y]->pinnedPiece;
		ChessBoard::Chessboard->board[idx.x][idx.y]->PinningPiece = pair<sf::Vector2i, sf::Vector2i>();
		ChessBoard::Chessboard->board[from.x][from.y]->pinnedPiece = sf::Vector2i();
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
				/*king check logic */
				if (ChessBoard::Chessboard->board[pos.x][pos.y]->pieceType == ChessPiece::KING) {
					King* opposingKing = pieceColor == Turn::WHITE ? ChessBoard::Chessboard->BlackKing.first : ChessBoard::Chessboard->WhiteKing.first;

					if (opposingKing) {
						opposingKing->beingCheck(pair<sf::Vector2i, sf::Vector2i>{from, placements});
						opposingKing->inCheck = true;
						opposingKing->checkmate = true;
					}
				}

				moves.push_back(pos);
				pinPieceLogic(from, pos, placements, type);
				break;
			}
		}
	}
	return moves;
}

std::vector<sf::Vector2i> MoveGenerator::PieceBlockLogic(const std::vector<sf::Vector2i> offsets, const sf::Vector2i& from, Turn& pieceColor, King* pieceKing)
{

	/* (Checking king perspective)
	two things to cater intersect(block) only for longRangeAttackers for shortRange see if they lie in the path of the selected piece and take
	Implemenation:
	Assign clear labels to the shortRange and longRange attackers
	if they shortRange see if curPos+offset = AttackingPieceStartingPos
	if they longRange extend the ray to check for intersecting point or if they can potencially take
		for intersecting case also find the point of inetersection

	*/

	// logic shouldnt invoke for shortRangeAttackers aka knights and pawns
	if (pieceKing->CheckingPieces.second == sf::Vector2i(-1, -1)) { // 
		return std::vector<sf::Vector2i>();
	}

	Ray attackingPiece(pieceKing->CheckingPieces.first, pieceKing->CheckingPieces.second);

	// opposite color bishops and queens cannt instersect each other)
	if (ChessBoard::Chessboard->board[attackingPiece.start.x][attackingPiece.start.y]->pieceType == ChessPiece::QUEEN ||
		ChessBoard::Chessboard->board[attackingPiece.start.x][attackingPiece.start.y]->pieceType == ChessPiece::BISHOP &&
		ChessBoard::Chessboard->board[from.x][from.y]->pieceType == ChessPiece::BISHOP ||
		ChessBoard::Chessboard->board[from.x][from.y]->pieceType == ChessPiece::QUEEN) {
		if ((attackingPiece.start.x + from.x) % 2 != (attackingPiece.start.y + from.y) % 2) {
			return std::vector<sf::Vector2i>();
		}
	}

	std::vector<sf::Vector2i> blockingMoves;
	sf::Vector2i piece{ from.x, from.y };
	for (auto of : offsets) {
		Ray selectedPiece(piece, of);
		sf::Vector2i PointOfIntersection = selectedPiece.raysIntersect(selectedPiece, attackingPiece);
		if (PointOfIntersection != sf::Vector2i()) { // two rays can only intersect at one point
			blockingMoves.push_back(PointOfIntersection);
			cout << "PIECE CAN BLOCK!!! " << PointOfIntersection.x << ", " << PointOfIntersection.y << endl;
			break;
		}
		/* Piece take logic*/
		else {
			// if (from.x) // check if the attacking and selected piece lie in the same file
		}
	}
	return blockingMoves;
}

std::vector<sf::Vector2i> MoveGenerator::applyPinConstraints(const std::vector<sf::Vector2i> offsets, const sf::Vector2i& from, Turn& pieceColor)
{
	std::vector<sf::Vector2i> moves;

	sf::Vector2i pinningPiece = ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece.first;
	sf::Vector2i placement = ChessBoard::Chessboard->board[from.x][from.y]->PinningPiece.second;

	// means pieces are different needed for queen-rook and queen-bishop offsets
	// it send empty for rook-bishop pins

	if (find(offsets.begin(), offsets.end(), placement) == offsets.end()) 
		return vector<sf::Vector2i>();

	sf::Vector2i pos;

	// check if this can see an other piece which can be potencially pinned
	ChessPiece type = ChessBoard::Chessboard->board[from.x][from.y]->pieceType;

	pinPieceLogic(from, pinningPiece, placement, type);

	pos = from + placement;
	while (pos != pinningPiece + placement) {
		moves.push_back(pos);
		pos += placement;
	}
	return moves;
}

/*
Used only by long range Attackers
*/

void MoveGenerator::pinPieceLogic(sf::Vector2i from, sf::Vector2i pos, sf::Vector2i placements, ChessPiece type)
{
	/*ReImplement this via Ray Logic*/
	/*
	from = piece rather then king
	to   = king position

	*/
	sf::Vector2i npos = pos + placements;

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
					if (ChessBoard::Chessboard->board[from.x][from.y]->pieceType == type || ChessBoard::Chessboard->board[from.x][from.y]->pieceType == ChessPiece::QUEEN) {
						std::cout << "PIECE PINNED!!!!" << pos.x << ", " << pos.y << " | " << placements.x << ", " << placements.y << endl;
						ChessBoard::Chessboard->board[pos.x][pos.y]->PinningPiece = std::pair<sf::Vector2i, sf::Vector2i>(sf::Vector2i({ from }), sf::Vector2i({ -placements }));
						ChessBoard::Chessboard->board[from.x][from.y]->pinnedPiece = sf::Vector2i({ pos });
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

/* 
TODO:
implement how can a piece pinning an particular piece be track and it knows what piece its pinning maybe add an third arg corresponding to the 
pinned piece argument as in c_pair<sf::vector2i, sf::vector2i, sf::vector2i> corresponding to the pinningPiece pos, its placement and the 
pinned piece

issue with current implementation: its hardcoded an pinned piece can regardless see those squares meaning can pinned piece can also pin
an other piece despite being pinned and still restricts the blocks it see for the king 

also see how to implement king check logic and cater for
	king taking the attacking piece
	some random piece blocking 
	king moving out of the way
double check
	king taking one of the attcking piece (provided its not defended)
	king moving out of the way
*/
