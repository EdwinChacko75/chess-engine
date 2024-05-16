#ifndef MOVE_GENERATION_H
#define MOVE_GENERATION_H

#include "./bitboard.h"
#include "./gamestate.h"

using Move = uint32_t;

// Each move is a 32-bit integer with the following format:
// 0-5: source square (0-63)
// 6-11: destination square (0-63)
// 12-14: promotion piece (0-4, 0 = none, 1 = knight, 2 = bishop, 3 = rook, 4 = queen)
// 15: capture flag (1 if the move is a capture, 0 otherwise)
// 16: en passant flag (1 if the move is an en passant capture, 0 otherwise)
// 17: double pawn push flag (1 if the move is a double pawn push, 0 otherwise)
// 18: castling flag (1 if the move is a castling move, 0 otherwise)
// 19-22: piece type (0-6, 1 = pawn, 2 = knight, 3 = bishop, 4 = rook, 5 = queen, 6 = king)
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

// With PIECE_MOVES[0] being H1 and PIECE_MOVES[63] being A8

constexpr Bitboard FORWARD_PAWN_PUSHES[64] = {
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,
    0x1010000, 0x2020000, 0x4040000, 0x8080000, 0x10100000, 0x20200000, 0x40400000, 0x80800000, 
    0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 
    0x100000000, 0x200000000, 0x400000000, 0x800000000, 0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000,
    0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000, 0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000,
    0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000, 0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000,
    0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000, 0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000,
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL
};

constexpr Bitboard BACKWARD_PAWN_PUSHES[64] = {
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,
    0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80,
    0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000, 0x8000,
    0x10000, 0x20000, 0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000,
    0x1000000, 0x2000000, 0x4000000, 0x8000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000,
    0x100000000, 0x200000000, 0x400000000, 0x800000000, 0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000,
    0x10100000000, 0x20200000000, 0x40400000000, 0x80800000000, 0x101000000000, 0x202000000000, 0x404000000000, 0x808000000000,
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL

};

constexpr Bitboard FORWARD_PAWN_CAPTURES[64] = {
    0x200, 0x500, 0xA00, 0x1400, 0x2800, 0x5000, 0xA000, 0x4000,
    0x20000, 0x50000, 0xA0000, 0x140000, 0x280000, 0x500000, 0xA00000, 0x400000,
    0x2000000, 0x5000000, 0xA000000, 0x14000000, 0x28000000, 0x50000000, 0xA0000000, 0x40000000,
    0x200000000, 0x500000000, 0xA00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xA000000000, 0x4000000000,
    0x20000000000, 0x50000000000, 0xA0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xA00000000000, 0x400000000000,
    0x2000000000000, 0x5000000000000, 0xA000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xA0000000000000, 0x40000000000000,
    0x200000000000000, 0x500000000000000, 0xA00000000000000, 0x1400000000000000, 0x2800000000000000, 0x5000000000000000, 0xA000000000000000, 0x4000000000000000,
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL
};

constexpr Bitboard BACKWARD_PAWN_CAPTURES[64] = {
    0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL, 0ULL,
    0x2, 0x5, 0xA, 0x14, 0x28, 0x50, 0xA0, 0x40,
    0x200, 0x500, 0xA00, 0x1400, 0x2800, 0x5000, 0xA000, 0x4000,
    0x20000, 0x50000, 0xA0000, 0x140000, 0x280000, 0x500000, 0xA00000, 0x400000,
    0x2000000, 0x5000000, 0xA000000, 0x14000000, 0x28000000, 0x50000000, 0xA0000000, 0x40000000,
    0x200000000, 0x500000000, 0xA00000000, 0x1400000000, 0x2800000000, 0x5000000000, 0xA000000000, 0x4000000000,
    0x20000000000, 0x50000000000, 0xA0000000000, 0x140000000000, 0x280000000000, 0x500000000000, 0xA00000000000, 0x400000000000,
    0x2000000000000, 0x5000000000000, 0xA000000000000, 0x14000000000000, 0x28000000000000, 0x50000000000000, 0xA0000000000000, 0x40000000000000,
};

constexpr Bitboard KNIGHT_MOVES[64] = {
    0x20400ULL, 0x50800ULL, 0xA1100ULL, 0x142200ULL, 0x284400ULL, 0x508800ULL, 0xA01000ULL, 0x402000ULL,
    0x2040004ULL, 0x5080008ULL, 0xA110011ULL, 0x14220022ULL, 0x28440044ULL, 0x50880088ULL, 0xA0100010ULL, 0x40200020ULL,
    0x204000402ULL, 0x508000805ULL, 0xA1100110AULL, 0x1422002214ULL, 0x2844004428ULL, 0x5088008850ULL, 0xA0100010A0ULL, 0x4020002040ULL,
    0x20400040200ULL, 0x50800080500ULL, 0xA1100110A00ULL, 0x142200221400ULL, 0x284400442800ULL, 0x508800885000ULL, 0xA0100010A000ULL, 0x402000204000ULL,
    0x2040004020000ULL, 0x5080008050000ULL, 0xA1100110A0000ULL, 0x14220022140000ULL, 0x28440044280000ULL, 0x50880088500000ULL, 0xA0100010A00000ULL, 0x40200020400000ULL,
    0x204000402000000ULL, 0x508000805000000ULL, 0xA1100110A000000ULL, 0x1422002214000000ULL, 0x2844004428000000ULL, 0x5088008850000000ULL, 0xA0100010A0000000ULL, 0x4020002040000000ULL,
    0x400040200000000ULL, 0x800080500000000ULL, 0x1100110A00000000ULL, 0x2200221400000000ULL, 0x4400442800000000ULL, 0x8800885000000000ULL, 0x100010A000000000ULL, 0x2000204000000000ULL,
    0x4020000000000ULL, 0x8050000000000ULL, 0x110A0000000000ULL, 0x22140000000000ULL, 0x44280000000000ULL, 0x88500000000000ULL, 0x10A00000000000ULL, 0x20400000000000ULL
};


