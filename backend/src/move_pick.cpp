#include "../include/move_pick.h"
#include <limits>
#include <iostream>
using namespace std;


Move movePick(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState& gameState, bool ourMove, int alpha, int beta, int ply) {
	return 0;
}
int negamax(PlayerBitboard& allies, PlayerBitboard& opponents,const GameState& gameState, bool ourMove, int alpha, int beta, int ply, int& counter) {
	counter++;
	std::cout<< "ply: " << ply << std::endl;
	printChessBoard(allies, opponents);
	int depth = (gameState & depthMask) >> 5;
	int whoIsMoving = ourMove? 1 : -1;

	if (depth == 0) return evaluate(allies, opponents, whoIsMoving); 
	

	int bestMoveIndex = 0;
	int moveIndex = 0;
	Move* moves = generateMoves(allies, opponents, gameState, moveIndex);
	
	if (moveIndex == 0) return std::numeric_limits<int>::max();
	
	int maxEval = std::numeric_limits<int>::min();

	for (int i = 0; i < moveIndex; i++) {
		GameState copyGameState = gameState;
		PlayerBitboard* realAllies;
		PlayerBitboard* realOpponents;
		if (ourMove)
		{
			realAllies = &allies;
			realOpponents = &opponents;
		}
		else
		{
			realAllies = &opponents;
			realOpponents = &allies;

		}
		const Bitboard beforeAllies = realAllies->allPieces;
		const Bitboard beforeOpponents = realOpponents->allPieces;

		makeMove(*realAllies, *realOpponents, moves[i]);
		updateGameState(copyGameState, moves[i]);
		
		int eval = -negamax(*realAllies, *realOpponents, copyGameState, !ourMove, -beta, -alpha, ply + 1, counter);
		
		if (eval > maxEval) maxEval = eval;
		
		unMakeMove(*realAllies, *realOpponents, moves[i]);
		const Bitboard afterAllies = realAllies->allPieces;
		const Bitboard afterOpponents = realOpponents->allPieces;
		if (beforeAllies != afterAllies || beforeOpponents != afterOpponents) {
			
			std::cout << "Error in move: " << (moves[i] & sourceMask) << " to: " << ((moves[i] & destinationMask)>>6) << '\n'<<moves[i] << std::endl;
			printChessBoard(*realAllies, *realOpponents);

			makeMove(*realAllies, *realOpponents, moves[i]);

			std::cout << "After: " << std::endl;
			printChessBoard(*realAllies, *realOpponents);

			unMakeMove(*realAllies, *realOpponents, moves[i]);

			std::cout << "After unmake: "  << std::endl;
			printChessBoard(*realAllies, *realOpponents);

			printBoards(*realAllies, *realOpponents);
			exit(1);
		}
		alpha = std::max(alpha, eval);

		if (beta <= alpha) break;
		
	}
	delete[] moves;
	return maxEval;
	
}

int quiesce(int alpha, int beta, int color, int ply) {
	return 0;
}