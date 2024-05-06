#include "../include/move_generation.h"
#include <iostream>
using namespace std;


// testing print array:
void printMoves(Move* moves, int size) {
	cout << size<<'\n';
	for (int i = 0; i < size; i++) {
		Move curMove = moves[i];
		std::cout << "Move: "
			<< "from " << (curMove & sourceMask)
			<< ", to " << ((destinationMask & curMove) >> 6)
			<< " " << curMove
			<< '\n' << std::endl;  

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
void addMovesFromBitboard(Bitboard pieceMoves, const Bitboards bitboards, const int source, const Bitboard allPieces, Move*& moves, int& size, int& moveIndex) {
	while (pieceMoves) {
		int targetSquare = lsb(pieceMoves);
		pieceMoves &= pieceMoves - 1;
		Move move = source;
		move |= targetSquare << 6;
		move |= pieceTypeOnSq(bitboards, source) << 19;
		move |= pieceTypeOnSq(bitboards, targetSquare) << 23;
		  
		insertMove(moves, move, size, moveIndex);
	}
}

Move* generateMoves(const Bitboards& bitboards, const GameState gameState) {
	int moveIndex = 0;
	int size = 32;
	Move* moves = new Move[size];

	generatePawnMoves(bitboards, moves, gameState, size, moveIndex);
	generateKnightMoves(bitboards, moves, gameState, size, moveIndex);
	generateKingMoves(bitboards, moves, gameState, size, moveIndex);
	generateBishopMoves(bitboards, moves, gameState, size, moveIndex);
	generateRookMoves(bitboards, moves, gameState, size, moveIndex);
	generateQueenMoves(bitboards, moves, gameState, size, moveIndex);
	printMoves(moves, moveIndex);
	return moves;
}

void generateOrthagonalMoves(Bitboard& possibleMoves, Bitboard& blockers, const int source, const Bitboard allPieces) {
	Bitboard boardEdges[4] = { FILES[0], RANKS[7], FILES[7], RANKS[0]};
	int directions[4] = { 1, 8, -1, -8 };

	for (int i = 0; i < 4; i++) {
		int sq = source;
		int direction = directions[i];
		Bitboard edge = boardEdges[i];

		if ( !((1ULL << sq) & edge) ) {
			while (sq + direction < 64 && sq + direction >= 0) {
				sq += direction;
				possibleMoves |= 1ULL << sq;

				if (allPieces & (1ULL << sq)) {
					blockers |= 1ULL << sq;
					break;
				}
				if ( (1ULL << sq) & edge) break;
			}
		}
	}
	
}

void generateDiagonalMoves(Bitboard& possibleMoves, Bitboard& blockers, const int source, const Bitboard allPieces) {
	int ranksIndices[4] = { 7, 7, 0, 0 };
	int filesIndices[4] = { 7, 0, 0, 7 };
	int directions[4] = { 7, 9, -7, -9 };
	
	for (int i = 0; i < 4; i++) {
		int sq = source;
		int direction = directions[i];
		int ranksIndex = ranksIndices[i];
		int filesIndex = filesIndices[i];
		
		if (!((1ULL << sq) & RANKS[ranksIndex] || (1ULL << sq) & FILES[filesIndex])) {
			while (sq + direction < 64 && sq + direction >=0) {
				sq += direction;
				possibleMoves |= 1ULL << sq;

				if (allPieces & (1ULL << sq)) {
					blockers |= 1ULL << sq;
					break;
				}
				if (1ULL << sq & RANKS[ranksIndex] || 1ULL << sq & FILES[filesIndex]) break;
			}
		}
	}
}

void generateQueenMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	Bitboard queens = (gameState & turnMask) ? bitboards.whiteQueens : bitboards.blackQueens;
	const Bitboard friendlyPieces = (gameState & turnMask) ? bitboards.whitePieces : bitboards.blackPieces;

	while (queens) {
		int queenPosition = lsb(queens);
		Bitboard possibleMoves = 0ULL;
		Bitboard blockers = 0ULL;
		Bitboard queen = 1ULL << queenPosition;

		generateDiagonalMoves(possibleMoves, blockers, queenPosition, bitboards.allPieces);
		generateOrthagonalMoves(possibleMoves, blockers, queenPosition, bitboards.allPieces);

		possibleMoves &= ~friendlyPieces;

		printBoard(possibleMoves);

		addMovesFromBitboard(possibleMoves, bitboards, queenPosition, bitboards.allPieces, moves, size, moveIndex);

		queens &= queens - 1;
	}
}

void generateRookMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	Bitboard rooks = (gameState & turnMask) ? bitboards.whiteRooks : bitboards.blackRooks;
	const Bitboard friendlyPieces = (gameState & turnMask) ? bitboards.whitePieces : bitboards.blackPieces;

	while (rooks) {
		int rookPosition = lsb(rooks);
		Bitboard possibleMoves = 0ULL;
		Bitboard blockers = 0ULL;
		Bitboard rook = 1ULL << rookPosition;

		generateOrthagonalMoves(possibleMoves, blockers, rookPosition, bitboards.allPieces);

		possibleMoves &= ~friendlyPieces;
		printBoard(possibleMoves);
		addMovesFromBitboard(possibleMoves, bitboards, rookPosition, bitboards.allPieces, moves, size, moveIndex);

		rooks &= rooks - 1;
	}
}

void generateBishopMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	Bitboard bishops = (gameState & turnMask) ? bitboards.whiteBishops : bitboards.blackBishops;
	const Bitboard friendlyPieces = (gameState & turnMask) ? bitboards.whitePieces : bitboards.blackPieces;

	while (bishops) {
		int bishopPosition = lsb(bishops);
		Bitboard possibleMoves = 0ULL;
		Bitboard blockers = 0ULL;
		Bitboard bishop = 1ULL << bishopPosition;
		
		generateDiagonalMoves(possibleMoves, blockers, bishopPosition, bitboards.allPieces);

		possibleMoves &= ~friendlyPieces;

		addMovesFromBitboard(possibleMoves, bitboards, bishopPosition, bitboards.allPieces, moves, size, moveIndex);
		
		bishops &= bishops - 1;
	}
}


void generateKnightMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	Bitboard knights = (gameState & turnMask) ? bitboards.whiteKnights : bitboards.blackKnights;
	const Bitboard friendlyPieces = (gameState & turnMask) ? bitboards.whitePieces : bitboards.blackPieces;
	int numberOfKnights = countBits(knights);

	while(knights) {
		int knightPosition = lsb(knights);
		Bitboard possibleMoves = KNIGHT_MOVES[knightPosition] & ~friendlyPieces;
		addMovesFromBitboard(possibleMoves, bitboards, knightPosition, bitboards.allPieces, moves, size, moveIndex);

		knights &= knights - 1;
	}
}


void generateKingMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	Bitboard king = (gameState & turnMask) ? bitboards.whiteKing : bitboards.blackKing;
	const Bitboard friendlyPieces = (gameState & turnMask) ? bitboards.whitePieces : bitboards.blackPieces;

	
	int kingPosition = lsb(king);
	if (kingPosition == -1) return;
	Bitboard possibleMoves = KING_MOVES[kingPosition] & ~friendlyPieces;

	addMovesFromBitboard(possibleMoves, bitboards, kingPosition, bitboards.allPieces, moves, size, moveIndex);

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