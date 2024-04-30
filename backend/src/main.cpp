#include "crow.h"
#include "crow/middlewares/cors.h"
#include <vector>
#include <string>
#include <thread>
#include "../include/bitboard.h"
#include "../include/move_generation.h"
#include "../include/gamestate.h"

#include <iostream>// only in dev.

// #include your chess logic header files here

using namespace crow;

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
        jsGameState.enPassant = receivedData["enPassant"].i();
        
        jsGameState.whiteKingsideCastling = receivedData["whiteKingsideCastling"].i();
        jsGameState.whiteQueensideCastling = receivedData["whiteQueensideCastling"].i();
        jsGameState.blackKingsideCastling = receivedData["blackKingsideCastling"].i();
        jsGameState.blackQueensideCastling = receivedData["blackQueensideCastling"].i();
       
        int counter = receivedData.has("counter") ? receivedData["counter"].i() : -1;
        int pruned = receivedData.has("pruned") ? receivedData["pruned"].i() : -1;
        int zorbisted = receivedData.has("zorbisted") ? receivedData["zorbisted"].i() : -1;
         
        int sq = 0;

        // Chess AI logic
        Bitboards gameBoards;
        GameState gameState = 0;

        arrayToBitboardConverter(boardCPP, gameBoards, gameState);
        initGameState(gameState, jsGameState);

        Move* movesArray = generateMoves(gameBoards, gameState);
        //printMoves(movesArray, 32);
        //pb();
        delete[] movesArray;
        //printBoards(gameBoards);
        

        std::vector<Bitboard> bestMove = { gameBoards.whitePawns, gameBoards.whiteKnights, gameBoards.whiteBishops, gameBoards.whiteRooks, 
            gameBoards.whiteQueens, gameBoards.whiteKing, gameBoards.blackPawns, gameBoards.blackKnights, 
            gameBoards.blackBishops, gameBoards.blackRooks, gameBoards.blackQueens, gameBoards.blackKing
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
        return res;
            });

    app.concurrency(2);//numThreads == 0 ? 2 : numThreads/2);
    app.port(8080).run();
    return 0;
}