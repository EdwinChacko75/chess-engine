#include "../include/move_generation.h"
#include <iostream>
using namespace std;


// testing print array:
void printMoves(Move* moves, int size) {
	cout << size<<'\n';
	for (int i = 0; i < size; i++) {
		cout << i<<": " << (moves[i]) << '\n' << endl;

	}
}
static void promotionMoves(Move*& moves, const Move move, int& size, int& moveIndex) {
	Move promoteToQueen = move;
	Move promoteToRook = move;
	Move promoteToBishop = move;
	Move promoteToKnight = move;
	promoteToQueen |= 4UL << 12;
	promoteToRook |= 3UL << 12;
	promoteToBishop |= 2UL << 12;
	promoteToKnight |= 1UL << 12;
	insertMove(moves, promoteToQueen, size, moveIndex);
	insertMove(moves, promoteToRook, size, moveIndex);
	insertMove(moves, promoteToBishop, size, moveIndex);
	insertMove(moves, promoteToKnight, size, moveIndex);
}


static void insertMove(Move*& moves, const Move move, int& size, int& moveIndex) {
	if (moveIndex == size) {
		cout<< "resizing" << endl;
		int newSize = size * 2;
		Move* newMoves = new Move[newSize];

		for (int i = 0; i < size; i++) {
			newMoves[i] = moves[i];
		}
		// Initialize new elements
		for (int i = size; i < newSize; i++) {
			newMoves[i] = 0;  // Or any other default value appropriate for Move
		}
		delete[] moves;
		moves = newMoves;

		size = newSize;
	}
	moves[moveIndex] = move;
	moveIndex++;
}

Move* generateMoves(const Bitboards& bitboards, const GameState gameState) {
	int moveIndex = 0;
	int size = 32;
	Move* moves = new Move[size];

	generatePawnMoves(bitboards, moves, gameState, size, moveIndex);
	generateKnightMoves(bitboards, moves, gameState, size, moveIndex);
	generateKingMoves(bitboards, moves, gameState, size, moveIndex);
	printMoves(moves, moveIndex);
	return moves;
}



void generateKnightMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	Bitboard knights = (gameState & turnMask) ? bitboards.whiteKnights : bitboards.blackKnights;
	const Bitboard friendlyPieces = (gameState & turnMask) ? bitboards.whitePieces : bitboards.blackPieces;
	int numberOfKnights = countBits(knights);

	while(knights) {
		int knightPosition = lsb(knights);
		Bitboard possibleMoves = KNIGHT_MOVES[knightPosition] & ~friendlyPieces;
		while (possibleMoves) {
			int targetSquare = lsb(possibleMoves);
			possibleMoves &= possibleMoves - 1;
			Move move = knightPosition;
			move |= targetSquare << 6;
			move |= (1UL << 19);
			move |= pieceTypeOnSq(bitboards, targetSquare) << 23;

			insertMove(moves, move, size, moveIndex);
		}
		knights &= knights - 1;
	}
	
}





void generateKingMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	Bitboard king = (gameState & turnMask) ? bitboards.whiteKing : bitboards.blackKing;
	const Bitboard friendlyPieces = (gameState & turnMask) ? bitboards.whitePieces : bitboards.blackPieces;

	
	int kingPosition = lsb(king);
	Bitboard possibleMoves = KING_MOVES[kingPosition] & ~friendlyPieces;
	while (possibleMoves) {
		int targetSquare = lsb(possibleMoves);
		possibleMoves &= possibleMoves - 1;
		Move move = kingPosition;
		move |= targetSquare << 6;
		move |= (1UL << 19);
		move |= pieceTypeOnSq(bitboards, targetSquare) << 23;

		insertMove(moves, move, size, moveIndex);
	}

}






















