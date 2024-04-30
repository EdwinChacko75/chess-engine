import {ChessBoard, c} from "./chessBoard.js";
import {Move} from "./move.js";
import {Game} from "./game.js";
import {Player, AI} from "./player.js";

export class Piece {
    constructor(type, color, position) {
        this.type = type; // the type of piece
        this.color = color; // the color of the piece
        this.position = position; // position on board in indices [row, col]
        this.firstMove = true; // is it the first time this piece is moving 
        this.isCaptured = false; // is this piece captured
        this.moves = []; // the moves, and later, legal moves
        this.targets = []; // squares this piece controls
        this.promoted = false; // is this piece promoted from a pawn
    }
    // Create a piece given piece type, color, position
    static createPiece(type, color, position) {
        switch (type) {
            case 'Pawn':
                return new Pawn(color, position);
            case 'Rook':
                return new Rook(color, position);
            case 'Knight':
                return new Knight(color, position);
            case 'Bishop':
                return new Bishop(color, position);
            case 'Queen':
                return new Queen(color, position);
            case 'King':
                return new King(color, position);
            case 'Empty':
                return new Empty(position);
            default:
                throw new Error('Invalid piece type');
        }
    }
    // Gets all the potential moves of a piece given its movement vectors
    getMovesFromDirection(chessBoard, directions) {
        let moves = [];
        this.targets = [];
        let row = this.position[0];
        let col = this.position[1];
        let board = chessBoard.board;

        let piece = board[row][col];
        let start = [row, col];
        
        for(let dir of directions) {               
            let i = 1;
            do {
                let newRow = row + i * dir[0];
                let newCol = col + i * dir[1];
                
                if (newRow < 0 || newRow >= 8 || newCol < 0 || newCol >= 8) {
                    break;
                } else if (board[newRow][newCol].type !== "Empty") {
                    let move = new Move(piece.type, start, [newRow, newCol], false);
                    if (board[newRow][newCol].color !== this.color){
                        moves.push(move);
                        this.targets.push(move);

                    } 
                    else if (board[newRow][newCol].color === this.color){
                        this.targets.push(move);
                    }
                    
                    break;
                }
                let move = new Move(piece.type, start, [newRow, newCol], false);
                moves.push(move);
                this.targets.push(move);

                i++;
            } 
            while (true && this.type !== "Knight" && this.type !== "King");
        }
        return moves;
    }
}

export class Pawn extends Piece {
    constructor(color, position) {
        super('Pawn', color, position);
        this.justMoved = false;
        this.value = 100;
    }
    getMoves(chessBoard) {
        this.targets = [];
        let moves = [];
        let row = this.position[0];
        let col = this.position[1];
        let board = chessBoard.board;
        let piece = board[row][col];
        let start = [row, col];
        let direction = this.color == 'white' ? -c : c; // c is 1 or -1 declared at the top of the file

        
        let nextRow = row + direction;
        let doubleRow = row + 2 * direction;
        let leftCol = col - 1;
        let rightCol = col + 1;

        let forward = (nextRow > -1 && nextRow < 8) ? board[nextRow][col].position : null;
        let doubleStep = (doubleRow > -1 && doubleRow < 8) ? board[row + 2 * direction][col].position : null;
        let leftCapture = (nextRow >= 0 && nextRow < 8 && leftCol >= 0) ? board[nextRow][leftCol].position : null;
        let rightCapture = (nextRow >= 0 && nextRow < 8 && rightCol < 8) ? board[nextRow][rightCol].position : null;

        
        if (forward && board[forward[0]][forward[1]].type == 'Empty'){
            moves.push(new Move(piece.type, start,[nextRow, col], false))

            if (doubleStep && board[doubleStep[0]][doubleStep[1]].type == 'Empty' && this.firstMove) {
                moves.push(new Move(piece.type, start, [doubleRow, col], false));
            }
        }
        if (leftCapture) {
            let move = new Move(piece.type, start,[nextRow, leftCol], false);
            if (board[leftCapture[0]][leftCapture[1]].color != this.color && board[leftCapture[0]][leftCapture[1]].type !== 'Empty') {
                let move = new Move(piece.type, start,[nextRow, leftCol], false);
                moves.push(move);
                this.targets.push(move);
            }
            else {
                this.targets.push(move);
            }
        }
        if (rightCapture) {
            let move = new Move(piece.type, start,[nextRow, rightCol], false);
            if (board[rightCapture[0]][rightCapture[1]].color != this.color && board[rightCapture[0]][rightCapture[1]].type !== 'Empty') {
                let move = new Move(piece.type, start,[nextRow, rightCol], false);
                moves.push(move);
                this.targets.push(move);
            }
            else {
                this.targets.push(move);
            }
        }
        let enPassantRow = this.color == 'black' ? 4 : 3;
        let enPassantMoveRow = this.color == 'black' ? 5 : 2;

        if (row == enPassantRow) {
            let leftEnPassant = board[enPassantRow][leftCol];
            let rightEnPassant = board[enPassantRow][rightCol];

            if (leftEnPassant && leftEnPassant.type == 'Pawn' && leftEnPassant.color !== this.color && leftEnPassant.justMoved) {
                moves.push(new Move(piece.type, start, [enPassantMoveRow, leftCol], leftEnPassant.position));
            }

            if (rightEnPassant && rightEnPassant.type == 'Pawn' && rightEnPassant.color !== this.color && rightEnPassant.justMoved) {
                moves.push(new Move(piece.type, start, [enPassantMoveRow, rightCol], rightEnPassant.position));
            }
        }
       
        this.moves =  moves;
    }

