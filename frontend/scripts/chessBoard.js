import {Piece, Pawn, Rook, Bishop, Queen, King, Knight, Empty} from "./piece.js";
import {Move} from "./move.js";
import {Game} from "./game.js";
import {Player, AI} from "./player.js";
import { getZobristHash,zobristTable, pieceMapping, castlingMapping, enPassantMapping, turnMapping } from './zobristTable.js';

export let c = 1;
export class ChessBoard {
    constructor(initialPieces) {
        this.color = "white";
        this.opponent = 'black';
        this.turn = 0;
        this.board = [];
        this.whiteTargetedSquares = [];
        this.blackTargetedSquares = [];
        this.pieces = initialPieces;
        this.zobristTable = zobristTable;
        this.pieceMapping = pieceMapping;
        this.castlingMapping = castlingMapping;
        this.enPassantMapping = enPassantMapping;
        this.turnMapping = turnMapping;        
        this.enPassant = 9;
        this.columns = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];
        this.castling = {
            whiteKingside: false,
            whiteQueenside: false,
            blackKingside: false,
            blackQueenside: false
        };
        

        for (let i = 0; i < 8; i++) {
            let row = [];
            for (let j = 0; j < 8; j++) {
                row.push(new Empty([i, j]));
            }
            this.board.push(row);
        }
        
        initialPieces.forEach(piece => {

            this.board[piece.position[0]][piece.position[1]] = piece;
        });

        if (this.color == 'black') {
            this.board.reverse();
        }

