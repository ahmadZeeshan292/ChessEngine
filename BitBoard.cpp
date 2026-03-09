#include "BitBoard.h"
#include "BB_MoveGenerator.h"
#include <iostream>
ChessBitBoards::ChessBitBoards()
{
	Initialize_BitBoards();
}

void ChessBitBoards::Initialize_BitBoards()
{
	Boards[idx(Turn::WHITE)][idx(ChessPiece::PAWN)] = new BitBoard("Images/w_pawn.png", 0x0000000000000000ULL);
	Boards[idx(Turn::BLACK)][idx(ChessPiece::PAWN)] = new BitBoard("Images/b_pawn.png", 0x0000000000000000ULL);

	Boards[idx(Turn::WHITE)][idx(ChessPiece::KING)] = new BitBoard("Images/w_king.png", 0x0000000000000010ULL);
	Boards[idx(Turn::BLACK)][idx(ChessPiece::KING)] = new BitBoard("Images/b_king.png", 0x1000000000000000ULL);

	Boards[idx(Turn::WHITE)][idx(ChessPiece::QUEEN)] = new BitBoard("Images/w_queen.png", 0x0000000000000000ULL);
	Boards[idx(Turn::BLACK)][idx(ChessPiece::QUEEN)] = new BitBoard("Images/b_queen.png", 0x0000000000000000ULL);

	Boards[idx(Turn::WHITE)][idx(ChessPiece::ROOK)] = new BitBoard("Images/w_rook.png", 0x0000000000000000ULL);
	Boards[idx(Turn::BLACK)][idx(ChessPiece::ROOK)] = new BitBoard("Images/b_rook.png", 0x0000000000000000ULL);

	Boards[idx(Turn::WHITE)][idx(ChessPiece::BISHOP)] = new BitBoard("Images/w_bishop.png", 0x0000000000000000ULL);
	Boards[idx(Turn::BLACK)][idx(ChessPiece::BISHOP)] = new BitBoard("Images/b_bishop.png", 0x0000000000000000ULL);

	Boards[idx(Turn::WHITE)][idx(ChessPiece::KNIGHT)] = new BitBoard("Images/w_knight.png", 0x0010000000000000ULL);
	Boards[idx(Turn::BLACK)][idx(ChessPiece::KNIGHT)] = new BitBoard("Images/b_knight.png", 0x0001000000000000ULL);

	// Full Board State
	Boards[idx(Turn::BLACK)][6] = new BitBoard("", 0x00ULL);
	Boards[idx(Turn::WHITE)][6] = new BitBoard("", 0x00ULL);

	// Check Boards
	Boards[idx(Turn::BLACK)][7] = new BitBoard("", 0x00ULL);
	Boards[idx(Turn::WHITE)][7] = new BitBoard("", 0x00ULL);

	castlingRights = CastlingRights::WHITE_KING_SIDE | CastlingRights::WHITE_QUEEN_SIDE | 
					 CastlingRights::BLACK_KING_SIDE | CastlingRights::BLACK_QUEEN_SIDE;

	memset(pinMap, NO_PIN, 64);
	UpdateBoards(0x00ULL, 0);

	irreversibleMove = 0;

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 6; j++) {
			uint64_t board = Boards[i][j]->board;
			int8_t k = pop_lsb(board);

			while (k != -1) {
				zobrist.make(i, j, k);
				cout << "INITIALIZING ZOBRIST HASH FOR " << (i == idx(Turn::WHITE) ? "WHITE " : "BLACK ") << " PIECE TYPE: " << j << " AT POSITION: " << unsigned(k) << endl;
				k = pop_lsb(board);
			}

		}
	}
}

// This function is used for updating the boards 
// from an toMask we can tell if an piece was captured 
// but cant tell which piece was captured so we need to loop through the opponent pieces and check if any of them were on that toMask 
// if so we can update the board and return the piece type and bit position for visual purposes

