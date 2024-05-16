import {ChessBoard, c} from "./chessBoard.js";
import {Piece, Pawn, Rook, Bishop, Queen, King, Knight, Empty} from "./piece.js";
import {Move} from "./move.js";
import {Game} from "./game.js";
import { getZobristHash } from "./zobristTable.js";

export class Player {
    constructor(name, color) {
        this.name = name;
        this.color = color;
        this.moves = [];
        this.capturedPieces = [];
        this.isInCheck = false;
        this.checkmate = false;
        this.stalemate = false;
        let i = color === 'white' ? 7 : 0;
        let j = Math.abs(i - 1);
        this.pieces = [
            new Rook(color, [i, 0]),
            new Knight(color, [i, 1]),
            new Bishop(color, [i, 2]),
            new Queen(color, [i, 3]),
            new King(color, [i, 4]),
            new Bishop(color, [i, 5]),
            new Knight(color, [i, 6]),
            new Rook(color, [i, 7]),
            new Pawn(color, [j, 0]),
            new Pawn(color, [j, 1]),
            new Pawn(color, [j, 2]),
            new Pawn(color, [j, 3]),
            new Pawn(color, [j, 4]),
            new Pawn(color, [j, 5]),
            new Pawn(color, [j, 6]),
            new Pawn(color, [j, 7])
        ];
    }
    
    static makeTestMove(chessBoard, move) {
        let start = move.start;
        let end = move.end;
        let board = chessBoard.board;
        let piece = board[start[0]][start[1]];
        let endPiece = board[end[0]][end[1]];
        let previousJustMoved = null;
        let exception = false;
        let previousFirstMove = false;

        if (piece.firstMove) {
            previousFirstMove = true;
            piece.firstMove = false;
            if (piece.type === "Pawn") {
                if (piece.justMoved){
                    piece.justMoved = false;
                } else {
                    piece.justMoved = true;
                    // if the pawn is on an enpassant row, update chessBoard.enPassant
                    if (piece.position[0] === 4 || piece.position[0] === 3) {
                        chessBoard.enPassant = chessBoard.rows[piece.position[1]];
                        chessBoard.enPassant = [piece.position[0] + 1, piece.position[1]];
                    } else {
                        chessBoard.enPassant = 9;
                    }
                }
            }
        }
        let i = 0;
        let j = 0;
        for (i=0; i < 8; i++) {
            for (j=0; j < 8 ; j++) {
                if (i === start[0] && j === start[1])
                    continue;
                if (board[i][j].justMoved) {
                    previousJustMoved = [i, j];
                }
                board[i][j].justMoved = false;
            }
        }
        if (board[end[0]][end[1]].type !== "Empty") {
            board[end[0]][end[1]].isCaptured = true;
        }
        board[start[0]][start[1]] = new Empty(start);
        board[end[0]][end[1]] = piece;
        piece.position = end;

        // move.exception is the final position, not a move object
        // if (move.exception) {
        //     exception = Player.makeTestMove(chessBoard, move.exception);
        // }

        chessBoard.updateTargetedSquares();

        return {endPiece, piece, previousJustMoved, exception, previousFirstMove};
    }
    static undoTestMove(chessBoard, move, oldBoardState) {
        let board = chessBoard.board;
        let start = move.start;
        let end = move.end;
        oldBoardState.piece.position = start;
        board[start[0]][start[1]] = oldBoardState.piece;
        board[end[0]][end[1]] = oldBoardState.endPiece;
        if (oldBoardState.endPiece.type !== "Empty") {
            oldBoardState.endPiece.isCaptured = false;
            oldBoardState.endPiece.getMoves(chessBoard);
        }
        if (oldBoardState.piece.type === "Pawn") {
            oldBoardState.piece.justMoved = oldBoardState.previousJustMoved;
        }

        if (oldBoardState.previousJustMoved !== null) {
            board[oldBoardState.previousJustMoved[0]][oldBoardState.previousJustMoved[1]].justMoved = true;
        }
        if (oldBoardState.previousFirstMove) {
            oldBoardState.piece.firstMove = true;
        }
        
        // if (oldBoardState.exception) {
        //     Player.undoTestMove(chessBoard, move.exception, oldBoardState.exception, oldBoardState.previousJustMoved);
        // }

        chessBoard.updateTargetedSquares();

    }
    decideMove(event, game) {
        event.stopPropagation();
        let clickedElement = event.target;
        let chessBoard = game.board;
        let board = game.board.board;
        let turn = game.turn;
        if (document.querySelector('.selected') === null) {
            return null;
        }
        let start = ChessBoard.coordsToIndices(document.querySelector('.selected').id);
        let end = clickedElement.id ? ChessBoard.coordsToIndices(clickedElement.id) : ChessBoard.coordsToIndices(clickedElement.parentNode.id);
        let moves = board[start[0]][start[1]].moves;
        let move = moves.find(move => move.end[0] == end[0] && move.end[1] == end[1]);

        return move;
    }  

