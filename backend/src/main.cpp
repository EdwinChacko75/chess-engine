#include "crow.h"
#include "crow/middlewares/cors.h"
#include <vector>
#include <string>
#include <thread>
#include "../include/bitboard.h"
#include "../include/move_generation.h"
#include "../include/gamestate.h"
#include "../include/evaluate.h"
#include "../include/move_pick.h"
#include "../include/transposition_table.h"




#include <chrono>

#include <iostream>// only in dev.

// #include your chess logic header files here

int main()
{
    // Initialize the transposition table
    initializeTable(64);


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
        //printChessBoard(white, black);

        int nodesVisitied = 0;
        int prunedBranches = 0;
        int zorbistedNodes = 0;
        int computedMove;

        Move bestMove;
        if ((gameState & turnMask) >> 4) {
            bestMove = searchBestMove(white, black, gameState, nodesVisitied, prunedBranches, zorbistedNodes);
        }
        else {
            bestMove = searchBestMove(black, white, gameState, nodesVisitied, prunedBranches, zorbistedNodes);
        }
        std::cout << "Best Move: " << (bestMove & sourceMask) << " to " << ((bestMove & destinationMask)>>6) << "; " << bestMove << std::endl;

        
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
        auto time = duration.count();
        std::cout << "Time taken: "
            << duration.count() << " milliseconds" << std::endl;
        std::cout << "Nodes visited: " << nodesVisitied << std::endl;
        std::cout << "Pruned branches: " << prunedBranches << std::endl;
        std::cout << "Zorbisted nodes: " << zorbistedNodes << std::endl;
        if (time > 0) {
            std::cout << "Nodes Per Second: " << ((nodesVisitied / duration.count()) * 1000) << std::endl;
        }

       
        //std::cout << "is set: " << countBits(gameBoards.whitePawns) << std::endl;

        // Prepare the response
        crow::json::wvalue responseData;
        int startSq = bestMove & sourceMask;
        int endSq = (bestMove & destinationMask) >> 6;
        int startCol = (startSq ) % 8;
        int startRow = (63 - startSq) / 8;
        int endCol = (endSq ) % 8;
        int endRow =(63- endSq) / 8 ;
        
        std::vector<int> bestMoveStart = {startRow, 7-startCol };
        std::vector<int> bestMoveEnd = { endRow, 7-endCol  };
        std::string bestPieceType;
        int pieceType = (bestMove & pieceTypeMask) >> 19;
        switch (pieceType) {
            case 1: 
                bestPieceType = "Pawn";
                break;
            case 2:
                bestPieceType = "Knight";
                break;

            case 3:
                bestPieceType = "Bishop";
                break;
            case 4:
                bestPieceType = "Rook";
                break;
            case 5:
                bestPieceType = "Queen";
                break;
            case 6:
                bestPieceType = "King";
                break;
            default:
                bestPieceType = "Empty";
                break;
        }
        
        bool moveException = false;// move& castlingMask || move & enPassantMask ||
        responseData["bestMoveStart"] = bestMoveStart;
        responseData["bestMoveEnd"] = bestMoveEnd;
        responseData["exception"] = moveException;
        responseData["pieceType"] = bestPieceType;
        
        responseData["counter"] = nodesVisitied;
        responseData["pruned"] = prunedBranches;
        responseData["zorbisted"] = zorbistedNodes;
		responseData["time"] = duration.count();

        crow::response res(responseData);
        res.set_header("Content-Type", "application/json");
        
       /* for (int i = 0; i < 64; i++) {
            std::cout << "pawn on " << i << '\n';
            printBoard(FORWARD_PAWN_PUSHES[i]);

        }*/

        return res;
            });


    app.concurrency(1);//numThreads == 0 ? 2 : numThreads/2);
    const char* port = std::getenv("PORT");
    uint16_t port_number = port ? static_cast<uint16_t>(std::stoi(port)) : 8080;

    app.port(port_number).bindaddr("0.0.0.0").run();
    return 0;
}