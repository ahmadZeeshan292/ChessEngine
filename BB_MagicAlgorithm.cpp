#include "BB_MagicAlgorithm.h"
#include <cmath>
#include "PieceMagicStorage.h"
#include <iostream>
#include <vector>
#include <set>
using namespace std;

int count_ones(uint64_t bb)
{
    int count = 0;
    for (int i = 0; i < 64; i++) {
        if ((bb >> i) & 1ULL) {
            count++;
        }
    }
    return count;
}

int get_lsb_index(uint64_t bitboard) {
   
    for (int i = 0; i < 64; i++) {
		if ((bitboard >> i) & 1ULL) {
			return i;
		}
    }
    return -1;
}

uint64_t compute_occupany(int index, uint64_t attack_mask, int nbits)
{
    uint64_t occupany = 0x00;
    uint64_t mask_copy = attack_mask; // Work with a copy

    for (int count = 0; count < nbits; count++) {
        int lsb = get_lsb_index(mask_copy);
        if (lsb == -1) {
            cout << "LSB!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! BROKEN" << endl;
        }
        mask_copy &= (mask_copy - 1); // clear the lsb

        if (index & (1ULL << count))
            occupany |= (1ULL << lsb);
    }

    return occupany & attack_mask; // Mask the result
}


void forward(bool bishop, uint64_t bitPos)
{

    uint64_t attackMask = bishop ? bishopAttackMask(bitPos, false) : rookAttackMask(bitPos, false);

    int nbits = count_ones(attackMask);

    int permutations = 1 << nbits;

    if ((permutations > 512 && bishop) || (permutations > 4096 && !bishop)) {
		printBitBoard(attackMask);
        cout << "Stop bitpos: " << bitPos << endl;
    }

    int shift = 64 - nbits;
	!bishop ? RookShifts[bitPos] = shift : BishopShifts[bitPos] = shift;

	uint64_t magic = bishop ? BishopMagic[bitPos] : RookMagic[bitPos];

    for (int index = 0; index < permutations; index++) {
		uint64_t occupany = compute_occupany(index, attackMask, nbits);

        // index to place legal moves in the moves table
        int idx = (occupany * magic) >> shift;

        if ((bishop && idx >= 512) || (!bishop && idx >= 4096)) {
			cout << "Magic number is broken!" << endl;
        }

        // write function to compute the legal moves for an occupancy
		uint64_t legal_moves = bishop ? bishopLegalMoves(occupany, bitPos) : rookLegalMoves(occupany, bitPos);

		// Store legal moves in the magic moves table
        !bishop ? RookAttacks[bitPos][idx] = legal_moves : BishopAttacks[bitPos][idx] = legal_moves;
    }
}

void forwardMagic()
{
    const uint64_t notFileA = 0xFEFEFEFEFEFEFEFEULL; // Everything except File A
    const uint64_t notFileH = 0x7F7F7F7F7F7F7F7FULL; // Everything except File H

    for (int i = 0; i < 64; i++) {
        cout << "BitPosition " << i << endl;

        cout << "Bishop!!!!!!!!!!!!!!!!" << endl;
		forward(true, i);   // bishop

        cout << "Rook!!!!!!!!!!!!!!!!" << endl;
		forward(false, i);  // rook 

        PawnAttacks[0][i] = ((1ULL << i) << 7 & notFileH) | ((1ULL << i) << 9 & notFileA);
        PawnAttacks[1][i] = ((1ULL << i) >> 9 & notFileH) | ((1ULL << i) >> 7 & notFileA);

        kingMoves[i] = computeKingMask(i); // king
        knightMoves[i] = computeKnightMask(i); //knight
    }

    init_CheckLine_table();
}

void init_CheckLine_table()
{
    int count = 0;
    for (int s1 = 0; s1 < 64; s1++) {
        for (int s2 = 0; s2 < 64; s2++) {
            // need to filter out points which are not in path
            int from = s1 > s2 ? s2 : s1;
            int to = s1 > s2 ? s1 : s2;

			uint64_t m = 0x00ULL;
            // condition to check if points intersect
            if (rookAttackMask(s1, true) & (1ULL << s2)) {
                 m = (rookAttackMask(s1, true) & rookAttackMask(s2, true)); // common ray between points
            }
            else if (bishopAttackMask(s1, true) & (1ULL << s2)) {
                m = (bishopAttackMask(s1, true) & bishopAttackMask(s2, true));
            }
            else {
                CheckRays[s1][s2] = 0x00ULL;
                continue;
            }
			CheckRays[s1][s2] = 0x00ULL;

            for (int i = from; i < to; i++) { // loop to filter out points in ray before to and after from
                if (m & (1ULL << i)) {
                    CheckRays[s1][s2] |= (1ULL << i); // can be true for checkRays[s2][s1]
                }
            }
            count++;
        }
    }

	cout << "RAY COUNT: " << count << endl;
}

uint64_t computeCheckRay(int checkingPiecePos, int kingPos)
{
    return CheckRays[checkingPiecePos][kingPos];
}

uint64_t rookAttackMask(uint64_t bit_pos, bool edges)
{

    int file = bit_pos / 8;
    int rank = bit_pos % 8;
    // vertical file
    uint64_t b = edges ? 0xFFULL << (file * 8) : 0x7EULL << (file * 8);
    // horizontal rank
    uint64_t c = edges ? 0x0101010101010101ULL << rank : 0x0001010101010100ULL << rank;
    return edges ? (c | b) : (c | b) & ~(1ULL << bit_pos);

}

