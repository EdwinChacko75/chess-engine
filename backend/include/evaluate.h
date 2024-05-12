#ifndef EVALUATE_H
#define EVALUATE_H

#include "./bitboard.h"

int evaluate(const PlayerBitboard& white, const PlayerBitboard& black, int& whoIsMoving);

#endif // !EVALUATE_H
