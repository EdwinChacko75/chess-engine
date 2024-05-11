#include "../include/move_generation.h"
#include <iostream>
using namespace std;


// testing print array:
void printMoves(PlayerBitboard& allies, PlayerBitboard& opponents, Move* moves, int size) {
	cout << size<<'\n';
	for (int i = 0; i < size; i++) {
		Move curMove = moves[i];
		std::cout << "Move: "
			<< "from " << (curMove & sourceMask)
			<< ", to " << ((destinationMask & curMove) >> 6)
			<< " " << curMove
			<< '\n' << std::endl;  
		makeMove(allies, opponents, curMove);
		printChessBoard(allies, opponents);
		unMakeMove(allies, opponents, curMove);
	}
}
static void insertMove(Move*& moves, const Move move, int& size, int& moveIndex) {
	if (moveIndex == size) {
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

static void addPromotionMoves(const PlayerBitboard& allies, const PlayerBitboard& opponents, Move*& moves, Bitboard promotionMoves, const int source, int& size, int& moveIndex) {
	Move promoteToQueen = source | 4UL << 12 | 1UL << 19;
	Move promoteToRook = source | 3UL << 12 | 1UL << 19;
	Move promoteToBishop = source | 2UL << 12 | 1UL << 19;
	Move promoteToKnight = source | 1UL << 12 | 1UL << 19;

	while (promotionMoves) {
		int promotionSquare = lsb(promotionMoves);
		promoteToQueen |= promotionSquare << 6;
		promoteToRook |= promotionSquare << 6;
		promoteToBishop |= promotionSquare << 6;
		promoteToKnight |= promotionSquare << 6;

		if ( !(source - promotionSquare == 8 || source - promotionSquare == -8) ) {
			promoteToQueen |= captureMask;
			promoteToRook |= captureMask;
			promoteToBishop |= captureMask;
			promoteToKnight |= captureMask;

			promoteToQueen |= pieceTypeOnSq(allies, opponents, promotionSquare) << 23;
			promoteToRook |= pieceTypeOnSq(allies, opponents, promotionSquare) << 23;
			promoteToBishop |= pieceTypeOnSq(allies, opponents, promotionSquare) << 23;
			promoteToKnight |= pieceTypeOnSq(allies, opponents, promotionSquare) << 23;
		}

		insertMove(moves, promoteToQueen, size, moveIndex);
		insertMove(moves, promoteToRook, size, moveIndex);
		insertMove(moves, promoteToBishop, size, moveIndex);
		insertMove(moves, promoteToKnight, size, moveIndex);

		promotionMoves &= promotionMoves - 1;
	}

}

void addMovesFromBitboard(Bitboard pieceMoves, const PlayerBitboard& allies, const PlayerBitboard& opponents, const int source, const Bitboard allPieces, Move*& moves, int& size, int& moveIndex) {
	int pieceType = pieceTypeOnSq(allies, opponents, source);
	while (pieceMoves) {
		int targetSquare = lsb(pieceMoves);
		pieceMoves &= pieceMoves - 1;
		Move move = source;
		move |= targetSquare << 6;
		move |= pieceType << 19;
		int capturedPieceType = pieceTypeOnSq(allies, opponents, targetSquare);
		if (capturedPieceType) {
			move |= captureMask;
			move |= capturedPieceType << 23;
		}
		insertMove(moves, move, size, moveIndex);
	}
}

void makeMove(PlayerBitboard& allies, PlayerBitboard& opponents, Move move) {
	int pieceType = (move & pieceTypeMask) >> 19;
	int source = move & sourceMask;
	int destination = (move & destinationMask) >> 6;
	
	switch (pieceType) {
		case 1: // pawn
			
			allies.pawns = clearBit(allies.pawns, source);
			allies.pawns = setBit(allies.pawns, destination);
			break;
		case 2: // knight
			allies.knights = clearBit(allies.knights, source);
			allies.knights = setBit(allies.knights, destination);
			break;
		case 3: // bishop
			allies.bishops = clearBit(allies.bishops, source);
			allies.bishops = setBit(allies.bishops, destination);
			break;
		case 4: // rook
			allies.rooks = clearBit(allies.rooks, source);
			allies.rooks = setBit(allies.rooks, destination);
			break;
		case 5: // queen
			allies.queens = clearBit(allies.queens, source);
			allies.queens = setBit(allies.queens, destination);
			break;
		case 6: // king
			allies.king = clearBit(allies.king, source);
			allies.king = setBit(allies.king, destination);
			allies.kingPosition = destination;
			break;
		default:
			break;
	}

	if (move & captureMask) {
		int capturedPieceType = (move & capturedPieceMask) >> 23;

		switch (capturedPieceType) {
			case 1: // pawn
				opponents.pawns = clearBit(opponents.pawns, destination);
				break;
			case 2: // knight
				opponents.knights = clearBit(opponents.knights, destination);
				break;
			case 3: // bishop
				opponents.bishops = clearBit(opponents.bishops, destination);
				break;
			case 4: // rook
				opponents.rooks = clearBit(opponents.rooks, destination);
				break;
			case 5: // queen
				opponents.queens = clearBit(opponents.queens, destination);
				break;
			case 6: // king
				opponents.king = clearBit(opponents.king, destination);
				break;
			default:
				break;
		}
	}
	
	if (move & castlingMask) {
		if (destination == 1) {
			allies.rooks = clearBit(allies.rooks, 0);
			allies.rooks = setBit(allies.rooks, 2);
		}
		else if (destination == 5) {
			allies.rooks = clearBit(allies.rooks, 7);
			allies.rooks = setBit(allies.rooks, 4);
		}
		else if (destination == 59) {
			allies.rooks = clearBit(allies.rooks, 56);
			allies.rooks = setBit(allies.rooks, 58);
		}
		else if (destination == 61) {
			allies.rooks = clearBit(allies.rooks, 63);
			allies.rooks = setBit(allies.rooks, 60);
		}
	}

	if (move & enPassantMask) {
		int change = source - destination;
		if (change == 9 || change == 7) { // black enpassant
			opponents.pawns = clearBit(opponents.pawns, destination + 8);
		}
		else if (change == -7 || change == -9) { // white enpassant
			opponents.pawns = clearBit(opponents.pawns, destination - 8);
		}
	}

	allies.updatePlayerBitboard(opponents);

}

void unMakeMove(PlayerBitboard& allies, PlayerBitboard& opponents, Move move) {
	int pieceType = (move & pieceTypeMask) >> 19;
	int source = move & sourceMask;
	int destination = (move & destinationMask) >> 6;


	switch (pieceType) {
		case 1: // pawn
			allies.pawns = clearBit(allies.pawns, destination);
			allies.pawns = setBit(allies.pawns, source);
			break;
		case 2: // knight
			allies.knights = clearBit(allies.knights, destination);
			allies.knights = setBit(allies.knights, source);
			break;
		case 3: // bishop
			allies.bishops = clearBit(allies.bishops, destination);
			allies.bishops = setBit(allies.bishops, source);
			break;
		case 4: // rook
			allies.rooks = clearBit(allies.rooks, destination);
			allies.rooks = setBit(allies.rooks, source);
			break;
		case 5: // queen
			allies.queens = clearBit(allies.queens, destination);
			allies.queens = setBit(allies.queens, source);
			break;
		case 6: // king
			allies.king = clearBit(allies.king, destination);
			allies.king = setBit(allies.king, source);
			allies.kingPosition = source;
			break;
		default:
			break;
	}

	if (move & captureMask && !(move & enPassantMask)) {
		int capturedPieceType = (move & capturedPieceMask) >> 23;
		switch (capturedPieceType) {
			case 1: // pawn
				opponents.pawns = setBit(opponents.pawns, destination);
				break;
			case 2: // knight
				opponents.knights = setBit(opponents.knights, destination);
				break;
			case 3: // bishop
				opponents.bishops = setBit(opponents.bishops, destination);
				break;
			case 4: // rook
				opponents.rooks = setBit(opponents.rooks, destination);
				break;
			case 5: // queen
				opponents.queens = setBit(opponents.queens, destination);
				break;
			case 6: // king
				opponents.king = setBit(opponents.king, destination);
				break;
			default:
				break;
		}
	}

	if (move & castlingMask) {
		if (destination == 1) {
			allies.rooks = clearBit(allies.rooks, 2);
			allies.rooks = setBit(allies.rooks, 0);
		}
		else if (destination == 5) {
			allies.rooks = clearBit(allies.rooks, 4);
			allies.rooks = setBit(allies.rooks, 7);
		}
		else if (destination == 59) {
			allies.rooks = clearBit(allies.rooks, 58);
			allies.rooks = setBit(allies.rooks, 56);
		}
		else if (destination == 61) {
			allies.rooks = clearBit(allies.rooks, 60);
			allies.rooks = setBit(allies.rooks, 63);
		}
	}
	if (move & enPassantMask) {
		int change = source - destination;
		if (change == 9 || change == 7) { // black enpassant
			opponents.pawns = setBit(opponents.pawns, destination + 8);
		}
		else if (change == -7 || change == -9) { // white enpassant
			opponents.pawns = setBit(opponents.pawns, destination - 8);
		}
	}
	
}

bool isKingInCheck(const PlayerBitboard& allies, const PlayerBitboard& opponents, const GameState gameState, const Move move) {

	Bitboard diagonalBlockers = 0ULL;
	Bitboard orthagonalBlockers = 0ULL;
	Bitboard diagonalAttacks = 0ULL;
	Bitboard orthagonalAttacks = 0ULL;
	Bitboard knightAttacks = KNIGHT_MOVES[allies.kingPosition];	
	Bitboard kingAttacks = KING_MOVES[opponents.kingPosition];
	generateDiagonalMoves(diagonalAttacks, diagonalBlockers, allies.kingPosition, allies.allPieces);
	generateOrthagonalMoves(orthagonalAttacks, orthagonalBlockers, allies.kingPosition, allies.allPieces);
	Bitboard pawnAttacks = gameState & turnMask ? FORWARD_PAWN_CAPTURES[allies.kingPosition] : BACKWARD_PAWN_CAPTURES[allies.kingPosition];
	
	return opponents.knights & knightAttacks || opponents.pawns & pawnAttacks ||
		opponents.king & kingAttacks || opponents.queens & (diagonalAttacks | orthagonalAttacks) || opponents.rooks & orthagonalAttacks ||
		opponents.bishops & diagonalAttacks;

}

void filterLegalMoves(const PlayerBitboard& ally, const PlayerBitboard& opponent, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	
	PlayerBitboard allies = ally;
	PlayerBitboard opponents = opponent;
	int kingPosition = allies.kingPosition;
	int kept = 0;

	Move* legalMoves = new Move[moveIndex];

	for (int i = 0; i <  moveIndex; i++) {
		
		makeMove(allies, opponents, moves[i]);

		if (!isKingInCheck(allies, opponents, gameState, moves[i])) legalMoves[kept++] = moves[i];
		
		unMakeMove(allies, opponents, moves[i]);
	}
	delete[] moves;

	moves = legalMoves;
	moveIndex = kept;
	
}

Move* generateMoves(PlayerBitboard& white, PlayerBitboard& black, const GameState gameState) {
	int moveIndex = 0;
	int size = 32;

	PlayerBitboard& allies = (gameState & turnMask) ? white : black;
	PlayerBitboard& opponents = (gameState & turnMask) ? black : white;

	Move* moves = new Move[size];

	generatePawnMoves(allies, opponents, moves, gameState, size, moveIndex);
	generateKnightMoves(allies, opponents, moves, gameState, size, moveIndex);
	generateKingMoves(allies, opponents, moves, gameState, size, moveIndex);
	generateBishopMoves(allies, opponents, moves, gameState, size, moveIndex);
	generateRookMoves(allies, opponents, moves, gameState, size, moveIndex);
	generateQueenMoves(allies, opponents, moves, gameState, size, moveIndex);

	filterLegalMoves(allies, opponents, moves, gameState, size, moveIndex);

	printMoves(allies, opponents, moves, moveIndex);
	return moves;
}

// Generate Moves helpers:

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

void generateCastlingMoves(PlayerBitboard& allies, PlayerBitboard& opponents, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	if (isKingInCheck(allies, opponents, gameState, 0)) return;
	int turn = gameState & turnMask;
	if (gameState & whiteKingsideCastleMask && turn && !(0x6 & allies.allPieces) ) {	// white kingside castle
		Move castlingCondition = 3;
		castlingCondition |= 2 << 6;
		castlingCondition |= (6 << 19);
		makeMove(allies, opponents, castlingCondition);
		if (!isKingInCheck(allies, opponents, gameState, castlingCondition)) {
			Move castlingMove = 3;
			castlingMove |= 1 << 6;
			castlingMove |= (6 << 19) | (1 << 18);
			insertMove(moves, castlingMove, size, moveIndex);
		}
		unMakeMove(allies, opponents, castlingCondition);
	}

	if (gameState & whiteQueensideCastleMask && turn && !(0x70 & allies.allPieces) ) {	// white queenside castle
		Move castlingCondition = 3;
		castlingCondition |= 4 << 6;
		castlingCondition |= (6 << 19);
		makeMove(allies, opponents, castlingCondition);
		if (!isKingInCheck(allies, opponents, gameState, castlingCondition)) {
			Move castlingMove = 3;
			castlingMove |= 5 << 6;
			castlingMove |= (6 << 19) | (1 << 18);
			insertMove(moves, castlingMove, size, moveIndex);
		}
		unMakeMove(allies, opponents, castlingCondition);
	}

	if (gameState & blackKingsideCastleMask && !turn && !(0x600000000000000 & allies.allPieces)) {	// black kingside castle
		Move castlingCondition = 59;
		castlingCondition |= 58 << 6;
		castlingCondition |= (6 << 19);
		makeMove(allies, opponents, castlingCondition);
		if (!isKingInCheck(allies, opponents, gameState, castlingCondition)) {
			Move castlingMove = 59;
			castlingMove |= 57 << 6;
			castlingMove |= (6 << 19) | (1 << 18);
			insertMove(moves, castlingMove, size, moveIndex);
		}
		unMakeMove(allies, opponents, castlingCondition);
	}

	if (gameState & blackQueensideCastleMask && !turn && !(0x7000000000000000 & allies.allPieces)) {	// black queenside castle
		Move castlingCondition = 59;
		castlingCondition |= 60 << 6;
		castlingCondition |= (6 << 19);
		makeMove(allies, opponents, castlingCondition);
		if (!isKingInCheck(allies, opponents, gameState, castlingCondition)) {
			Move castlingMove = 59;
			castlingMove |= 61 << 6;
			castlingMove |= (6 << 19) | (1 << 18);
			insertMove(moves, castlingMove, size, moveIndex);
		}
		unMakeMove(allies, opponents, castlingCondition);
		
	}
}

void generateQueenMoves(const PlayerBitboard& allies, const PlayerBitboard& opponents, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	// can remove the need to pass opponents into this function 
	// by adding the moves from bitboard in the generateMoves function
	Bitboard queens = allies.queens;

	while (queens) {
		int queenPosition = lsb(queens);
		Bitboard possibleMoves = 0ULL;
		Bitboard blockers = 0ULL;
		Bitboard queen = 1ULL << queenPosition;

		generateDiagonalMoves(possibleMoves, blockers, queenPosition, allies.allPieces);
		generateOrthagonalMoves(possibleMoves, blockers, queenPosition, allies.allPieces);

		possibleMoves &= ~allies.friendlyPieces;

		addMovesFromBitboard(possibleMoves, allies, opponents, queenPosition, allies.allPieces, moves, size, moveIndex);

		queens &= queens - 1;
	}
}


void generateRookMoves(const PlayerBitboard& allies, const PlayerBitboard& opponents, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	Bitboard rooks = allies.rooks;

	while (rooks) {
		int rookPosition = lsb(rooks);
		Bitboard possibleMoves = 0ULL;
		Bitboard blockers = 0ULL;
		Bitboard rook = 1ULL << rookPosition;

		generateOrthagonalMoves(possibleMoves, blockers, rookPosition, allies.allPieces);

		possibleMoves &= ~allies.friendlyPieces;

		addMovesFromBitboard(possibleMoves, allies, opponents, rookPosition, allies.allPieces, moves, size, moveIndex);

		rooks &= rooks - 1;
	}

	
}

void generateBishopMoves(const PlayerBitboard& allies, const PlayerBitboard& opponents, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	Bitboard bishops = allies.bishops;

	while (bishops) {
		int bishopPosition = lsb(bishops);
		Bitboard possibleMoves = 0ULL;
		Bitboard blockers = 0ULL;
		Bitboard bishop = 1ULL << bishopPosition;
		
		generateDiagonalMoves(possibleMoves, blockers, bishopPosition, allies.allPieces);

		possibleMoves &= ~allies.friendlyPieces;

		addMovesFromBitboard(possibleMoves, allies, opponents, bishopPosition, allies.allPieces, moves, size, moveIndex);

		bishops &= bishops - 1;
	}
}


void generateKnightMoves(const PlayerBitboard& allies, const PlayerBitboard& opponents, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	Bitboard knights = allies.knights;

	while(knights) {
		int knightPosition = lsb(knights);
		Bitboard possibleMoves = KNIGHT_MOVES[knightPosition] & ~allies.friendlyPieces;
	
		// not efficent to call this each time possible moves is updated
		// move outside loop and declare pM out as well
		addMovesFromBitboard(possibleMoves, allies, opponents, knightPosition, allies.allPieces, moves, size, moveIndex);

		knights &= knights - 1;
	}
}


void generateKingMoves(PlayerBitboard& allies, PlayerBitboard& opponents, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	Bitboard king = allies.king;
	int kingPosition = allies.kingPosition;

	if (kingPosition == -1) return;

	Bitboard possibleMoves = KING_MOVES[kingPosition] & ~allies.friendlyPieces;

	addMovesFromBitboard(possibleMoves, allies, opponents, kingPosition, allies.allPieces, moves, size, moveIndex);

	generateCastlingMoves(allies, opponents, moves, gameState, size, moveIndex);

}



static void generatePawnMoves(const PlayerBitboard& allies, const PlayerBitboard& opponents, Move*& moves, const GameState gameState, int& size, int& moveIndex) {
	int turn = gameState & turnMask;
	const Bitboard* pawnPushesArray = turn ? FORWARD_PAWN_PUSHES : BACKWARD_PAWN_PUSHES;
	const Bitboard* pawnCapturesArray = turn ? FORWARD_PAWN_CAPTURES : BACKWARD_PAWN_CAPTURES;
	Bitboard promotionRank = turn ? RANKS[7] : RANKS[0];

	Bitboard pawns = allies.pawns;

	int enPassantSquare = (gameState & enPassantFileMask) >> 10;
	Bitboard enPassantTargetMask = 1ULL << enPassantSquare;

	while (pawns) {
		int pawnPosition = lsb(pawns);
		Bitboard currentSquareMask = 1ULL << pawnPosition;
		Bitboard possibleMoves = pawnPushesArray[pawnPosition] & ~allies.friendlyPieces;
		possibleMoves |= pawnCapturesArray[pawnPosition] & opponents.friendlyPieces;

		if (pawnCapturesArray[pawnPosition] & enPassantTargetMask) {
			Move move = pawnPosition | (enPassantSquare << 6);
			move |= captureMask;
			move |= enPassantMask;
			move |= 1UL << 19;
			move |= (1UL << 23);
			insertMove(moves, move, size, moveIndex);
		}

		if (possibleMoves & promotionRank) {
			Bitboard promotionMoves = promotionRank & possibleMoves;
			possibleMoves &= ~promotionRank;

			addPromotionMoves(allies, opponents, moves, promotionMoves, pawnPosition, size, moveIndex);
		}
		else {
			addMovesFromBitboard(possibleMoves, allies, opponents, pawnPosition, allies.allPieces, moves, size, moveIndex);
		}
		

		pawns &= pawns - 1;
	}


}