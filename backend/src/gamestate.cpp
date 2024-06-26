#include "../include/gamestate.h"
#include <iostream>// only in dev.


void initGameState(GameState& gameState, JSgameState jsState){
	gameState |= jsState.whiteKingsideCastling;
	gameState |= jsState.whiteQueensideCastling << 1;
	gameState |= jsState.blackKingsideCastling << 2;
	gameState |= jsState.blackQueensideCastling << 3;
	gameState |= jsState.turn << 4;
	gameState |= jsState.depth << 5;
	gameState |= jsState.enPassant << 10;
}


