#ifndef GAMESTATE_H
#define GAMESTATE_H

//#include "./bitboard.h"
#include <cstdint>

typedef struct {
    int turn;
    int depth;
    int enPassant;
    int whiteKingsideCastling;
    int whiteQueensideCastling;
    int blackKingsideCastling;
    int blackQueensideCastling;
    int whiteKingPosition;
    int blackKingPosition;
    int checkStatus;
} JSgameState;

// Using the uint16_t to represent the GameState
using GameState = uint32_t;

// The game state is a 16-bit integer with the following format:
// 0-3: Castling rights (0 = white kingside, bit 1 = white queenside, bit 2 = black kingside, bit 3 = black queenside)
// 4: turn (1 = white, 0 = black)
// 5-9: analysis depth 
// 10-13:enPassant file (0-8, 8 = none, 0 = H, 1 = G, ... 7 = A)
// 14-19: White king position (0-63)
// 20-25: Black king position (0-63)
// 26-27: Check status (00 = normal, 01 = check, 10 = checkmate, 11 = draw)

constexpr GameState turnMask = (1UL << 4);
constexpr GameState castlingRights = (15UL << 0);
constexpr GameState depth = (31UL << 5);
constexpr GameState enPassantFileMask = (15UL << 10);
constexpr GameState whiteKing = (63UL << 14);
constexpr GameState blackKing = (63UL << 20);
constexpr GameState checkStatus = (3UL << 26);


enum CastlingRights {
    WhiteKingside = 1 << 0,  // 00000001
    WhiteQueenside = 1 << 1, // 00000010
    BlackKingside = 1 << 2,  // 00000100
    BlackQueenside = 1 << 3, // 00001000
};

enum CheckStatus {
    Normal = 4, // 00010000
    Check = 1 << 5, // 00100000
    Checkmate = 1 << 6, // 01000000
    Draw = 1 << 5 | 1 << 6, // 01100000
};

inline bool isWhiteKingsideCastlingAllowed(GameState gameState) {
    return gameState & WhiteKingside;
}
inline bool isWhiteQueensideCastlingAllowed(GameState gameState) {
	return gameState & WhiteQueenside;
}
inline bool isBlackKingsideCastlingAllowed(GameState gameState) {
	return gameState & BlackKingside;
}
inline bool isBlackQueensideCastlingAllowed(GameState gameState) {
	return gameState & BlackQueenside;
}
inline bool isCheck(GameState gameState) {
	return gameState & Check;
}
inline bool isCheckmate(GameState gameState) {
	return gameState & Checkmate;
}
inline bool isDraw(GameState gameState) {
	return gameState & Draw;
}


// Gamestate evaluation functions
inline int turn(const GameState& gameState) {
    return (gameState & turnMask) >> 4;
}


void initGameState(GameState& gameState, JSgameState jsState); 


#endif