constexpr Bitboard KING_MOVES[64] = {
    0x302ULL, 0x705ULL, 0xE0AULL, 0x1C14ULL, 0x3828ULL, 0x7050ULL, 0xE0A0ULL, 0xC040ULL,
    0x30203ULL, 0x70507ULL, 0xE0A0EULL, 0x1C141CULL, 0x382838ULL, 0x705070ULL, 0xE0A0E0ULL, 0xC040C0ULL,
    0x3020300ULL, 0x7050700ULL, 0xE0A0E00ULL, 0x1C141C00ULL, 0x38283800ULL, 0x70507000ULL, 0xE0A0E000ULL, 0xC040C000ULL,
    0x302030000ULL, 0x705070000ULL, 0xE0A0E0000ULL, 0x1C141C0000ULL, 0x3828380000ULL, 0x7050700000ULL, 0xE0A0E00000ULL, 0xC040C00000ULL,
    0x30203000000ULL, 0x70507000000ULL, 0xE0A0E000000ULL, 0x1C141C000000ULL, 0x382838000000ULL, 0x705070000000ULL, 0xE0A0E0000000ULL, 0xC040C0000000ULL,
    0x3020300000000ULL, 0x7050700000000ULL, 0xE0A0E00000000ULL, 0x1C141C00000000ULL, 0x38283800000000ULL, 0x70507000000000ULL, 0xE0A0E000000000ULL, 0xC040C000000000ULL,
    0x302030000000000ULL, 0x705070000000000ULL, 0xE0A0E0000000000ULL, 0x1C141C0000000000ULL, 0x3828380000000000ULL, 0x7050700000000000ULL, 0xE0A0E00000000000ULL, 0xC040C00000000000ULL,
    0x203000000000000ULL, 0x507000000000000ULL, 0x0A0E000000000000ULL, 0x141C000000000000ULL, 0x2838000000000000ULL, 0x5070000000000000ULL, 0xA0E0000000000000ULL, 0x40C0000000000000ULL
};


void addMovesFromBitboard(Bitboard pieceMoves, const PlayerBitboard& allies, const PlayerBitboard& opponents,
	const int source, const Bitboard allPieces, Move*& moves, int& size, int& moveIndex);

void makeTestMove(PlayerBitboard& allies, PlayerBitboard& opponents, Move move);

void unMakeTestMove(PlayerBitboard& allies, PlayerBitboard& opponents, Move move);
bool isKingInCheck(const PlayerBitboard& allies, const PlayerBitboard& opponents, const GameState gameState);
void filterLegalMoves(const PlayerBitboard& ally, const PlayerBitboard& opponent,
	Move*& moves, const GameState gamestate, int& size, int& moveIndex);

Move* generateMoves(PlayerBitboard& allies, PlayerBitboard& opponents, const GameState gamestate, int& moveIndex);


void generatePawnMoves(const PlayerBitboard& allies, const PlayerBitboard& opponents, Move*& moves, const GameState gameState, int& size, int& moveIndex);


void generateKnightMoves(const PlayerBitboard& allies, const PlayerBitboard& opponents,
	Move*& moves, const GameState gameState, int& size, int& moveIndex);


void generateKingMoves(PlayerBitboard& allies, PlayerBitboard& opponents,
	Move*& moves, const GameState gameState, int& size, int& moveIndex);

void generateCastlingMoves(PlayerBitboard& allies, PlayerBitboard& opponents, Move*& moves, const GameState gameState, int& size, int& moveIndex);


void generateQueenMoves(const PlayerBitboard& allies, const PlayerBitboard& opponents,
	Move*& moves, const GameState gameState, int& size, int& moveIndex);

void generateBishopMoves(const PlayerBitboard& allies, const PlayerBitboard& opponents,
	Move*& moves, const GameState gameState, int& size, int& moveIndex);

void generateDiagonalMoves(Bitboard& possibleMoves, Bitboard& blockers, const int sq, const Bitboard allPieces);

void generateRookMoves(const PlayerBitboard& allies, const PlayerBitboard& opponents,
	Move*& moves, const GameState gameState, int& size, int& moveIndex);

void generateOrthagonalMoves(Bitboard& possibleMoves, Bitboard& blockers, const int sq, const Bitboard allPieces);


void printMoves(PlayerBitboard& allies, PlayerBitboard& opponents, Move* moves, int size);




void updateGameState(GameState& gameState, const Move move);

#endif