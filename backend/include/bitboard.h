#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <cstdint>
#include <iostream> // Only for developments
#include <bitset>
//#include "./gamestate.h"
//Define constants for squares

using Bitboard = uint64_t;


constexpr Bitboard RANKS[8] = {
	0xFFULL, 0xFF00ULL, 0xFF0000ULL, 0xFF000000ULL, 0xFF00000000ULL, 0xFF0000000000ULL, 0xFF000000000000ULL, 0xFF00000000000000ULL
};
constexpr Bitboard FILES[8] = {
    0x8080808080808080ULL, 0x4040404040404040ULL, 0x2020202020202020ULL,0x1010101010101010ULL,0x808080808080808ULL, 0x404040404040404ULL, 0x202020202020202ULL, 0x101010101010101ULL
};

constexpr Bitboard HFile = 0x8080808080808080ULL;
constexpr Bitboard AFile = 0x101010101010101ULL;

constexpr Bitboard FirstRank = 0x0000000000000FFULL;
constexpr Bitboard EighthRank = 0xFF00000000000000ULL;



#ifdef _MSC_VER
// Microsoft Visual C++ compiler
#include <intrin.h>
inline int lsb(Bitboard bitboard) {
    if (bitboard == 0)
        return -1;
    unsigned long index;
    _BitScanForward64(&index, bitboard);
    return static_cast<int>(index);
}
inline int msb(Bitboard bitboard) {
    if (bitboard == 0)
        return -1;
    unsigned long index;
    _BitScanReverse64(&index, bitboard);
    return static_cast<int>(index);
}
inline int countBits(Bitboard bitboard) {
    return static_cast<int>(__popcnt64(bitboard));
}
#else
// Fallback implementation
inline int lsb(Bitboard bitboard) {
    if (bitboard == 0)
        return -1;
    int i = 0;
    while (0 == (1ULL << i & bitboard)) {
        i++;
    }
    return i;
}
inline int msb(Bitboard bitboard) {
    if (bitboard == 0)
        return -1;
    int i = 63;
    while (0 == (1ULL << i & bitboard)) {
        i--;
    }
    return i;
}
inline int countBits(Bitboard bitboard) {
    int count = 0;
    while (bitboard) {
        count += bitboard & 1;
        bitboard >>= 1;
    //}
    return count;
}
#endif

struct PlayerBitboard {
    Bitboard pawns = 0; 
    Bitboard bishops = 0;
    Bitboard knights = 0;
    Bitboard rooks = 0;
    Bitboard queens = 0;
    Bitboard king = 0;
    Bitboard friendlyPieces = 0;
    Bitboard allPieces = 0;

    int kingPosition;
    
    void updatePlayerBitboard(PlayerBitboard& opp) {
		friendlyPieces = pawns | bishops | knights | rooks | queens | king;
        kingPosition = msb(king);
        opp.friendlyPieces = opp.pawns | opp.bishops | opp.knights | opp.rooks | opp.queens | opp.king;
        opp.kingPosition = msb(opp.king);
        allPieces = friendlyPieces | opp.friendlyPieces;
        opp.allPieces = allPieces;
	}

};

inline Bitboard setBit(Bitboard bitboard, int sq) {
    return bitboard | (1ULL << sq);
}

inline Bitboard clearBit(Bitboard bitboard, int sq) {
    return bitboard & ~(1ULL << sq);
}

inline bool isSet(const Bitboard bitboard, int sq) {
    return (bitboard & (1ULL << sq)) != 0;
}


void arrayToBitboardConverter(const std::vector<int> &chessboard, PlayerBitboard& white, PlayerBitboard& black);

inline int pieceTypeOnSq(const PlayerBitboard& allies, const PlayerBitboard& opponents, int sq) {
    Bitboard square = 1ULL << sq;
    if (square & (allies.pawns | opponents.pawns) ) {
        return 1;
    }
    else if (square & (allies.knights | opponents.knights) ) {
        return 2;
    }
    else if (square & (allies.bishops | opponents.bishops) ) {
        return 3;
    }
    else if (square & (allies.rooks | opponents.rooks) ) {
        return 4;
    }
    else if (square & (allies.queens | opponents.queens) ) {
        return 5;
    }
    else if (square & (allies.king | opponents.king) ) {
        return 6;
    }
    return 0;
}
// Only for developments
void printBoards(const PlayerBitboard& white, const PlayerBitboard& black);
void printBoard(const Bitboard bitboard);
void printChessBoard(const PlayerBitboard& white, const PlayerBitboard& black);

#endif