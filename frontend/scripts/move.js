import {ChessBoard, c} from "./chessBoard.js";
import {Piece, Pawn, Rook, Bishop, Queen, King, Knight, Empty} from "./piece.js";
import {Game} from "./game.js";
import {Player, AI} from "./player.js";

// The move class is a data class that stores the start, 
// end, and piece type calues. The exception is 
// for castling and en passant. The type notes whether 
// or not the move can perform a capture (pawns cant capture forward).
export class Move {
    constructor(piece, start, end, exception) {
        this.piece = piece;
        this.start = start;
        this.end = end;
        this.exception = exception;
        this.type = 'Capture';
    }
}