uint64_t bishopAttackMask(uint64_t bitPos, bool edges)
{
	uint64_t mask = 0x00ULL;
    vector<pair<int64_t, int64_t>> offsets{ {1, 1}, {-1, -1}, {1, -1}, {-1, 1} };

    for (auto off : offsets) {
        int64_t i_ = (bitPos / 8) + (edges ? 0 : off.first);
        int64_t j_ = (bitPos % 8) + (edges ? 0 : off.second);

        for (int64_t i = i_, j = j_; ; i += off.first, j += off.second) {
            if (edges) {
                if (i < 0 || i >= 8 || j < 0 || j >= 8) break;
            }
            else {
                if (i <= 0 || i >= 7 || j <= 0 || j >= 7) break;
            }
            mask |= (1ULL << (i * 8 + j));
        }

    }
    return edges ? mask : mask & ~(1ULL << bitPos);
}

uint64_t bishopLegalMoves(uint64_t occupancy, uint64_t bitPos)
{
    uint64_t moves = 0ULL;
    int rank = bitPos / 8;
    int file = bitPos % 8;

    vector<pair<int, int>> offsets = { {1, 1}, {1, -1}, {-1, 1}, {-1, -1} };

    for (auto off : offsets) {
        for (int r = rank + off.first, f = file + off.second;
            r >= 0 && r < 8 && f >= 0 && f < 8;
            r += off.first, f += off.second)
        {
            int sq = r * 8 + f;
            moves |= (1ULL << sq);
            if (occupancy & (1ULL << sq)) break;
        }
    }
    return moves;
}

uint64_t rookLegalMoves(uint64_t occupancy, uint64_t bitPos)
{
    uint64_t moves = 0ULL;

    int rank = bitPos / 8;
    int file = bitPos % 8;

    // north
    for (int r = rank + 1; r < 8; r++) {
        int sq = r * 8 + file;
        moves |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break;
    }

    // south
    for (int r = rank - 1; r >= 0; r--) {
        int sq = r * 8 + file;
        moves |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break;
    }

    // east
    for (int f = file + 1; f < 8; f++) {
        int sq = rank * 8 + f;
        moves |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break;
        
    }

    // west
    for (int f = file - 1; f >= 0; f--) {
        int sq = rank * 8 + f;
        moves |= (1ULL << sq);
        if (occupancy & (1ULL << sq)) break;
        
    }

    return moves;
}

uint64_t computeRookMoves(uint64_t occupany, uint64_t bitPos)
{
	//occupany &= rookAttackMask(bitPos, false);
	int idx = (occupany * RookMagic[bitPos]) >> RookShifts[bitPos];
    return RookAttacks[bitPos][idx];
    //return rookLegalMoves(occupany, bitPos);
}

uint64_t computeBishopMoves(uint64_t occupany, uint64_t bitPos)
{
	int idx = (occupany * BishopMagic[bitPos]) >> BishopShifts[bitPos];
	return BishopAttacks[bitPos][idx];
    //return bishopLegalMoves(occupany, bitPos);
}

uint64_t computeKingMask(uint64_t bitPos)
{
	vector<pair<int64_t, int64_t>> offsets{ {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1} };
	uint64_t mask = 0x00ULL;

    for (auto off : offsets) {
        int i = bitPos / 8 + off.first;
		int j = bitPos % 8 + off.second;

        if ((i >= 0 && i < 8) && (j >= 0 && j < 8)) {
			mask |= (1ULL << (i * 8 + j));
        }
    }
    return mask;
}

uint64_t computeKnightMask(uint64_t bitPos)
{
    vector<pair<int64_t, int64_t>> offsets{ {2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {1, -2}, {-1, 2}, {-1, -2} };
    uint64_t mask = 0x00ULL;

    for (auto off : offsets) {
        int i = bitPos / 8 + off.first;
        int j = bitPos % 8 + off.second;

        if ((i >= 0 && i < 8) && (j >= 0 && j < 8)) {
            mask |= (1ULL << (i * 8 + j));
        }
    }
    return mask;
}

uint64_t computeKingMoves(uint64_t bitPos)
{
    return kingMoves[bitPos];
}

uint64_t computeKnightMoves(uint64_t bitPos)
{
    return knightMoves[bitPos];
}

uint64_t computePawnAttacks(uint64_t bitPos, bool player)
{
    return PawnAttacks[player][bitPos];
}

void find_mf_index() {
    set<uint64_t> uni_pos;
    int count = 0;
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 4096; j++) {
			if (RookAttacks[i][j] != 0ULL) {
                count++;
			}
		}
	}
	cout << "Rook Positions: " << count << endl;
}

void printBitBoard(uint64_t bb)
{
	for (int rank = 7; rank >= 0; rank--) {

		cout << rank + 1 << "    ";
		for (int file = 0; file < 8; file++) {
			int sq = rank * 8 + file;
			if (bb & (1ULL << sq)) {
				cout << "1 ";
			}
			else {
				cout << "0 ";
			}
		}
		cout << "\n";
	}

    cout << "     ";
	for (int i = 'a'; i <= 'h'; i++) {
		cout << (char)i << " ";
	}

    cout << endl << endl;
}


