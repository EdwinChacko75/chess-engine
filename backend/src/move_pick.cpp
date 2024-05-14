#include "../include/move_pick.h"
#include <limits>
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

		//throw std::runtime_error("Error: No piece on source square");
	}
	allies.updatePlayerBitboard(opponents);

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
	allies.updatePlayerBitboard(opponents);

}
Move movePick(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, bool ourMove, int alpha, int beta, int ply) {
	return 0;
}
int negamax(PlayerBitboard& allies, PlayerBitboard& opponents,const GameState& gameState, bool ourMove, int alpha, int beta, int ply, int& counter) {
	counter++;
	//std::cout<< "ply: " << ply << std::endl;
	printChessBoard(allies, opponents);

	int depth = (gameState & depthMask) >> 5;
	int whoIsMoving = ourMove ? 1 : -1;
	bool turn = (gameState & turnMask) >> 4;

	if (depth == 0) return whoIsMoving * evaluate(allies, opponents);
	

	int bestMoveIndex = 0;
	int moveIndex = 0;
	Move* moves = generateMoves(allies, opponents, gameState, moveIndex);
	//cout << "at depth : " << (depth) << endl;
	//printChessBoard(allies, opponents);
	//printMoves(allies, opponents, moves, moveIndex);
	if (moveIndex == 0) return std::numeric_limits<int>::max();
	
	int maxEval = std::numeric_limits<int>::min();

	for (int i = 0; i < moveIndex; i++) {
		//cout << "at depth : " << (3 - depth) << " move: " << (i + 1) << endl;
		//printBoards(allies, opponents);
		GameState copyGameState = gameState;

		
		const Bitboard beforeAllies = allies.allPieces;
		const Bitboard beforeOpponents = opponents.allPieces;

		makeMove(allies, opponents, moves[i]);
		allies.updatePlayerBitboard(opponents);
		//printChessBoard(allies, opponents);

		updateGameState(copyGameState, moves[i]);
		
		int eval = -negamax(opponents, allies, copyGameState, !ourMove, -beta, -alpha, ply + 1, counter);
		
		if (eval > maxEval) maxEval = eval;
		
		unMakeMove(allies, opponents, moves[i]);
		allies.updatePlayerBitboard(opponents);
		const Bitboard& afterAllies = allies.allPieces;
		const Bitboard& afterOpponents = opponents.allPieces;


		if (beforeAllies != afterAllies || beforeOpponents != afterOpponents) {
			cout << "before" << endl;printBoard(beforeAllies);cout << "after" << endl;printBoard(afterAllies);		
			cout << "ourMove: " << ourMove << " are we white: " << allies.isWhite << endl;
			exit(1);
		}

		alpha = std::max(alpha, eval);

		//if (beta <= alpha) break;
		
	}
	delete[] moves;
	return maxEval;
	
}

int quiesce(int alpha, int beta, int color, int ply) {
	return 0;
}