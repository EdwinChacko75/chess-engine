#ifndef MOVE_PICK_H
#define MOVE_PICK_H

#include "./transposition_table.h"
#include "./move_generation.h"
#include "./evaluate.h"
#include <algorithm>
using PositionHistory = std::vector<std::string>;


void makeMove(PlayerBitboard& allies, PlayerBitboard& opponents, Move move);
void unMakeMove(PlayerBitboard& allies, PlayerBitboard& opponents, Move move);

Move searchBestMove(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, int& counter, int& pruned, int& cached);
int negamax(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, bool ourMove, int alpha, int beta, int ply, int& counter, int& pruned, int& cached);

int quiesce(int alpha, int beta, int color, int ply);


#endif

