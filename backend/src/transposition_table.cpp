#include "../include/transposition_table.h"


constexpr uint64_t rand64(uint64_t state) {
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    return state * 2685821657736338717LL;
}

constexpr uint64_t initZobristPieceSquare(int piece, int square) {
    uint64_t state = 1070372 + (piece * SQUARE_NB + square) * 1024;
    return rand64(state);
}

constexpr uint64_t initZobristEnpassant(int file) {
    uint64_t state = 1070372 + (PIECE_NB * SQUARE_NB + file) * 1024;
    return rand64(state);
}

constexpr uint64_t initZobristCastling(int castlingRight) {
    uint64_t state = 1070372 + (PIECE_NB * SQUARE_NB + FILE_NB + castlingRight) * 1024;
    return rand64(state);
}

namespace Zobrist {
    constexpr std::array<std::array<uint64_t, SQUARE_NB>, PIECE_NB> initPieceSquare() {
        std::array<std::array<uint64_t, SQUARE_NB>, PIECE_NB> temp{};
        for (int pc = 0; pc < PIECE_NB; ++pc) {
            for (int sq = 0; sq < SQUARE_NB; ++sq) {
                temp[pc][sq] = initZobristPieceSquare(pc, sq);
            }
        }
        return temp;
    }

    constexpr std::array<std::array<uint64_t, SQUARE_NB>, PIECE_NB> pieceSquare = initPieceSquare();

    constexpr std::array<uint64_t, FILE_NB> initEnpassant() {
        std::array<uint64_t, FILE_NB> temp{};
        for (int f = 0; f < FILE_NB; ++f) {
            temp[f] = initZobristEnpassant(f);
        }
        return temp;
    }

    constexpr std::array<uint64_t, FILE_NB> enpassant = initEnpassant();

    constexpr std::array<uint64_t, CASTLING_RIGHT_NB> initCastling() {
        std::array<uint64_t, CASTLING_RIGHT_NB> temp{};
        for (int cr = 0; cr < CASTLING_RIGHT_NB; ++cr) {
            temp[cr] = initZobristCastling(cr);
        }
        return temp;
    }

    constexpr std::array<uint64_t, CASTLING_RIGHT_NB> castling = initCastling();

    constexpr uint64_t side = rand64(1070372 + (PIECE_NB * SQUARE_NB + FILE_NB + CASTLING_RIGHT_NB) * 1024);
    constexpr uint64_t noPawns = rand64(1070372 + (PIECE_NB * SQUARE_NB + FILE_NB + CASTLING_RIGHT_NB + 1) * 1024);
}


ZobristHash generateZorbistHash(const PlayerBitboard& allies, const PlayerBitboard& opponents, const GameState& gameState) {
    ZobristHash hash = 0;
    const PlayerBitboard* white = nullptr;
    const PlayerBitboard* black = nullptr;
    if (gameState & turnMask) {
        hash ^= Zobrist::side;
        white = &allies;
        black = &opponents;
    }
    else {
        white = &opponents;
        black = &allies;
    }
    for (int i = 0; i < 64; i++) {
        if (white->pawns & (1ULL << i)) {
			hash ^= Zobrist::pieceSquare[0][i];
		}
        else if (white->knights & (1ULL << i)) {
			hash ^= Zobrist::pieceSquare[1][i];
		}
        else if (white->bishops & (1ULL << i)) {
			hash ^= Zobrist::pieceSquare[2][i];
		}
        else if (white->rooks & (1ULL << i)) {
			hash ^= Zobrist::pieceSquare[3][i];
		}
        else if (white->queens & (1ULL << i)) {
			hash ^= Zobrist::pieceSquare[4][i];
		}
        else if (white->king & (1ULL << i)) {
			hash ^= Zobrist::pieceSquare[5][i];
		}
        else if (black->pawns & (1ULL << i)) {
			hash ^= Zobrist::pieceSquare[6][i];
		}
        else if (black->knights & (1ULL << i)) {
			hash ^= Zobrist::pieceSquare[7][i];
		}
        else if (black->bishops & (1ULL << i)) {
			hash ^= Zobrist::pieceSquare[8][i];
		}
        else if (black->rooks & (1ULL << i)) {
			hash ^= Zobrist::pieceSquare[9][i];
		}
        else if (black->queens & (1ULL << i)) {
			hash ^= Zobrist::pieceSquare[10][i];
		}
        else if (black->king & (1ULL << i)) {
			hash ^= Zobrist::pieceSquare[11][i];
		}   
    }

    if (gameState & enPassantSquareMask) {
		hash ^= Zobrist::enpassant[((gameState & enPassantSquareMask) >> 10) % 8];
	}

    if (gameState & whiteKingsideCastleMask) {
        hash ^= Zobrist::castling[0];
    }
    if (gameState & whiteQueensideCastleMask) {
		hash ^= Zobrist::castling[1];
	}
    if (gameState & blackKingsideCastleMask) {
		hash ^= Zobrist::castling[2];
	}
    if (gameState & blackQueensideCastleMask) {
		hash ^= Zobrist::castling[3];
	}

	return hash;
}

// use extern later
std::vector<TTEntry> table;
size_t tableSize;
size_t currentGeneration;

void startNewSearch() {
    ++currentGeneration;
}

void initializeTable(size_t sizeInMB) {
    tableSize = sizeInMB * 1048576 / sizeof(TTEntry);
    table.resize(tableSize);
    // clear table
    currentGeneration = 0;
    std::fill(table.begin(), table.end(), TTEntry{});
}

void storeEntry(ZobristHash key, int depth, int score, int bound, Move move) {
    size_t index = key & (tableSize - 1);  // Ensure this calculation doesn't exceed bounds
    if (index >= table.size()) {
        std::cerr << "Index out of range error: " << index << " >= " << table.size() << std::endl;
        return; // Or handle the error appropriately
    }
    TTEntry& entry = table[index];

    if (entry.hash != key || currentGeneration - entry.generation >= 4 || entry.depth <= depth) {
        entry.hash = key;
        entry.depth = depth;
        entry.score = score;
        entry.bound = bound;
        entry.bestMove = move;
        entry.generation = currentGeneration;
    }
}

bool probeEntry(ZobristHash key, TTEntry*& entry, const int depth) {
    size_t index = key & (tableSize - 1);  
    
    const TTEntry& retrivedEntry = table[index];

    if (retrivedEntry.hash == key && currentGeneration - retrivedEntry.generation <= 4 && retrivedEntry.depth >= depth) {
        entry = &table[index];
        return true;
    }

    return false;
}

