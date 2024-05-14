#include "crow.h"
#include "crow/middlewares/cors.h"
#include <vector>
#include <string>
#include <limits>
#include <thread>
#include "../include/bitboard.h"
#include "../include/move_generation.h"
#include "../include/gamestate.h"
#include "../include/evaluate.h"
#include "../include/move_pick.h"



#include <chrono>

#include <iostream>// only in dev.

// #include your chess logic header files here

int main()
{
    unsigned int numThreads = std::thread::hardware_concurrency();

    crow::App<crow::CORSHandler> app;
    auto& cors = app.get_middleware<crow::CORSHandler>();

    CROW_ROUTE(app, "/").methods("POST"_method)
        ([&](const crow::request& req) -> crow::response {

        auto receivedData = crow::json::load(req.body);
        if (!receivedData) {
            return crow::response(400, "Bad Request: Unable to parse JSON");
        }

        // Extract and convert JSON values to corresponding C++ types
        std::vector<int> boardCPP;
        if (receivedData["board"].t() == crow::json::type::List && receivedData["board"].size() == 64) {
            for (size_t i = 0; i < receivedData["board"].size(); ++i) {
                boardCPP.push_back(receivedData["board"][i].i());
            }
        }
        else {
            return crow::response(400, "Bad Request: Invalid board format");
        }
        JSgameState jsGameState;

        jsGameState.turn = receivedData["turn"].i();
        jsGameState.depth = receivedData["depth"].i();
        jsGameState.enPassant = receivedData["enPassantSquare"].i();
        
        jsGameState.whiteKingsideCastling = receivedData["whiteKingsideCastling"].i();
        jsGameState.whiteQueensideCastling = receivedData["whiteQueensideCastling"].i();
        jsGameState.blackKingsideCastling = receivedData["blackKingsideCastling"].i();
        jsGameState.blackQueensideCastling = receivedData["blackQueensideCastling"].i();
       
        int counter = receivedData.has("counter") ? receivedData["counter"].i() : -1;
        int pruned = receivedData.has("pruned") ? receivedData["pruned"].i() : -1;
        int zorbisted = receivedData.has("zorbisted") ? receivedData["zorbisted"].i() : -1;
         
        int sq = 0;

        // Chess AI logic
        PlayerBitboard white;
        PlayerBitboard black;
        white.isWhite = true;
        black.isWhite = false;


        GameState gameState = 0;

        arrayToBitboardConverter(boardCPP, white, black);

        initGameState(gameState, jsGameState);
        auto start = std::chrono::high_resolution_clock::now();

        int nodesVisitied = 0;
        int computedMove;
        if ((gameState & turnMask) >> 4 ) {
			computedMove = negamax(white, black, gameState, true, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 0, nodesVisitied);
        }
        else {
			computedMove = negamax(black, white, gameState, true, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), 0, nodesVisitied);
		}

        std::cout << "Best Move: " << (computedMove & sourceMask) << " to " << (computedMove & destinationMask) <<"; " << computedMove << std::endl;
        std::cout<< "highest rated move: " << computedMove << std::endl;
        //printChessBoard(white, black);
        ////makeTestMove(white, black, 3148467);
        ////makeTestMove(black, white, 3148467);

        //printChessBoard(white, black);
        //unMakeTestMove(white, black, 3148467);
        ////unMakeTestMove(black, white, 3148467);

        //printChessBoard(white, black);


        //printMoves(movesArray, 32);
        //pb();
        //printBoards(gameBoards);
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        std::cout << "Time taken: "
            << duration.count() << " milliseconds" << std::endl;
        std::cout << "Nodes visited: " << nodesVisitied << std::endl;

        std::vector<Bitboard> bestMove = { white.pawns, white.bishops, white.knights, white.rooks,
            white.queens, white.king, black.pawns, black.bishops, black.knights, black.rooks, 
            black.queens, black.king
        };
        //std::cout << "is set: " << countBits(gameBoards.whitePawns) << std::endl;

        // Prepare the response
        crow::json::wvalue responseData;
        responseData["bestMove"] = bestMove;
        responseData["counter"] = counter;
        responseData["pruned"] = pruned;
        responseData["zorbisted"] = zorbisted;

        crow::response res(responseData);
        res.set_header("Content-Type", "application/json");
        
       /* for (int i = 0; i < 64; i++) {
            std::cout << "pawn on " << i << '\n';
            printBoard(FORWARD_PAWN_PUSHES[i]);

        }*/


        return res;
            });

    app.concurrency(2);//numThreads == 0 ? 2 : numThreads/2);
    app.port(8080).run();
    return 0;
}