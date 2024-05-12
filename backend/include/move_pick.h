#ifndef MOVE_PICK_H
#define MOVE_PICK_H

#include "./move_generation.h"
#include "./evaluate.h"
Move movePick(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, bool ourMove, int alpha, int beta, int ply);

int negamax(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, bool ourMove, int alpha, int beta, int ply, int& counter);

int quiesce(int alpha, int beta, int color, int ply);


#endif

