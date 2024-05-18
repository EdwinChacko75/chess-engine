#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "../include/bitboard.h"
#include "../include/gamestate.h"

#include <cstdint>
#include <array>

using ZobristHash = uint64_t;
using Move = uint32_t;

constexpr int PIECE_NB = 12;
constexpr int SQUARE_NB = 64;
constexpr int FILE_NB = 8;
constexpr int CASTLING_RIGHT_NB = 16;

ZobristHash generateZorbistHash(const PlayerBitboard& allies, const PlayerBitboard& opponents, const GameState& gameState);

struct TTEntry {
	ZobristHash hash;
	Move bestMove;
	size_t generation;
	int depth;
	int score;
	int bound;
};

constexpr int BOUND_EXACT = 0;
constexpr int BOUND_LOWERBOUND = 1;
constexpr int BOUND_UPPERBOUND = 2;

void startNewSearch();
void initializeTable(size_t sizeInMB);
void storeEntry(ZobristHash key, int depth, int score, int bound, Move move);
bool probeEntry(ZobristHash key, TTEntry*& entry, const int depth);



#endif 
