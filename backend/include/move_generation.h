#ifndef MOVE_GENERATION_H
#define MOVE_GENERATION_H

#include "./bitboard.h"

using Move = uint32_t;

// Each move is a 32-bit integer with the following format:
// 0-5: source square (0-63)
// 6-11: destination square (0-63)
// 12-14: promotion piece (0-4, 0 = none, 1 = queen, 2 = rook, 3 = bishop, 4 = knight)
// 15: capture flag (1 if the move is a capture, 0 otherwise)
// 16: en passant flag (1 if the move is an en passant capture, 0 otherwise)
// 17: double pawn push flag (1 if the move is a double pawn push, 0 otherwise)
// 18: castling flag (1 if the move is a castling move, 0 otherwise)
// 19-22: piece type (0-5, 0 = pawn, 1 = knight, 2 = bishop, 3 = rook, 4 = queen, 5 = king)
// 23-26: captured piece type (0-6, 0 = None, 1 = pawn, 2 = knight, 3 = bishop, 4 = rook, 5 = queen, 6 = king)
// use the remaining for move search ordering during move desciion making

constexpr Move sourceMask = 63UL;
constexpr Move destinationMask = (63UL << 6);
constexpr Move promotionMask = (7UL << 12);
constexpr Move captureMask = (1UL << 15);
constexpr Move enPassantMask = (1UL << 16);
constexpr Move doublePawnPushMask = (1UL << 17);
constexpr Move castlingMask = (1UL << 18);
constexpr Move pieceTypeMask = (15UL << 19);
constexpr Move capturedPieceMask = (15UL << 23);


void insertMove(Move*& moves, const Move move, int& size, int& moveIndex);

void addMovesFromBitboard(Bitboard pieceMoves, const Bitboards bitboards, const int source, const Bitboard allPieces, Move*& moves, int& size, int& moveIndex);

Move* generateMoves(const Bitboards& bitboards, const GameState gamestate);

void generatePawnMoves(const Bitboards& bitboards, Move*& moves, const GameState gamestate, int& size, int& moveIndex);

void generateKnightMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex);

void generateKingMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex);
void generateCastlingMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex);



void generateQueenMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex);

void generateBishopMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex);
void generateDiagonalMoves(Bitboard& possibleMoves, Bitboard& blockers, const int sq, const Bitboard allPieces);

void generateRookMoves(const Bitboards& bitboards, Move*& moves, const GameState gameState, int& size, int& moveIndex);
void generateOrthagonalMoves(Bitboard& possibleMoves, Bitboard& blockers, const int sq, const Bitboard allPieces);


void printMoves(Move* moves, int size);


#endif