pair<uint8_t, uint8_t> ChessBitBoards::UpdateBoards(uint64_t toMask = 0x00ULL, bool player = 0)
{
	uint64_t a = 0x0ULL;
	uint8_t pieceType = -1;
	uint8_t bitPos = -1;

	for (int i = 0; i < 6; i++) {
		if (player == idx(Turn::WHITE)) {
			if (Boards[idx(Turn::BLACK)][i]->board & toMask) {
				pieceType = 6 + i;
				bitPos = log2(toMask);
				Boards[idx(Turn::BLACK)][i]->board ^= toMask;
				cout << "BLACK PIECE: " << pieceType << " CAPTURED!" << endl;
			}
		}
		a = a | Boards[idx(Turn::WHITE)][i]->board;
	}

	Boards[idx(Turn::WHITE)][6]->board = a;

	a = 0x0ULL;
	for (int i = 0; i < 6; i++) {
		if (player == idx(Turn::BLACK)) {
			if (Boards[idx(Turn::WHITE)][i]->board & toMask) {
				pieceType = i;
				bitPos = log2(toMask);
				Boards[idx(Turn::WHITE)][i]->board ^= toMask;
				cout << "WHITE PIECE: " << pieceType << " CAPTURED!" << endl;
			}
		}
		a = a | Boards[idx(Turn::BLACK)][i]->board;
	}

	Boards[idx(Turn::BLACK)][6]->board = a;

	return { pieceType, bitPos };
}

pair<int8_t, int8_t> ChessBitBoards::makeMove(uint64_t fromMask, uint64_t toMask, uint8_t pieceType, bool player)
{
	// Reset Check Boards
	Boards[idx(Turn::BLACK)][7]->board = 0x00ULL;
	Boards[idx(Turn::WHITE)][7]->board = 0x00ULL;

	// Handle Castling Rights
	CastlingHandling(fromMask, toMask, pieceType, player);

	// Reset Pin Maps
	memset(pinMap, NO_PIN, 64);

	// Update Piece Position
	Boards[player][pieceType]->board &= ~fromMask;
	Boards[player][pieceType]->board |= toMask;

	zobrist.make(player, pieceType, fromMask); // remove piece from old position in hash

	pair <int8_t, int8_t> visuals = UpdateBoards(toMask, player); // returns captured piecetype and bitPos

	if (visuals.first != -1) {
		zobrist.unmake(player, visuals.first, visuals.second);
		irreversibleMove = 0;

		// if captured piece was an rook update castling rights
		if (visuals.first == idx(ChessPiece::ROOK)) {
			uint8_t castlingType = visuals.first == 0 ? CastlingRights::WHITE_QUEEN_SIDE : visuals.first == 7 ? CastlingRights::WHITE_KING_SIDE :
				                   visuals.first == 56 ? CastlingRights::BLACK_QUEEN_SIDE : CastlingRights::BLACK_KING_SIDE;

			zobrist.castling(castlingRights, castlingRights & castlingType); 
			castlingRights &= ~castlingType;

			cout << "CASTLING RIGHTS LOST FOR " << (player == idx(Turn::WHITE) ? "WHITE " : "BLACK ") << ((castlingType | CastlingRights::WHITE_KING_SIDE || castlingType | CastlingRights::BLACK_KING_SIDE) ? "KING_SIDE" : "QUEEN SIDE") << endl;
		}

		memset(zobristHash, 0ULL, 1024); // reset zobrist hash history on capture 
	}

	zobristHash[irreversibleMove % 1024] = zobrist.getHash(); // store current hash for threefold repetition detection
	irreversibleMove++; // increment irreversible move counter for 50 move rule 
	UpdateGameState(player);

	if (pieceType == idx(ChessPiece::ROOK)) {
		uint8_t castlingType = fromMask == 0x0000000000000080ULL ? CastlingRights::WHITE_KING_SIDE : fromMask == 0x0000000000000001ULL ? CastlingRights::WHITE_QUEEN_SIDE :
			fromMask == 0x8000000000000000ULL ? CastlingRights::BLACK_KING_SIDE : CastlingRights::BLACK_QUEEN_SIDE;

		if ((castlingRights & castlingType) != 0) {
			zobrist.castling(castlingRights, castlingRights & castlingType);
			castlingRights &= ~castlingType;

			cout << "CASTLING RIGHTS LOST FOR " << (player == idx(Turn::WHITE) ? "WHITE " : "BLACK ") << ((castlingType | CastlingRights::WHITE_KING_SIDE || castlingType | CastlingRights::BLACK_KING_SIDE) ? "KING_SIDE" : "QUEEN SIDE") << endl;
		}
	}

	return visuals;
}