    promote(promoteToThis, game) {
        let newPiece;
        let player = this.color === 'white' ? game.whitePlayer : game.blackPlayer;
        switch (promoteToThis) {
            case 'Queen':
                newPiece = new Queen(this.color, this.position);
                newPiece.firstMove = false;
                break;
            case 'Rook':
                newPiece = new Rook(this.color, this.position);
                newPiece.firstMove = false;
                break;
            case 'Bishop':
                newPiece = new Bishop(this.color, this.position);
                newPiece.firstMove = false;
                break;
            case 'Knight':
                newPiece = new Knight(this.color, this.position);
                newPiece.firstMove = false;
                break;        
            default:
                newPiece = new Queen(this.color, this.position);
                newPiece.firstMove = false;
                break;
        }
        player.pieces.push(newPiece);
        newPiece.firstMove = false;
        newPiece.promoted = true;
        newPiece.isCaptured = false;

        return newPiece;
    }
}
export class Rook extends Piece {
    constructor(color, position) {
        super('Rook', color, position);
        this.directions = [[0, 1], [0, -1], [1, 0], [-1, 0]];
        this.value = 525;
    }
    getMoves(chessBoard) {
        this.moves = this.getMovesFromDirection(chessBoard, this.directions);
    }

}
export class Bishop extends Piece {
    constructor(color, position) {
        super('Bishop', color, position);
        this.directions = [[1, 1], [1, -1], [-1, 1], [-1, -1]];
        this.value = 350;
    }
    getMoves(chessBoard) {        
        this.moves = this.getMovesFromDirection(chessBoard, this.directions);
    }
}
export class Knight extends Piece {
    constructor(color, position) {
        super('Knight', color, position);
        this.directions = [[2, 1], [1, 2], [-2, 1], [1, -2], [2, -1], [-2, -1], [-1, -2], [-1, 2]];
        this.value = 350;
    }
    getMoves(chessBoard) {
        this.moves = this.getMovesFromDirection(chessBoard, this.directions);
    }
}
export class Queen extends Piece {
    constructor(color, position) {
        super('Queen', color, position);
        this.directions = [[1, 1], [1, -1], [-1, 1], [-1, -1],[0, 1], [0, -1], [1, 0], [-1, 0]];
        this.value = 1000;
    }
    getMoves(chessBoard) {
        this.moves = this.getMovesFromDirection(chessBoard, this.directions);  
    }
    
}

export class King extends Piece {
    constructor(color, position) {
        super('King', color, position);
        this.directions = [[1, 1], [1, -1], [-1, 1], [-1, -1],[0, 1], [0, -1], [1, 0], [-1, 0]];
        this.value = 10000;
    }
    getMoves(chessBoard) {
        chessBoard.castling = {
            whiteKingside: false,
            whiteQueenside: false,
            blackKingside: false,
            blackQueenside: false
        };
        let board = chessBoard.board;
        let moves = this.getMovesFromDirection(chessBoard, this.directions);

        let row = this.position[0];
        let col = this.position[1];
    
        let blackTargets = chessBoard.blackTargetedSquares;
        let whiteTargets = chessBoard.whiteTargetedSquares;

        if (this.firstMove && this.isSafe([row, col], whiteTargets, blackTargets)) {
            let isSafe = true;
            // short castle
            let i = 1;
            while ((board[row][col + i].type === 'King' || board[row][col + i].type === "Empty") && col + i < 7) {
                
                if (!this.isSafe([row, col + i], whiteTargets, blackTargets)) {
                    isSafe = false;
                    break;
                }

                i++;

            }
            if (isSafe && board[row][col + i].type === "Rook" && board[row][col + i].color === this.color && board[row][col + i].firstMove) {
                // check that intermediate squares are safe
                
                if (this.color === 'black') {
                    chessBoard.castling.blackKingside = true;
                } else {
                    chessBoard.castling.whiteKingside = true;
                }
                moves.push(new Move(this.type, [row,col], [row, col + i - Math.floor(i/2)], new Move(board[row][col + i].type, [row, col + i], [row, col + 1], false)));
            }
            // long castle
            let j = 1;
            isSafe = true;
            while ((board[row][col - j].type === 'King' || board[row][col - j].type === "Empty") && col - j > 0) {
                if (!this.isSafe([row, col - j], whiteTargets, blackTargets)) {
                    isSafe = false;
                    break;
                }
                j++;
            }        
            if (board[row][col - j].type === "Rook" && board[row][col - j].color === this.color && board[row][col - j].firstMove) {
                //check that intermediate squares are safe
                
                if (this.color === 'black') {
                    chessBoard.castling.blackQueenside = true;
                } else {
                    chessBoard.castling.whiteQueenside = true;
                }
                moves.push(new Move(this.type, [row,col], [row, col - j + Math.floor(j/2)], new Move(board[row][col - j].type, [row, col - j], [row, col - 1], false)));
            }
        }
        
        this.moves = moves;
    }
    isSafe(testMove, whiteTargets, blackTargets) {
        let opponentMoves;
        if (this.color === 'white') {
            opponentMoves = blackTargets;
        } else {
            opponentMoves = whiteTargets;
        }
        if (opponentMoves === undefined) {
            return true;
        }
        return !opponentMoves.some(move => 
            move.end[0] === testMove[0] && move.end[1] === testMove[1]
        );
    }

}
export class Empty extends Piece {
    constructor(position) {
        super('Empty', null, position);
        this.firstMove = false;
        this.value = 0;
    }
    getMoves(chessBoard) {
        return;
    }
}