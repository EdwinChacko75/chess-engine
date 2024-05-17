#include "../include/move_pick.h"
#include <iostream>
#include<string>
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
		cout<< "error : no piece on source square" << endl;
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

Move searchBestMove(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, int& counter, int& pruned) {
	Move bestMove;
	int bestScore = INT_MIN;

	int moveIndex = 0;
	Move* moves = generateMoves(allies, opponents, gameState, moveIndex);


	for (int i = 0; i < moveIndex; i++) {
		makeMove(allies, opponents, moves[i]);

		GameState copyGameState = gameState;
		updateGameState(copyGameState, moves[i]);

		//PositionHistory positionHistory;
		//positionHistory.push_back(positionAsString(allies, opponents));

		int eval = -negamax(opponents, allies, copyGameState, true, INT_MIN, INT_MAX, 1, counter, pruned);
		cout << " Eval: " << eval << endl;
		printChessBoard(allies, opponents);
		unMakeMove(allies, opponents, moves[i]);

		if (eval > bestScore) {
			bestScore = eval;
			bestMove = moves[i];
		}
	}
	cout<< "best score: " << bestScore << endl;
	makeMove(allies, opponents, bestMove);
	printChessBoard(allies, opponents);
	unMakeMove(allies, opponents, bestMove);
	delete[] moves;

	return bestMove;
}
int negamax(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, bool ourMove, int alpha, int beta, int ply, int& counter, int& pruned) {
	int depth = (gameState & depthMask) >> 5;
	int moveIndex = 0;
	if (depth == 0) {
		//int evaluation = evaluate(allies, opponents);
		// cout << "leaf eval: " << evaluation << endl;
		// printChessBoard(allies, opponents);
		return evaluate(allies, opponents);
	}

	Move* moves = generateMoves(allies, opponents, gameState, moveIndex);

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

		int eval = -negamax(opponents, allies, copyGameState, !ourMove, -beta, -alpha, ply + 1, ++counter, pruned);
		unMakeMove(allies, opponents, moves[i]);

		maxEval = std::max(maxEval, eval);
		alpha = std::max(alpha, maxEval);

		if (alpha >= beta) {
			pruned++;
			break;
		}
	}

	delete[] moves;
	return maxEval;
}

int quiesce(int alpha, int beta, int color, int ply) {
	return 0;
}




/*
if (beforeAllies != afterAllies || beforeOpponents != afterOpponents) {
	cout << "before" << endl; printBoard(beforeAllies); cout << "after" << endl; printBoard(afterAllies);
	printChessBoard(allies, opponents);
	printBoards(allies, opponents);
	makeMove(allies, opponents, moves[i]);
	allies.updatePlayerBitboard(opponents);
	cout << "made move" << endl;

	printChessBoard(allies, opponents);
	unMakeMove(allies, opponents, moves[i]);
	allies.updatePlayerBitboard(opponents);
	cout << "unmade move" << endl;
	printChessBoard(allies, opponents);

	exit(1);
}
*/