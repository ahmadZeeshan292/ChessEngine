#pragma once

#include <cstdint>


int count_ones(uint64_t bb);
int get_lsb_index(uint64_t bb);
uint64_t compute_occupany(int index, uint64_t attack_mask, int nbits);

void forward(bool bishop, uint64_t bitPos);
void forwardMagic();

void init_CheckLine_table();
uint64_t computeCheckRay(int checkingPiecePos, int kingPos);

uint64_t rookAttackMask(uint64_t square, bool edges);
uint64_t bishopAttackMask(uint64_t square, bool edges);

uint64_t bishopLegalMoves(uint64_t occupany, uint64_t bitPos);
uint64_t rookLegalMoves(uint64_t occupany, uint64_t bitPos);

uint64_t computeRookMoves(uint64_t occupany, uint64_t bitPos);
uint64_t computeBishopMoves(uint64_t occupany, uint64_t bitPos);

uint64_t computeKingMask(uint64_t bitPos);
uint64_t computeKnightMask(uint64_t bitPos);

uint64_t computeKingMoves(uint64_t bitPos);
uint64_t computeKnightMoves(uint64_t bitPos);

uint64_t computePawnAttacks(uint64_t bitPos, bool player);
uint64_t computeBishopAttackMask(uint8_t bitPos);

void find_mf_index();

void printBitBoard(uint64_t bb);