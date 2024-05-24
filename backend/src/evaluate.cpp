#include "../include/evaluate.h"

using namespace PieceValues;

int evaluate(const PlayerBitboard& allies, const PlayerBitboard& opponents) {
	int pieceValue = pawnValue * (countBits(allies.pawns) - countBits(opponents.pawns))
		+ knightValue * (countBits(allies.knights) - countBits(opponents.knights))
		+ bishopValue * (countBits(allies.bishops) - countBits(opponents.bishops))
		+ rookValue * (countBits(allies.rooks) - countBits(opponents.rooks))
		+ queenValue * (countBits(allies.queens) - countBits(opponents.queens))
		+ kingValue * (countBits(allies.king) - countBits(opponents.king));


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

	return (white->isWhite ? 1 : -1) * (positionalValue) + pieceValue;
}