    kingPosition(color) {
        let king = this.pieces.find(piece => piece.type === 'King');
        return king.position;
    }  
}

export class AI extends Player {
    constructor(name, color, difficulty) {
        super(name, color);
        this.difficulty = difficulty;    
        this.counter = 0; 
        this.pruned = 0;
        this.zorbisted = 0;
    }
    JSboardToCPPboard(board) {
        let cppBoard = [];
        for (let i = 0; i < 8; i++) {  
            
            for (let j = 0; j < 8; j++) {
                let piece = board[i][j];
                if (piece.type === "Empty") {
                    cppBoard.push(0);
                } else if (piece.type === "Pawn") {
                    cppBoard.push(piece.color === "white" ? 1 : -1);
                }
                else if (piece.type === "Knight") {
                    cppBoard.push(piece.color === "white" ? 2 : -2);
                }
                else if (piece.type === "Bishop") {
                    cppBoard.push(piece.color === "white" ? 3 : -3);
                }
                else if (piece.type === "Rook") {
                    cppBoard.push(piece.color === "white" ? 4 : -4);
                }
                else if (piece.type === "Queen") {
                    cppBoard.push(piece.color === "white" ? 5 : -5);
                }
                else if (piece.type === "King") {
                    cppBoard.push(piece.color === "white" ? 6 : -6);
                }
            }
        }
        return cppBoard;
    }
    // TT not storing all explored positions 
    async decideMove(game) {
        let cppResponse;
        let whiteKing = game.board.castling.whiteKingside ? 1 : 0;
        let whiteQueen = game.board.castling.whiteQueenside ? 1 : 0;
        let blackKing = game.board.castling.blackKingside ? 1 : 0;
        let blackQueen = game.board.castling.blackQueenside ? 1 : 0;
      
        let msg = {
          "board": this.JSboardToCPPboard(game.board.board),
          "turn": 0,
          "enPassantSquare": game.board.enPassant,
          "depth": this.difficulty,
          "counter": this.counter,
          "pruned": this.pruned,
          "zorbisted": this.zorbisted,
          "whiteKingsideCastling": whiteKing,
          "whiteQueensideCastling": whiteQueen,
          "blackKingsideCastling": blackKing,
          "blackQueensideCastling": blackQueen,
        };
            
        try {
          const response = await fetch("http://localhost:8080/", {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: JSON.stringify(msg),
          });
      
          if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
          }
      
          const data = await response.json();
          cppResponse = new Move(data.pieceType, data.bestMoveStart, data.bestMoveEnd, data.exception);
        } catch (error) {
          console.log('Fetch error:', error);
        }
      