static void generatePawnMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	// precompute pawn pushes and captures
	int pawnDirection = (gameState & turnMask) ? 8 : -8;
	const Bitboard pawns = (gameState & turnMask) ? 0ULL | bitboards.whitePawns : 0ULL | bitboards.blackPawns;
	const Bitboard enemyPieces = (gameState & turnMask) ? 0ULL | bitboards.blackPieces : 0ULL | bitboards.whitePieces;
	int numberOfPawns = countBits(pawns);
	// use while loop for pawns as well later if possible
	// probably better to just use while for pawn pushes and captures and precompute. 
	// And for enpassant do some kind of for loop maybe
	for (int i = 0; i < 64; i++) {
		// add some logic to store how many pawns have been found and compare to the number of pawns in the bitboard
		// to avoid iterating over the whole board
		Bitboard currentSquareMask = 1ULL << i;

		if (numberOfPawns == 0) break;

		
		if (!(pawns & currentSquareMask)) continue; // is a pawn on cur square
			
		
		
		numberOfPawns--;
		int isPawnAboutToPromote = 0;
		if (pawnDirection > 0 && i > 47  || pawnDirection < 0 && i <16) isPawnAboutToPromote = 1; // is the pawn on the last rank
		



		// since we use 1d array, check that the pawn is not on the H file (where it cannot capture right) 
		// and simultaneously if the targeted piece is an enemy piece. -> the conditions for right capture
		if (! (currentSquareMask & HFile) && enemyPieces & (currentSquareMask << (pawnDirection - 1)) ) { 
			Move move = i;
			move |= (i + pawnDirection - 1) << 6;
			move |= captureMask;
			// captured piece type
			move |= (pieceTypeOnSq(bitboards, (i + pawnDirection - 1)) + 0UL) << 23;

			if (!isPawnAboutToPromote) insertMove(moves, move, size, moveIndex);
			if (isPawnAboutToPromote) promotionMoves(moves, move, size, moveIndex);
		}
		if (! (currentSquareMask & AFile) && enemyPieces & (currentSquareMask << (pawnDirection + 1)) ) { // left capture

			Move move = i;
			move |= (i + pawnDirection + 1) << 6;
			move |= captureMask;
			// captured piece type
			move |= pieceTypeOnSq(bitboards, (i + pawnDirection + 1))  << 23;
			if (!isPawnAboutToPromote) insertMove(moves, move, size, moveIndex);
			if (isPawnAboutToPromote) promotionMoves(moves, move, size, moveIndex);
		}
		// 63, 62, 61, 60, 59, 58, 57, 56, 
		// 55, 54, 53, 52, 51, 50, 49, 48, 
		// 47, 46, 45, 44, 43, 42, 41, 40, 
		// 39, 38, 37, 36, 35, 34, 33, 32, // white pawns ep
		// 31, 30, 29, 28, 27, 26, 25, 24, 
		// 23, 22, 21, 20, 19, 18, 17, 16, 
		// 15, 14, 13, 12, 11, 10, 9,  8, 
		// 7,  6,  5,  4,  3,  2,  1,  0
		int enPassantFile = (gameState & enPassantFileMask) >> 10;
		if (enPassantFile == 9) {// no file

		}
		else if (pawnDirection > 0 && 31 < i && i < 40 || pawnDirection < 0 && 23 < i && i < 32) {
			if ((i + 1) % 8 == enPassantFile && !(currentSquareMask & AFile)) {// left enPassant
				Move move = i;
				move |= (i + 1 + pawnDirection) << 6;
				move |= captureMask;
				// maybe add more than just the flag for enPassant later, cus rn its 
				// not clear how we are going to remove the captured pawn effectively
				move |= enPassantMask;
				insertMove(moves, move, size, moveIndex);

			}
			if ((i - 1) % 8 == enPassantFile && !(currentSquareMask & HFile)) {// right enPassant
				Move move = i;
				move |= (i - 1 + pawnDirection) << 6;
				move |= captureMask;
				move |= enPassantMask;
			

				insertMove(moves, move, size, moveIndex);
			}
		}
		if (!(bitboards.allPieces & (currentSquareMask << pawnDirection))) { // single pawn push
			Move move = i;
			move |= (i + pawnDirection) << 6;
			// piece type is pawn is 0 so no need to change it
			if (!isPawnAboutToPromote) insertMove(moves, move, size, moveIndex);
			if (isPawnAboutToPromote) promotionMoves(moves, move, size, moveIndex);

		}
		else { // pawn is blocked
			continue;
		}
		if (pawnDirection > 0 && i > 15 || pawnDirection < 0 && i < 48) continue; // is the pawn on its starting rank

		if(!(bitboards.allPieces & (currentSquareMask << (2 * pawnDirection))) ) { // double pawn push
			Move move = i;
			move |= (i + 2 * pawnDirection) << 6;
			move |= doublePawnPushMask; // double pawn push flag
			// piece type is pawn is 0 so no need to do anything
			insertMove(moves, move, size, moveIndex);
		}
		
		
	}
}