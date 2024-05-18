#include "../include/evaluate.h"

int evaluate(const PlayerBitboard& allies, const PlayerBitboard& opponents) {
	int pieceValue = 100 * (countBits(allies.pawns) - countBits(opponents.pawns))
		+ 300 * (countBits(allies.knights) - countBits(opponents.knights))
		+ 300 * (countBits(allies.bishops) - countBits(opponents.bishops))
		+ 500 * (countBits(allies.rooks) - countBits(opponents.rooks))
		+ 900 * (countBits(allies.queens) - countBits(opponents.queens))
		+ 10000 * (countBits(allies.king) - countBits(opponents.king));

	const PlayerBitboard* white;
	const PlayerBitboard* black;
	if (allies.isWhite) {
		white = &allies;
		black = &opponents;
	}
	else {
		white = &opponents;
		black = &allies;
	}
	
	int positionalValue = 0;
	for (int i = 0; i < 64; i++) {
		if (white->pawns & (1ULL << i)) {
			positionalValue += PAWN_SQUARE_TABLE_WHITE[i];
		}
		else if (black->pawns & (1ULL << i)) {
			positionalValue -= PAWN_SQUARE_TABLE_BLACK[i];
		}
		else if (white->knights & (1ULL << i)) {
			positionalValue += KNIGHT_SQUARE_TABLE_WHITE[i];
		}
		else if (black->knights & (1ULL << i)) {
			positionalValue -= KNIGHT_SQUARE_TABLE_BLACK[i];
		}
		else if (white->bishops & (1ULL << i)) {
			positionalValue += BISHOP_SQUARE_TABLE_WHITE[i];
		}
		else if (black->bishops & (1ULL << i)) {
			positionalValue -= BISHOP_SQUARE_TABLE_BLACK[i];
		}
		else if (white->rooks & (1ULL << i)) {
			positionalValue += ROOK_SQUARE_TABLE_WHITE[i];
		}
		else if (black->rooks & (1ULL << i)) {
			positionalValue -= ROOK_SQUARE_TABLE_BLACK[i];
		}
		else if (white->queens & (1ULL << i)) {
			positionalValue += QUEEN_SQUARE_TABLE_WHITE[i];
		}
		else if (black->queens & (1ULL << i)) {
			positionalValue -= QUEEN_SQUARE_TABLE_BLACK[i];
		}
		else if (white->king & (1ULL << i)) {
			if (countBits(white->pawns) + countBits(black->pawns) > 10) {
				positionalValue += KING_SQUARE_TABLE_WHITE_OP[i];
			}
			else {
				positionalValue += KING_SQUARE_TABLE_ENDGAME[i];
			}
		}
		else if (black->king & (1ULL << i)) {
			if (countBits(white->pawns) + countBits(black->pawns) > 10) {
				positionalValue -= KING_SQUARE_TABLE_BLACK_OP[i];
			}
			else {
				positionalValue -= KING_SQUARE_TABLE_ENDGAME[i];
			}
		}
	}
	//std::cout << allies.isWhite << " " << perspectiveCompensation << " " << positionalValue<<'\n';
	return (white->isWhite ? 1 : -1) * (positionalValue) + pieceValue;
}
