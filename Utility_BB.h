#include <cstdint>

uint64_t rookMask(uint64_t bit_pos, bool noedges = false) {
    int file = bit_pos / 8;
    int rank = bit_pos % 8;

	// vertical file
    uint64_t b = 0xFFULL << (file * 8);
	// horizontal rank
    uint64_t c = 0x0101010101010101ULL * (rank + 1);

    return (~b & c) | b;
}

uint64_t bishopMask(uint64_t sq) {

    int rank = sq / 8;
    int file = sq % 8;

    uint64_t diag = 0x8040201008040201ULL;
    int diag_shift = (file - rank) * 8;
    if (diag_shift >= 0) diag <<= diag_shift;
    else diag >>= -diag_shift;

    uint64_t antiDiag = 0x0102040810204080ULL;
    int anti_shift = (7 - file - rank) * 8;
    if (anti_shift >= 0) antiDiag <<= anti_shift;
    else antiDiag >>= -anti_shift;

    return (diag ^ antiDiag) & ~(1ULL << sq);
}



