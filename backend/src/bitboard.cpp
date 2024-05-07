#include "../include/bitboard.h"

void arrayToBitboardConverter(const std::vector<int>& chessboard, Bitboards& bitboards, GameState& gameState) {


    for (size_t i = 0; i < 64; i++) {
        Bitboard square = (1ULL << (63 - i));

        switch (chessboard[i])
        {
            case 1: bitboards.whitePawns |= square; break;
            case 2:  bitboards.whiteKnights |= square; break;
            case 3:  bitboards.whiteBishops |= square; break;
            case 4:  bitboards.whiteRooks |= square; break;
            case 5:  bitboards.whiteQueens |= square; break;
            case 6:  
                bitboards.whiteKing |= square; 
                gameState |= (i) << 14;

                break;
            case -1: bitboards.blackPawns |= square; break;
            case -2: bitboards.blackKnights |= square; break;
            case -3: bitboards.blackBishops |= square; break;
            case -4: bitboards.blackRooks |= square; break;
            case -5: bitboards.blackQueens |= square; break;
            case -6: 
                bitboards.blackKing |= square; 
                gameState |= (i) << 20;


                break;
            default:
                break;
        }
    }
    bitboards.updateAggregateBitboards();
    bitboards.updateKingSquares();
}

void printBoards(const Bitboards& bitboards) {


    std:: cout << "White Pawns" << std::endl;
    printBoard(bitboards.whitePawns);
    std:: cout << "White Knights" << std::endl;
    printBoard(bitboards.whiteKnights);
    std:: cout << "White Bishops" << std::endl;
    printBoard(bitboards.whiteBishops);
    std:: cout << "White Rooks" << std::endl;
    printBoard(bitboards.whiteRooks);
    std:: cout << "White Queens" << std::endl;
    printBoard(bitboards.whiteQueens);
    std:: cout << "White King" << std::endl;
    printBoard(bitboards.whiteKing);
    std:: cout << "Black Pawns" << std::endl;
    printBoard(bitboards.blackPawns);
    std:: cout << "Black Knights" << std::endl;
    printBoard(bitboards.blackKnights);
    std:: cout << "Black Bishops" << std::endl;
    printBoard(bitboards.blackBishops);
    std:: cout << "Black Rooks" << std::endl;
    printBoard(bitboards.blackRooks);
    std:: cout << "Black Queens" << std::endl;
    printBoard(bitboards.blackQueens);
    std:: cout << "Black King" << std::endl;
    printBoard(bitboards.blackKing);
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