        this.updateTargetedSquares();

    }
   
    clickEvent(event) {
        event.stopPropagation();
        let clickedElement = event.target;
        let chessBoard = this.board;
        let board = this.board.board;
        let turn = this.turn;
        turn.moves = Game.legalMoves(Game.allPieces(this), turn.color);
        
        let indices = ChessBoard.coordsToIndices(clickedElement.parentNode.id);


        if (indices.length === 2 && board[indices[0]][indices[1]].color !== turn.color && !clickedElement.parentNode.classList.contains('displayedMoves')) {
            chessBoard.removeAllHighlights();
        }
        var squares = document.querySelectorAll('.square');
        squares.forEach(square => {
            if (square.classList.contains('highlighted')) {
                square.classList.remove('highlighted');
            }
        });
        if (clickedElement.tagName == 'LI' && clickedElement.classList.length == 2) {
            chessBoard.removeAllHighlights();
            squares.forEach(square => {
                if (square.classList.contains('selected')) {
                    square.classList.remove('selected');
                }
            });
        }
        else if (clickedElement.tagName === 'IMG' && !clickedElement.parentNode.classList.contains('displayedMoves')) {
            clickedElement = clickedElement.parentNode;
            var squares = document.querySelectorAll('.square');
            if (!clickedElement.classList.contains('selected')) {
                
                squares.forEach(square => {
                    if (square.classList.contains('selected')) {
                        square.classList.remove('selected');
                    }
                });
                clickedElement.classList.add('selected');
                
                let position = ChessBoard.coordsToIndices(clickedElement.id);
                let piece = board[position[0]][position[1]];
                if (piece.color !== turn.color) {
                    return;
                }

                let moves = piece.moves;

                let unSafeMoves = [];
                
                if (piece.type === "King") {
                    let opponentMoves = piece.color === 'white' ? chessBoard.blackTargetedSquares : chessBoard.whiteTargetedSquares;
                    piece.moves.forEach(testMove => {
                        opponentMoves.forEach(move => {
                            if (move.end[0] === testMove.end[0] && move.end[1] === testMove.end[1] && move.type === 'Capture') {
                                unSafeMoves.push(move.end);
                            }
                        });
                    });
                    let filteredMoves = moves.filter(move => 
                        !unSafeMoves.some(arr => 
                            arr[0] === move.end[0] && arr[1] === move.end[1]
                        )
                    );
                    moves = filteredMoves;
                }
                chessBoard.removeAllHighlights();
                moves.forEach(move => {
                    let square = document.getElementById(ChessBoard.indicesToCoords(move.end));
                    square.classList.add('displayedMoves');
                });
            }
            else if (clickedElement.classList.contains('selected')) {
                clickedElement.classList.remove('selected');
                chessBoard.removeAllHighlights();
            }   
        }
    }
    
    updateTargetedSquares() {
        this.whiteTargetedSquares = [];
        this.blackTargetedSquares = [];
        
        let kings = [];

        for (let row of this.board) { 
            for (let piece of row) {
                if (piece.type === "Empty") continue;
                if (piece.type === "King") {
                    kings.push(piece);
                    continue;
                }
                piece.getMoves(this); 
                if (piece.type === "King" && piece.color === "black") {
                }
                //CHANGED EQUALS TO NOT FULL EQUALS
                if (piece.targets == [] || piece.targets === undefined) continue;
                for (let move of piece.targets) {
                    if (piece.color === "black") {
                        this.blackTargetedSquares.push(move);
                    } 
                    else if (piece.color === "white"){
                        this.whiteTargetedSquares.push(move);
                    }
                }
            }
        }
        kings.forEach(king => {
            king.getMoves(this);
            //SAME EQUALS TO NOT FULL EQUALS
            if (king.targets == [] || king.targets === undefined) return;
            for (let move of king.targets) {
                if (king.color === "black") {
                    this.blackTargetedSquares.push(move);
                } 
                else if (king.color === "white"){
                    this.whiteTargetedSquares.push(move);
                }
            }
        });
    }

    static removeBoard() {
        document.getElementById("board-container").childNodes.forEach(child => {
            if (child.id === "board") {
                child.remove();
            }
        });
    }

    static indicesToCoords(position) {
        const columns = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];
        const rows = ['8', '7', '6', '5', '4', '3', '2', '1'];
        return columns[position[1]] + rows[position[0]];
    }
    static coordsToIndices(coord) {
        const columns = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];
        const rows = ['8', '7', '6', '5', '4', '3', '2', '1'];
        let res = [];
        for (let i = 0; i < 8; i++) {
            if (rows[i] == coord[1]) {
                res.push(i);
            }
        }
        for (let i = 0; i < 8; i++) {
            if (columns[i] == coord[0]) {
                res.push(i);
            }
        }
        return res;
    }
    
    removeHighlights() {
        var highlighted = document.querySelectorAll('.highlighted');
        highlighted.forEach(highlight => {
            if (highlight.classList.contains('highlighted')) {
                highlight.classList.remove('highlighted');
            }
            highlight.classList.remove('highlighted');
        });
    }

    removeMoveHighlights() {
        var newHighlights = document.querySelectorAll('.displayedMoves');
        newHighlights.forEach(newHighlight => {
            newHighlight.classList.remove('displayedMoves');
        });
    }

    removeAllHighlights() {
        this.removeHighlights();
        this.removeMoveHighlights();
    }

    flipBoard(color) {
        this.color = this.color == 'black' ? 'white' : "black";

        this.board.forEach(row => {
            row.forEach(piece => {
                piece.position = [7 - piece.position[0], 7 - piece.position[1]];
            });
        });
        this.board.forEach(row => {
            row.reverse(); 
        });
        this.board.reverse();

        let elements = document.querySelectorAll('.lastmove');
        c = -c; 
        this.createChessBoard(this.board, this.color);
        
        const boardElement = document.getElementById("board");
        boardElement.removeEventListener('click', ChessBoard.handleBoardClick.bind(this));
        boardElement.addEventListener('click', ChessBoard.handleBoardClick.bind(this));
        
    }
    
    createChessBoard(game) {
        let board = game.board.board;
        let color = game.color;
        ChessBoard.removeBoard();
        document.getElementById("board-container").innerHTML += '<ul id="board"></ul>';
        const columns = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'];
        const rows = ['8', '7', '6', '5', '4', '3', '2', '1'];

        if (color == "black") {
            columns.reverse();
            rows.reverse();
        }
        
        //Create the board
        for( let i = 0; i < 8; i++) {
            document.getElementById("board").innerHTML += '<div>';

            for (let j = 0; j < 8; j++) {
                var squareColor = (i + j) % 2 == 0 ? "light" : "dark" ;
                let squareHTML = '<li id="' + columns[j] + rows[i]  + '" class="square '+ squareColor +'">';

                if (board[i][j].type !== "Empty") { 
                    squareHTML += '<img class="pieces '+ board[i][j].type + '" src="../img/chesspieces/wikipedia/'+ board[i][j].color + board[i][j].type +'.png">';
                }
                if (i == 7 && j == 0){
                    squareHTML += '<p class="row ' + squareColor +'">' + rows[i] + '<p class="column">' + columns[j] + '</p>'
                }
                else if (i == 7){
                    squareHTML += '<p class="column"' + squareColor +'">' + columns[j] + '</p'
                }
                else if (j == 0){
                    squareHTML += '<p class="row"' + squareColor +'">' + rows[i] + '</p>'
                }

                squareHTML += '</li>';          
                document.getElementById("board").innerHTML += squareHTML;
            }
            document.getElementById("board").innerHTML += '</div>';
        }
        
        // Square Highlighting
        var squares = document.querySelectorAll('.square');
        squares.forEach(square => {
            square.addEventListener('contextmenu', function(event) {
                event.preventDefault();
                if (square.classList.contains('highlighted')) {
                    square.classList.remove('highlighted');
                }
                else {
                    square.classList.add('highlighted');
                    const isSelectedClassPresent = [...squares].some(square => square.classList.contains('selected'));
                    if (isSelectedClassPresent) {
                        squares.forEach(square => {
                            square.classList.remove('displayedMoves');
                            square.classList.remove('selected');
                        });
                    }
                }
            });
        });
        const boardElement = document.getElementById("board");
        boardElement.removeEventListener('click', this.clickEvent.bind(game));
        boardElement.addEventListener('click', this.clickEvent.bind(game));

    }
}
