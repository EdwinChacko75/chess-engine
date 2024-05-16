#include "../include/move_pick.h"
#include <iostream>

using std::cout;
using std::endl;

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

// move evaluating is weird
Move movePick(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, bool ourMove, int alpha, int beta, int ply, int& counter, int& pruned) {
	Move bestMove;
	int maxEval = INT_MIN;
	int moveIndex = 0;
	

	Move* moves = generateMoves(allies, opponents, gameState, moveIndex);
	printMoves(allies, opponents, moves, moveIndex);
	for (int i = 0; i < moveIndex; i++) {
		GameState copyGameState = gameState;

		makeMove(allies, opponents, moves[i]);

		updateGameState(copyGameState, moves[i]);

		int eval = negamax(opponents, allies, copyGameState, !ourMove, -beta, -alpha, ply + 1, ++counter, pruned);
		cout<< "eval: " << eval << endl;
		printChessBoard(allies, opponents);

		if (eval > maxEval) {
			maxEval = eval;
			bestMove = moves[i];
		}
		unMakeMove(allies, opponents, moves[i]);
	}
	delete[] moves;

	return bestMove;

}
int negamax(PlayerBitboard& allies, PlayerBitboard& opponents,const GameState& gameState, bool ourMove, int alpha, int beta, int ply, int& counter, int& pruned) {
	//printChessBoard(allies, opponents);
	int depth = (gameState & depthMask) >> 5;
	int whoIsMoving = ourMove ? 1 : -1;
	if (depth == 0) return whoIsMoving * evaluate(allies, opponents);

	bool turn = (gameState & turnMask) >> 4;
	int moveIndex = 0;
	const Bitboard& afterAllies = allies.allPieces;
	const Bitboard& afterOpponents = opponents.allPieces;
	
	
	Move* moves = generateMoves(allies, opponents, gameState, moveIndex);
	//printMoves(allies, opponents, moves, moveIndex);

	if (moveIndex == 0 && isKingInCheck(allies, opponents, gameState) ) return INT_MAX;
	if (moveIndex == 0) return 0;
	
	int maxEval = INT_MIN;

	for (int i = 0; i < moveIndex; i++) {
		GameState copyGameState = gameState;

		const Bitboard beforeAllies = allies.allPieces;
		const Bitboard beforeOpponents = opponents.allPieces;

		makeMove(allies, opponents, moves[i]);

		updateGameState(copyGameState, moves[i]);
		
		//printChessBoard(allies, opponents);
		const Bitboard duringAllies = allies.allPieces;
		int eval = -negamax(opponents, allies, copyGameState, !ourMove, -beta, -alpha, ply + 1, ++counter, pruned);
		
		if (eval > maxEval) maxEval = eval;
		
		unMakeMove(allies, opponents, moves[i]);

		if (beforeAllies != afterAllies || beforeOpponents != afterOpponents) {
			cout << "before" << endl;printBoard(beforeAllies);cout << "after" << endl;printBoard(afterAllies);
			printBoard(duringAllies);
			printChessBoard(allies, opponents);
			printBoards(allies, opponents);
			makeMove(allies, opponents, moves[i]);
			allies.updatePlayerBitboard(opponents);
			cout<<"made move"<<endl;

			printChessBoard(allies, opponents);
			unMakeMove(allies, opponents, moves[i]);
			allies.updatePlayerBitboard(opponents);
			cout<<"unmade move"<<endl;
			printChessBoard(allies, opponents);

			exit(1);
		}

		alpha = std::max(alpha, eval);

		if (beta <= alpha) {
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