        console.log(cppResponse);
        console.log(game.board.board)
        return cppResponse;
      }
    minimax(game, board, alpha, beta, maximizingPlayer, depth, previousBestMove = null) {
        this.counter++;
        let playerColor = maximizingPlayer ? game.turn.color : game.turn.color === 'white' ? 'black' : 'white';
        let hash = getZobristHash(board);
        let score = null;
    
        if (game.zobristCache.hasOwnProperty(hash)) {
            score = game.zobristCache[hash].score;
            if (game.zobristCache[hash].depth >= depth) {
                this.zorbisted++;
                return game.zobristCache[hash];
            }
        }
        else {
            score = this.evaluateBoard(board, playerColor, maximizingPlayer);
        }

        if (depth === 0 || score === 10000 || score === -10000) {
            return { score: score, move: null };
        }
       
        let bestMove = null;
        let bestScore = maximizingPlayer ? -Infinity : Infinity;

        let clonedPieces = board.pieces.map(Game.clonePiece).filter(piece => piece !== null);
        let testBoard = new ChessBoard(clonedPieces);
        let moves = Game.legalMoves(testBoard.pieces, playerColor);

        // Part of iterative deepening logic
        // if (previousBestMove !== null) {
        //     let previousBestMoveIndex = moves.findIndex(move => move.start[0] === previousBestMove.start[0] && move.start[1] === previousBestMove.start[1] && move.end[0] === previousBestMove.end[0] && move.end[1] === previousBestMove.end[1]);
        //     if (previousBestMoveIndex !== -1) {
        //         moves.splice(previousBestMoveIndex, 1);
        //         moves.unshift(previousBestMove);
        //     }
        // }

        // sorting moves to prune more branches
        // moves.sort((a, b) => {
        //     let aPiece = testBoard.board[a.start[0]][a.start[1]];
        //     let bPiece = testBoard.board[b.start[0]][b.start[1]];
        //     return -(aPiece.value - bPiece.value);
        // });


        for (let i = 0; i < moves.length; i++) {
            let oldBoardState = Player.makeTestMove(testBoard, moves[i]);
            // Still unknown if this line benefits or hurts performance
            // this.updateTestBoardPieces(testBoard);
            let evaluation = this.minimax(game, testBoard, alpha, beta, !maximizingPlayer, depth - 1);
            if (evaluation.move === null) {
                evaluation.move = moves[i];
            }
            if (maximizingPlayer) {
                if (evaluation.score > bestScore) {
                    bestScore = evaluation.score;
                    bestMove = moves[i];
                }
                alpha = Math.max(alpha, evaluation.score);
            } else {
                if (evaluation.score < bestScore) {
                    bestScore = evaluation.score;
                    bestMove = moves[i];
                }
                beta = Math.min(beta, evaluation.score);
            }
            // this may make it slower rather than faster. it needs to be tested before implementation
            // it stores every position evaluated, even when a best move isnt associated with it
            // let testBoardHash = getZobristHash(testBoard);
            // if (testBoardHash !== hash && (game.zobristCache[testBoardHash] === undefined || game.zobristCache[testBoardHash].move === null || game.zobristCache[testBoardHash].depth < depth)) {
            //     this.TTupdates++;
            //     // can use evaluation instead of null for speed but this needs to be tested before implementation
            //     game.zobristCache[testBoardHash] = { score: evaluation.score, move: null, depth: 0 };
            // }
            
            Player.undoTestMove(testBoard, moves[i], oldBoardState);
            if (beta <= alpha) {
                this.pruned++;
                break;
            }
        }

        if ((game.zobristCache[hash] === undefined || game.zobristCache[hash].move === null || game.zobristCache[hash].depth < depth) && bestMove !== null) {
            this.TTupdates++;
            game.zobristCache[hash] = { score: bestScore, move: bestMove , depth};
        }
        return { score: bestScore, move: bestMove };
    }
    
    evaluateBoard(testBoard, playerColor, maximizingPlayer) {
        let board = testBoard.board;
        let score = 0;
        let allPieces = testBoard.pieces;
        
        let kingPosition = Game.kingPosition(allPieces, playerColor);

        let king = board[kingPosition[0]][kingPosition[1]];
        
        if (Game.legalMoves(allPieces, playerColor).length === 0) {
            if(!king.isSafe(kingPosition, testBoard.whiteTargetedSquares, testBoard.blackTargetedSquares)) {
                return maximizingPlayer ? -10000 : 10000;
            }
            else {
                return 0;
            }
        }

        for (let row = 0; row < 8; row++) {
          for (let col = 0; col < 8; col++) {
            let piece = board[row][col];
            if (piece.type !== "Empty") {
              let value = piece.value;
              if (maximizingPlayer) {
                score += (piece.color === playerColor ? value : -value);
              } else {
                score += (piece.color === playerColor ? -value : value);
              }
            }
          }
        }
        
        return score;
    }

    updateTestBoardPieces(testBoard) {
        testBoard.pieces = [];
        let board = testBoard.board;
        for (let i = 0; i < 8; i++) {
            for (let j = 0; j < 8; j++) {
                let piece = board[i][j];
                if (piece.type !== "Empty") {
                    testBoard.pieces.push(Game.clonePiece(piece)); 
                }
            }
        }
    }
}