void ChessBitBoards::CastlingHandling(uint64_t fromMask, uint64_t toMask, uint8_t pieceType, bool player)
{
	if (pieceType == idx(ChessPiece::KING)) {
		// Update Castling Rights if King moves

		bool isCastlingMove = !(toMask & MoveGenerator::kingMoves(0x00ULL, log2(fromMask), 0x00ULL, true, castlingRights, player));

		cout << "IS CASTLING MOVE: " << isCastlingMove << endl;

		// Handle Castling Move
		if (isCastlingMove) {

			uint64_t fromRook, toRook;

			if (toMask > fromMask) { // king side castling
				fromRook = player == idx(Turn::WHITE) ? 0x0000000000000080ULL : 0x8000000000000000ULL;
				toRook = player == idx(Turn::WHITE) ? 0x0000000000000020ULL : 0x2000000000000000ULL;
			}
			else { // queen side castling
				fromRook = player == idx(Turn::WHITE) ? 0x0000000000000001ULL : 0x0100000000000000ULL;
				toRook = player == idx(Turn::WHITE) ? 0x0000000000000008ULL : 0x0800000000000000ULL;
			}

			uint8_t castlingType = player == idx(Turn::WHITE) ? CastlingRights::WHITE_KING_SIDE | CastlingRights::WHITE_QUEEN_SIDE : 
														CastlingRights::BLACK_KING_SIDE | CastlingRights::BLACK_QUEEN_SIDE;

			zobrist.unmake(player, idx(ChessPiece::ROOK), log2(fromRook));
			zobrist.make(player, idx(ChessPiece::ROOK), log2(toRook));
			zobrist.castling(castlingRights,  castlingRights & castlingType); // update castling rights in hash

			Boards[idx(Turn::WHITE)][idx(ChessPiece::ROOK)]->board &= ~fromRook; // remove rook from a1
			Boards[idx(Turn::WHITE)][idx(ChessPiece::ROOK)]->board |= toRook; // place rook on d1

			castlingRights &= ~castlingType; // remove castling rights after castling move
		}
	}
}

void ChessBitBoards::UpdateGameState(bool player)
{
	player = !player; // check the opponent's king state
	cout << "UPDATING GAME STATE FOR BOTH SIDES: " << endl;
	// ensure the player whose turn is now is executed first otherwise the gamestate could be wrong
	uint64_t PlayerKingPos = log2(Boards[player][idx(ChessPiece::KING)]->board);
	
	computeAttackingMask(PlayerKingPos, player, true, false);

	//convert to bit position
	PlayerKingPos = log2(Boards[!player][idx(ChessPiece::KING)]->board);

	cout << "\nUPDATING GAME STATE FOR OPPONENT: " << endl;

	/*printf("IS %s KING IN CHECK: %s\n",
		(player == idx(Turn::BLACK)) ? "BLACK" : "WHITE",
		(Boards[player][7]->board != 0x00ULL) ? "true" : "false");*/

	/*cout << "CHECK BOARD FOR OPPONENT: " << endl;
	MoveGenerator::printBitBoard_MG(Boards[player][7]->board);*/

	if (Boards[player][7]->board != 0x00ULL)
		computeAttackingMask(PlayerKingPos, !player, true, true);

	/*printf("IS %s KING IN CHECK: %s\n",
		(player == idx(Turn::BLACK)) ? "BLACK" : "WHITE",
		(Boards[player][7]->board != 0x00ULL) ? "true" : "false");*/

	/*cout << "FINAL CHECKING MASK: " << endl;
	MoveGenerator::printBitBoard_MG(Boards[player][7]->board);*/

	Terminal();

	cout << "CURRENT ZOBRIST HASH: " << zobrist.getHash() << endl;
}

bool ChessBitBoards::Terminal()
{
	if (Check3FoldRepetition(zobrist.getHash())) {
		cout << "DRAW BY THREEFOLD REPETITION!!!!!!!!!!" << endl;
		return true;
	}
	/*if (irreversibleMove >= 50) {
		cout << "DRAW BY 50 MOVE RULE!!!!!!!!!!" << endl;
		return true;
	}*/

	if (Boards[idx(Turn::WHITE)][7]->board == CHECKMATE) {
		cout << "WHITE WINS BY CHECKMATE!!!!!!!!!!" << endl;
		return true;
	}
	else if (Boards[idx(Turn::BLACK)][7]->board == CHECKMATE) {
		cout << "BLACK WINS BY CHECKMATE!!!!!!!!!!" << endl;
		return true;
	}

	if (Boards[idx(Turn::WHITE)][7]->board == STATEMATE || Boards[idx(Turn::BLACK)][7]->board == STATEMATE) {
		cout << "DRAW BY STALEMATE!!!!!!!!!!" << endl;
		return true;
	}

	return false;
}

