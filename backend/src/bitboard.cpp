#include "../include/bitboard.h"

void arrayToBitboardConverter(const std::vector<int>& chessboard, PlayerBitboard& white, PlayerBitboard& black) {


    for (size_t i = 0; i < 64; i++) {
        Bitboard square = (1ULL << (63 - i));

        switch (chessboard[i])
        {
            case 1:  white.pawns |= square; break;
            case 2:  white.knights |= square; break;
            case 3:  white.bishops |= square; break;
            case 4:  white.rooks |= square; break;
            case 5:  white.queens |= square; break;
            case 6:  
                white.king |= square; 

                break;
            case -1: black.pawns |= square; break;
            case -2: black.knights |= square; break;
            case -3: black.bishops |= square; break;
            case -4: black.rooks |= square; break;
            case -5: black.queens |= square; break;
            case -6: 
                black.king |= square; 

                break;
            default:
                break;
        }
    }
    white.updatePlayerBitboard(black);
}

void printBoards(const PlayerBitboard& white, const PlayerBitboard& black) {

    if (white.friendlyPieces & black.friendlyPieces) {
		std::cout << "Error: Overlapping pieces" << std::endl;
	}
    if(white.pawns) {
        std::cout << "White Pawns" << std::endl;
        printBoard(white.pawns);
    }
    if (white.knights) {
		std::cout << "White Knights" << std::endl;
		printBoard(white.knights);
	}
    if (white.bishops) {
        std::cout << "White Bishops" << std::endl;
        printBoard(white.bishops);
    }
    if (white.rooks) {
		std::cout << "White Rooks" << std::endl;
		printBoard(white.rooks);
	}
    if (white.queens) {
		std::cout << "White Queens" << std::endl;
		printBoard(white.queens);
	}
    if (white.king) {
        std::cout << "White King" << std::endl;
        printBoard(white.king);
    }
    if (black.pawns) {
		std::cout << "Black Pawns" << std::endl;
		printBoard(black.pawns);
	}
    if (black.knights) {
        std::cout << "Black Knights" << std::endl;
        printBoard(black.knights);
    }
    if (black.bishops) {
        std::cout << "Black Bishops" << std::endl;
        printBoard(black.bishops);
    }
    if (black.rooks) {
        std::cout << "Black Rooks" << std::endl;
        printBoard(black.rooks);
    }
    if (black.queens) {
		std::cout << "Black Queens" << std::endl;
		printBoard(black.queens);
	}
    if (black.king) {
		std::cout << "Black King" << std::endl;
		printBoard(black.king);
	}
  
}
void printChessBoard(const PlayerBitboard& white, const PlayerBitboard& black) {
	Bitboard allPieces = white.allPieces | black.allPieces;
    for (int row = 0; row < 8; row++) {
        for (int col = 7; col >= 0; col--) {
			int sq = 8 * (7 - row) + col;
            int whitePawn = isSet(white.pawns, sq);
            int whiteKnight = isSet(white.knights, sq);
            int whiteBishop = isSet(white.bishops, sq);
            int whiteRook = isSet(white.rooks, sq);
            int whiteQueen = isSet(white.queens, sq);
            int whiteKing = isSet(white.king, sq);
            int blackPawn = isSet(black.pawns, sq);
            int blackKnight = isSet(black.knights, sq);
            int blackBishop = isSet(black.bishops, sq);
            int blackRook = isSet(black.rooks, sq);
            int blackQueen = isSet(black.queens, sq);
            int blackKing = isSet(black.king, sq);
           
            if (isSet(white.pawns, sq)) {
				std::cout << "P";
			}
            if (isSet(white.knights, sq)) {
				std::cout << "N";
			}
            if (isSet(white.bishops, sq)) {
				std::cout << "B";
			}
            if (isSet(white.rooks, sq)) {
				std::cout << "R";
			}
            if (isSet(white.queens, sq)) {
				std::cout << "Q";
			}
            if (isSet(white.king, sq)) {
				std::cout << "K";
			}
            if (isSet(black.pawns, sq)) {
				std::cout << "p";
			}
            if (isSet(black.knights, sq)) {
				std::cout << "n";
			}
            if (isSet(black.bishops, sq)) {
				std::cout << "b";
			}
            if (isSet(black.rooks, sq)) {
				std::cout << "r";
			}
            if (isSet(black.queens, sq)) {
				std::cout << "q";
			}
            if (isSet(black.king, sq)) {
				std::cout << "k";
			}
            if (!(whitePawn || whiteKnight || whiteBishop || whiteRook || whiteQueen || whiteKing || blackPawn || blackKnight || blackBishop || blackRook || blackQueen || blackKing)) {
                std::cout << ".";

            }
          
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}
void printBoard(const Bitboard bitboard) {
	std::bitset<64> b(bitboard);
    for (int row = 0; row < 8; row++) {
        for (int col = 7; col >= 0; col--) {
			std::cout << b[8 * (7 - row) + col];
		}
		std::cout << '\n';
	}
    std::cout << '\n';

}


