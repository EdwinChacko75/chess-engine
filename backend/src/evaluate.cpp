#include "../include/evaluate.h"

int evaluate(const PlayerBitboard& white, const PlayerBitboard& black, int& whoIsMoving) {

	whoIsMoving = whoIsMoving == 1 ? -1 : 1;

	
	return whoIsMoving * (countBits(white.pawns) - countBits(black.pawns) 
		+ 3 * countBits(white.knights) - 3 * countBits(black.knights) 
		+ 3 * countBits(white.bishops) - 3 * countBits(black.bishops) 
		+ 5 * countBits(white.rooks) - 5 * countBits(black.rooks) 
		+ 9 * countBits(white.queens) - 9 * countBits(black.queens)
		+ 200 * countBits(white.king) - 200 * countBits(black.king)
		);
}