uint64_t ChessBitBoards::LegalMoves(uint64_t enemyKingPos, uint64_t bitPos, uint8_t pieceType, bool player, bool gameState)
{
	uint64_t board = Boards[player][6]->board | Boards[!player][6]->board;
	uint64_t moves = 0x00ULL;

	if (Boards[player][7]->board == DOUBLE_CHECK) {
		cout << "DOUBLE CHECK DETECTED ONLY KING MOVES ALLOWED!!!!!!!!!!(LEGAL MOVES)" << endl;
	}

	if (gameState) {
		cout << "Calculating Legal Moves in GameState (Check Detection) " << endl;
		cout << "EnemyKingPos: " << enemyKingPos << " BitPos: " << bitPos << " PieceType: " << pieceType << " Player: " << player << endl;
	}
	else {
		cout << "Calculating Legal Moves " << endl;
		cout << "EnemyKingPos: " << enemyKingPos << " BitPos: " << bitPos << " PieceType: " << pieceType << " Player: " << player << endl;
	}

	switch (pieceType) {
	case(idx(ChessPiece::ROOK)): {

		cout << "BOARD STATE FOR ROOK MOVES: " << endl;
		MoveGenerator::printBitBoard_MG(Boards[player][7]->board);

		cout << "CHECK BOARD STATE FOR ROOK MOVES: " << endl;
		MoveGenerator::printBitBoard_MG(Boards[!player][7]->board);

		moves = !gameState ?
			MoveGenerator::rookMoves(board, bitPos, Boards[player][6]->board, Boards[player][7]->board, pinMap) :
			MoveGenerator::rookCheck(board, enemyKingPos, bitPos, Boards[player][6]->board, pinMap);
		break;
	}
	case(idx(ChessPiece::BISHOP)): {
		moves = !gameState ?
			MoveGenerator::bishopMoves(board, bitPos, Boards[player][6]->board, Boards[player][7]->board, pinMap) :
			MoveGenerator::bishopCheck(board, enemyKingPos, bitPos, Boards[player][6]->board, pinMap);
		break;
	}
	case(idx(ChessPiece::KNIGHT)): {
		if (Boards[player][7]->board == DOUBLE_CHECK) cout << "DOUBLE CHECK DETECTED ONLY KING MOVES ALLOWED!!!!!!!!!!(LEGAL MOVES KNIGHT)" << endl;
		else cout << "SCREW U" << endl;
		moves = !gameState ?
			MoveGenerator::knightMoves(bitPos, Boards[player][6]->board, Boards[player][7]->board, pinMap) :
			MoveGenerator::knightCheck(board, enemyKingPos, bitPos, Boards[player][6]->board, pinMap);
		break;
	}
	case(idx(ChessPiece::QUEEN)): {
		moves = !gameState ?
			MoveGenerator::queenMoves(board, bitPos, Boards[player][6]->board, Boards[player][7]->board, pinMap) :
			MoveGenerator::queenCheck(board, enemyKingPos, bitPos, Boards[player][6]->board, pinMap);
		break;
	}
	case(idx(ChessPiece::KING)): {
		uint64_t attackingMask = computeAttackingMask(bitPos, player, gameState, false);
		moves = MoveGenerator::kingMoves(attackingMask, bitPos, Boards[player][6]->board, false, castlingRights, player);
		break;
	}
	}

	return moves;
}

