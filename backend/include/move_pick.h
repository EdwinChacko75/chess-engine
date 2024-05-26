#ifndef MOVE_PICK_H
#define MOVE_PICK_H

#include "./transposition_table.h"
#include "./move_generation.h"
#include "./evaluate.h"
#include <algorithm>
#include <array>
using PositionHistory = std::vector<std::string>;

constexpr int MaxDepth = 32;
constexpr int NumKillerMoves = 2;


extern std::array<std::array<Move, NumKillerMoves>, MaxDepth> killerMoves;

//inline std::array<std::array<Move, NumKillerMoves>, MaxDepth> killerMoves = { 0 };

constexpr int MVV_LVA_TABLE[6][5] = {
    {
        (PieceValues::pawnValue * 10 - PieceValues::pawnValue),
        (PieceValues::knightValue * 10 - PieceValues::pawnValue),
        (PieceValues::bishopValue * 10 - PieceValues::pawnValue),
        (PieceValues::rookValue * 10 - PieceValues::pawnValue),
        (PieceValues::queenValue * 10 - PieceValues::pawnValue),
    },
    {
		(PieceValues::pawnValue * 10 - PieceValues::knightValue),
		(PieceValues::knightValue * 10 - PieceValues::knightValue),
		(PieceValues::bishopValue * 10 - PieceValues::knightValue),
		(PieceValues::rookValue * 10 - PieceValues::knightValue),
		(PieceValues::queenValue * 10 - PieceValues::knightValue),
	},
    {
		(PieceValues::pawnValue * 10 - PieceValues::bishopValue),
		(PieceValues::knightValue * 10 - PieceValues::bishopValue),
		(PieceValues::bishopValue * 10 - PieceValues::bishopValue),
		(PieceValues::rookValue * 10 - PieceValues::bishopValue),
		(PieceValues::queenValue * 10 - PieceValues::bishopValue),
	},
	{
		(PieceValues::pawnValue * 10 - PieceValues::rookValue),
		(PieceValues::knightValue * 10 - PieceValues::rookValue),
		(PieceValues::bishopValue * 10 - PieceValues::rookValue),
		(PieceValues::rookValue * 10 - PieceValues::rookValue),
		(PieceValues::queenValue * 10 - PieceValues::rookValue),
	},
	{
		(PieceValues::pawnValue * 10 - PieceValues::queenValue),
		(PieceValues::knightValue * 10 - PieceValues::queenValue),
		(PieceValues::bishopValue * 10 - PieceValues::queenValue),
		(PieceValues::rookValue * 10 - PieceValues::queenValue),
		(PieceValues::queenValue * 10 - PieceValues::queenValue),
	},
	{
		(PieceValues::pawnValue * 10 - PieceValues::kingValue),
		(PieceValues::knightValue * 10 - PieceValues::kingValue),
		(PieceValues::bishopValue * 10 - PieceValues::kingValue),
		(PieceValues::rookValue * 10 - PieceValues::kingValue),
		(PieceValues::queenValue * 10 - PieceValues::kingValue),
	}
};

void orderMoves(Move* moves, const int moveIndex, int depth);

void makeMove(PlayerBitboard& allies, PlayerBitboard& opponents, Move move);
void unMakeMove(PlayerBitboard& allies, PlayerBitboard& opponents, Move move);

Move searchBestMove(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, int& counter, int& pruned, int& cached);
int negamax(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, bool ourMove, int alpha, int beta, int ply, int& counter, int& pruned, int& cached);

int quiesce(int alpha, int beta, int color, int ply);


#endif

