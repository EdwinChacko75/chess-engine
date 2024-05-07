#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <cstdint>
#include <iostream> // Only for developments
#include <bitset>
#include "./gamestate.h"
//Define constants for squares

using Bitboard = uint64_t;

// With PIECE_MOVES[0] being H1 and PIECE_MOVES[63] being A8
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
    0x3020300000000ULL, 0x7050700000000ULL, 0xE0A0E00000000ULL, 0x1C141C00000000ULL, 0x38283800000000ULL, 0x70507000000000ULL, 0xE0A0E0000000ULL, 0xC040C000000000ULL,
    0x302030000000000ULL, 0x705070000000000ULL, 0xE0A0E0000000000ULL, 0x1C141C0000000000ULL, 0x3828380000000000ULL, 0x7050700000000000ULL, 0xE0A0E00000000000ULL, 0xC040C00000000000ULL,
    0x203000000000000ULL, 0x507000000000000ULL, 0x0A0E000000000000ULL, 0x141C000000000000ULL, 0x2838000000000000ULL, 0x5070000000000000ULL, 0xA0E0000000000000ULL, 0x40C0000000000000ULL
   };

inline void pb() {
    for (int i = 0; i <= 63; i++) {
        Bitboard cur = KNIGHT_MOVES[i];
        std::cout << i << '\n';
        for (int j = 0; j < 64; j++) {
            if (j % 8 == 0) std::cout << '\n';
            std::cout << (cur & 1ULL) << ' ';
            cur >>= 1;
        }
        std::cout << '\n';
    }

}constexpr Bitboard RANKS[8] = {
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

struct Bitboards {
    Bitboard whitePawns, whiteKnights, whiteBishops, whiteRooks, whiteQueens, whiteKing;
    Bitboard blackPawns, blackKnights, blackBishops, blackRooks, blackQueens, blackKing;
    Bitboard whitePieces, blackPieces, allPieces;
    int whiteKingSquare, blackKingSquare;

    Bitboards() : whitePawns(0), whiteKnights(0), whiteBishops(0),
        whiteRooks(0), whiteQueens(0), whiteKing(0),
        blackPawns(0), blackKnights(0), blackBishops(0),
        blackRooks(0), blackQueens(0), blackKing(0),
        whitePieces(0), blackPieces(0), allPieces(0),
        whiteKingSquare(0), blackKingSquare(0) {
        updateAggregateBitboards();
        updateKingSquares();
    }

    void updateAggregateBitboards() {
        whitePieces = whitePawns | whiteKnights | whiteBishops | whiteRooks | whiteQueens | whiteKing;
        blackPieces = blackPawns | blackKnights | blackBishops | blackRooks | blackQueens | blackKing;
        allPieces = whitePieces | blackPieces;
    }
    void updateKingSquares() {
        whiteKingSquare = lsb(whiteKing);
        blackKingSquare = msb(blackKing);
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


void arrayToBitboardConverter(const std::vector<int> &chessboard, Bitboards& bitboards, GameState& gameState);

inline int pieceTypeOnSq(const Bitboards& bitboards, int sq) {
    Bitboard square = 1ULL << sq;
    if (square & (bitboards.whitePawns | bitboards.blackPawns) ) {
        return 1;
    }
    else if (square & (bitboards.whiteKnights | bitboards.blackKnights) ) {
        return 2;
    }
    else if (square & (bitboards.whiteBishops | bitboards.blackBishops) ) {
        return 3;
    }
    else if (square & (bitboards.whiteRooks | bitboards.blackRooks) ) {
        return 4;
    }
    else if (square & (bitboards.whiteQueens | bitboards.blackQueens) ) {
        return 5;
    }
    else if (square & (bitboards.whiteKing | bitboards.blackKing) ) {
        return 6;
    }
    return 0;
}
// Only for developments
void printBoards(const Bitboards& bitboards);
void printBoard(const Bitboard bitboard);


#endif