uint64_t ChessBitBoards::computeAttackingMask(uint64_t bitPos, bool player, bool gameState, bool detCanMove)
{
	// provided your in check its unlikely the oppenent is also in check so u need to answer can u defend that check? if not GGs figure it out lil bro
	uint64_t mask = 0x00ULL;
	int count = 0;
	bool canMove = false;

	for (int j = 0; j < 6; j++) {
		//if (!detCanMove && Boards[!player][j]->board == 0x00ULL) continue;
		for (int k = 0; k < 64; k++) {

			// should only work for the !player after player can updated checks and gamestate for !player namely the checkBoard add another boolean in argument for this x 
			if (detCanMove && !canMove && (Boards[!player][j]->board >> k) & 1ULL) {  // determine if any piece of the player can move
				printf("COMPUTING IF PIECETYPE: %d AT POSITION: %d CAN MOVE TO DEFEND %s KING!\n",
					j, k, !player == idx(Turn::BLACK) ? "BLACK" : "WHITE");
				if (uint64_t h = LegalMoves(bitPos, k, j, !player, false) != 0x00ULL) {
					canMove = true;
					printf("PIECETYPE: %d AT POSITION: %d CAN MOVE TO DEFEND %s KING!\n",
						j, k, !player == idx(Turn::BLACK) ? "BLACK" : "WHITE");
					MoveGenerator::printBitBoard_MG(h);
				}
			}

			if (!detCanMove && (Boards[!player][j]->board >> k) & 1ULL) {
				if (j == idx(ChessPiece::KING)) {
					if (!gameState)
						mask |= MoveGenerator::kingMoves(0x00ULL, k, 0x00ULL, true, castlingRights, player);
					continue;
				}

				uint64_t m = LegalMoves(bitPos, k, j, !player, gameState);

				cout << "LEGAL MOVES COMPUTED FOR PIECE " << j << endl;
				MoveGenerator::printBitBoard_MG(m);

				cout << "CHECKBOARD FOR KING" << endl;
				MoveGenerator::printBitBoard_MG(Boards[!player][7]->board);

				mask |= m;
				cout << "ATTACKING MASK AFTER ADDING NEW ATTACKER" << endl;
				MoveGenerator::printBitBoard_MG(mask);
				if (gameState && m != 0x00ULL) {
					count++;
					cout << "KING CHECK ATTACK DETECTED FROM PIECE TYPE: " << j << " AT POSITION: " << k << endl;
				}
			}
		}
	}

	if (gameState && detCanMove) {
		cout << "DEBUG: gameState=" << (int)gameState << " detCanMove=" << (int)detCanMove << " player=" << int(player) << std::endl;
		cout << "HERE IDIOT" << endl;
	}

	if (gameState) {
		cout << "FINAL KING CHECK ATTACK MASK: " << endl;
		MoveGenerator::printBitBoard_MG(mask);
	}

	if (gameState) {
		
		Boards[player][7]->board = mask;
		if (mask != 0x00ULL) {
			printf("%s KING IN CHECK!!!!!!!!!!\n", player == idx(Turn::WHITE) ? "WHITE" : "BLACK");
			cout << "KING IN CHECK ATTACKING MASK: " << endl;
			MoveGenerator::printBitBoard_MG(Boards[player][7]->board);
		}

		if (count >= 2) {
			// double check detected 
			printf("DOUBLE CHECK DETECTED CAN ONLY MOVE %s KING\n", player == idx(Turn::WHITE) ? "WHITE" : "BLACK");
			Boards[player][7]->board = DOUBLE_CHECK; 
		}

		if (detCanMove) {
			cout << idx(Turn::BLACK) << " " << idx(Turn::WHITE) << endl;
			printf("PLAYER = %s\n", player == idx(Turn::BLACK) ? "Black " : "White ");
			if ((Boards[!player][7]->board != 0x00ULL && !canMove)) {
				printf("%s KING IS CHECKMATED AS NO MOVES ARE AVAILABLE\n", !player == idx(Turn::BLACK) ? "Black " : "White ");
				Boards[!player][7]->board = CHECKMATE; 
			}
			else if (Boards[!player][7]->board == 0x00ULL && !canMove) {
				printf("%s KING IS STALEMATED AS NO MOVES ARE AVAILABLE\n", !player == idx(Turn::BLACK) ? "Black " : "White ");
				Boards[!player][7]->board = STATEMATE; 
			}
		}

		if (!canMove) printf("%s KING CANT MOVE\n", !player == idx(Turn::BLACK) ? "BLACK" : "WHITE");
	}
	
	return mask;
}

bool ChessBitBoards::Check3FoldRepetition(uint64_t curHash)
{
	int count = 0;

	for (int i = 0; i < irreversibleMove; i++) {
		if (zobristHash[i] == curHash) 
			count++;
		
		cout << "ZOBRIST HASH IN HISTORY: " << zobristHash[i] << " CURRENT HASH: " << curHash << " COUNT: " << count << endl;
		if (count >= 3) {
			cout << "THREEFOLD REPETITION DETECTED DRAW OFFERED" << endl;
			cout << "Irreversible Moves: " << unsigned(irreversibleMove) << endl;
			return true;
		}
	}

	return false;
}

// Create an full attack mask for the player
// Answer if the piece is an enemy piece (an sliding one at that) what r the occupancies
// does the king lie in the entire attack mask
// if so how many occupancies in that line of attack
// if two meaning the king and some other that piece is pinned
