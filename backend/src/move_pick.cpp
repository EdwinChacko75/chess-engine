#include "../include/move_pick.h"
#include <iostream>
#include <string>
#include <algorithm>


using std::cout;
using std::endl;


void printHistory(const PositionHistory& positionHistory) {
	for (int i = 0; i < positionHistory.size(); i++) {
		cout << "\nPosition " << (i + 1) << endl;
		cout << positionHistory[i] << endl;
	}
}

static std::string positionAsString(const PlayerBitboard& allies, const PlayerBitboard& opponents) {
	std::string position = "";
	Bitboard allPieces = allies.allPieces | opponents.allPieces;
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			int sq = row * 8 + col;
			if (isSet(allies.pawns, sq)) {
				position += "P";
			}
			else if (isSet(allies.knights, sq)) {
				position += "N";
			}
			else if (isSet(allies.bishops, sq)) {
				position += "B";
			}
			else if (isSet(allies.rooks, sq)) {
				position += "R";
			}
			else if (isSet(allies.queens, sq)) {
				position += "Q";
			}
			else if (isSet(allies.king, sq)) {
				position += "K";
			}
			else if (isSet(opponents.pawns, sq)) {
				position += "p";
			}
			else if (isSet(opponents.knights, sq)) {
				position += "n";
			}
			else if (isSet(opponents.bishops, sq)) {
				position += "b";
			}
			else if (isSet(opponents.rooks, sq)) {
				position += "r";
			}
			else if (isSet(opponents.queens, sq)) {
				position += "q";
			}
			else if (isSet(opponents.king, sq)) {
				position += "k";
			}
			else {
				position += ".";
			}
		}
		position += "\n";
	}
	reverse(position.begin(), position.end());

	return position;
}

void makeMove(PlayerBitboard& allies, PlayerBitboard& opponents, Move move) {
	int source = move & sourceMask;

	if ((1ULL << source) & allies.friendlyPieces) {
		makeTestMove(allies, opponents, move);
	}
	else if ((1ULL << source) & opponents.friendlyPieces)
	{
		makeTestMove(opponents, allies, move);
	}
	else {
		cout << (move & sourceMask) << ((move & destinationMask) >> 6) << endl;
		makeTestMove(opponents, allies, move);

		throw std::runtime_error("Error: No piece on source square");
	}

}
void unMakeMove(PlayerBitboard& allies, PlayerBitboard& opponents, Move move) {
	int destination = (move & destinationMask) >> 6;
	
	if ((1ULL << destination) & allies.friendlyPieces) {
		unMakeTestMove(allies, opponents, move);
	}
	else if ((1ULL << destination) & opponents.friendlyPieces) 
	{
		unMakeTestMove(opponents, allies, move);
	}
	else {
		cout << "error : no piece on destination square" << endl;
		unMakeTestMove(opponents, allies, move);
	}

}
void orderMoves(Move* moves, const int moveIndex) {
	std::sort(moves, moves + moveIndex, [](Move a, Move b) {
		bool aIsCapture = a & captureMask;
		bool bIsCapture = b & captureMask;

		if (aIsCapture && bIsCapture) {
			return MVV_LVA_TABLE[(a & pieceTypeMask) >> 19][(a & capturedPieceMask) >> 23] >
				MVV_LVA_TABLE[(b & pieceTypeMask) >> 19][(b & capturedPieceMask) >> 23];
		}
		else if (aIsCapture && !bIsCapture) {
			return true;
		}
		else if (!aIsCapture && bIsCapture) {
			return false;
		}
		else {
			return false;
		}
	});
}

Move searchBestMove(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, int& counter, int& pruned, int& cached) {
	startNewSearch();
	ZobristHash zobristHash = generateZorbistHash(allies, opponents, gameState);
	int depth = (gameState & depthMask) >> 5;
	
	TTEntry* entry = nullptr;
	if (probeEntry(zobristHash, entry, depth)  && entry->bound == BOUND_EXACT) {
		cached++;
		return entry->bestMove;
	}

	Move bestMove;
	int bestScore = INT_MIN;

	int moveIndex = 0;
	Move* moves = generateMoves(allies, opponents, gameState, moveIndex);
	orderMoves(moves, moveIndex);
	cout << "zorbist hash: " << zobristHash << endl;

	for (int i = 0; i < moveIndex; i++) {
		GameState copyGameState = gameState;
		makeMove(allies, opponents, moves[i]);
		updateGameState(copyGameState, moves[i]);
		
		if (isKingInCheck(allies, opponents, copyGameState) && ((moves[i] & capturedPieceMask) >> 23) != 6) {
			unMakeMove(allies, opponents, moves[i]);
			continue;
		}

		int eval = -negamax(opponents, allies, copyGameState, true, INT_MIN, INT_MAX, 1, counter, pruned, cached);
		
		unMakeMove(allies, opponents, moves[i]);

		if (eval > bestScore) {
			bestScore = eval;
			bestMove = moves[i];
		}
	}
	
	
	storeEntry(zobristHash, depth, bestScore,  BOUND_EXACT, bestMove);

	delete[] moves;
	return bestMove;
}



int negamax(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, bool ourMove, int alpha, int beta, int ply, int& counter, int& pruned, int& cached) {
	ZobristHash zobristHash = generateZorbistHash(allies, opponents, gameState);
	int alphaOriginal = alpha;
	int depth = (gameState & depthMask) >> 5;	
	
	TTEntry* entry = nullptr;
	if (  probeEntry(zobristHash, entry, (depth - 1) )  ) {
		++cached;
		switch (entry->bound) {
			case BOUND_EXACT: return entry->score;
			case BOUND_LOWERBOUND:	alpha = std::max(alpha, entry->score); break;
			case BOUND_UPPERBOUND: beta = std::min(beta, entry->score); break;
			default: throw std::runtime_error("check TT handling"); exit(69); break;
		}
		if (alpha >= beta) return entry->score;
	}
	
	int moveIndex = 0;

	if (depth == 0) return evaluate(allies, opponents);
	 
	Move bestMove_TT = 0;
	Move* moves = generateMoves(allies, opponents, gameState, moveIndex);

	orderMoves(moves, moveIndex);
	if (moveIndex == 0 && isKingInCheck(allies, opponents, gameState)) {
		delete[] moves;

		return INT_MIN;
	}
	if (moveIndex == 0) {
		delete[] moves;
		return 0;
	}

	int maxEval = INT_MIN;

	for (int i = 0; i < moveIndex; i++) {
		GameState copyGameState = gameState;
		makeMove(allies, opponents, moves[i]);
		updateGameState(copyGameState, moves[i]);

		if (isKingInCheck(allies, opponents, copyGameState) && ((moves[i] & capturedPieceMask) >> 23) != 6) {
			unMakeMove(allies, opponents, moves[i]);
			continue;
		}

		int eval = -negamax(opponents, allies, copyGameState, !ourMove, -beta, -alpha, ply + 1, ++counter, pruned, cached);
		unMakeMove(allies, opponents, moves[i]);

		if (eval > maxEval) {
			maxEval = eval;
			bestMove_TT = moves[i];
		}
		alpha = std::max(alpha, maxEval);

		if (alpha >= beta) {
			pruned++;
			bestMove_TT = moves[i];
			break;
		}
	}

	storeEntry(zobristHash, depth, maxEval, (alpha >= beta ? BOUND_LOWERBOUND : (alphaOriginal != alpha ? BOUND_EXACT : BOUND_UPPERBOUND)), bestMove_TT);

	delete[] moves;
	return maxEval;
}

int quiesce(int alpha, int beta, int color, int ply) {
	return 0;
}
