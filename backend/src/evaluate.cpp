#include "../include/evaluate.h"

using namespace PieceValues;

int evaluate(const PlayerBitboard& allies, const PlayerBitboard& opponents) {
	int alliedPawns = countBits(allies.pawns);
	int opposingPawns = countBits(opponents.pawns);
	bool isEndgame = (alliedPawns + opposingPawns) < 8;
	
	int pieceValue = pawnValue * (alliedPawns - opposingPawns)
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
	Bitboard allPieces = white->allPieces;
	while (allPieces) {
		int i = lsb(allPieces);
		Bitboard current = (1ULL << i);
		
		if (white->pawns & current) {
			positionalValue += PAWN_SQUARE_TABLE_WHITE[i];
		}
		else if (black->pawns & current) {
			positionalValue -= PAWN_SQUARE_TABLE_BLACK[i];
		}
		else if (white->knights & current) {
			positionalValue += KNIGHT_SQUARE_TABLE_WHITE[i];
		}
		else if (black->knights & current) {
			positionalValue -= KNIGHT_SQUARE_TABLE_BLACK[i];
		}
		else if (white->bishops & current) {
			positionalValue += BISHOP_SQUARE_TABLE_WHITE[i];
		}
		else if (black->bishops & current) {
			positionalValue -= BISHOP_SQUARE_TABLE_BLACK[i];
		}
		else if (white->rooks & current) {
			positionalValue += ROOK_SQUARE_TABLE_WHITE[i];
		}
		else if (black->rooks & current) {
			positionalValue -= ROOK_SQUARE_TABLE_BLACK[i];
		}
		else if (white->queens & current) {
			positionalValue += QUEEN_SQUARE_TABLE_WHITE[i];
		}
		else if (black->queens & current) {
			positionalValue -= QUEEN_SQUARE_TABLE_BLACK[i];
		}
		else if (white->king & current) {
			positionalValue += isEndgame ? KING_SQUARE_TABLE_ENDGAME[i] : KING_SQUARE_TABLE_WHITE_OP[i];
		}
		else if (black->king & current) {
			positionalValue -= isEndgame ? KING_SQUARE_TABLE_ENDGAME[i] : KING_SQUARE_TABLE_BLACK_OP[i];
		}
		allPieces &= allPieces - 1;
	
	}

	return (white->isWhite ? 1 : -1) * (positionalValue) + pieceValue;
}
