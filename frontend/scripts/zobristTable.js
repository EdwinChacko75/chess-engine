// This file handles the creation of the zorbist hash table,
// retrieval of hashes for positions, and mapping of 
// item to 3d square indices

// Create the table
export const zobristTable = initializeZobristTable();

// Mapping piece indices
export const pieceMapping = {
    'whitePawn': 0,
    'whiteKnight': 1,
    'whiteBishop': 2,
    'whiteRook': 3,
    'whiteQueen': 4,
    'whiteKing': 5,
    'blackPawn': 6,
    'blackKnight': 7,
    'blackBishop': 8,
    'blackRook': 9,
    'blackQueen': 10,
    'blackKing': 11,
};

// Mapping castling indices
export const castlingMapping = {
    'whiteKingside': 0,
    'whiteQueenside': 1,
    'blackKingside': 2,
    'blackQueenside': 3
};

// Mapping En Passant indices
export const enPassantMapping = {
    'a': 0,
    'b': 1,
    'c': 2,
    'd': 3,
    'e': 4,
    'f': 5,
    'g': 6,
    'h': 7
};

// Mapping turn inidces
export const turnMapping = {
    0: 0,
    1: 1
};

// Functionn that returns the zorbist hash table
function initializeZobristTable() {
    // Create empty table 
    let table = {
        pieces: [],
        castling: [],
        enPassant: [],
        turn: [],   
    };
    
    // Creating hashes for white and black turn
    let whiteTurn = BigInt(Math.floor(Math.random() * Number.MAX_SAFE_INTEGER)) * BigInt(2)**BigInt(64);
    let blackTurn = BigInt(Math.floor(Math.random() * Number.MAX_SAFE_INTEGER)) * BigInt(2)**BigInt(64);

    // Adding hashes to table
    table.turn = [whiteTurn, blackTurn];
    
    // Creating 8x8 list in table, populated by hashes corresponding to the board
    for (let i = 0; i < 8; i++) {
        table.pieces.push([]);
        for (let j = 0; j < 8; j++) {
            table.pieces[i].push([]);
            for (let k = 0; k < 12; k++) {
                table.pieces[i][j].push(BigInt(Math.floor(Math.random() * Number.MAX_SAFE_INTEGER)) * BigInt(2)**BigInt(64));
            }
        }
    }
    
    // Creating and assigning castling hashes
    table.castling = [
        BigInt(Math.floor(Math.random() * Number.MAX_SAFE_INTEGER)) * BigInt(2)**BigInt(64),
        BigInt(Math.floor(Math.random() * Number.MAX_SAFE_INTEGER)) * BigInt(2)**BigInt(64),
        BigInt(Math.floor(Math.random() * Number.MAX_SAFE_INTEGER)) * BigInt(2)**BigInt(64),
        BigInt(Math.floor(Math.random() * Number.MAX_SAFE_INTEGER)) * BigInt(2)**BigInt(64)
    ];
    
    // Creating and assinging castling hashes
    table.enPassant = new Array(8).fill(0).map(() => BigInt(Math.floor(Math.random() * Number.MAX_SAFE_INTEGER)) * BigInt(2)**BigInt(64));

    return table;
}

// This function generates a Zobrist key for a given chessboard.
// A Zobrist key is a unique identifier for a particular chessboard configuration,
// and is used to speed up the evaluation of chess positions.
export function getZobristHash(chessBoard) {
    // Initialize the hash to 0.
    let hash = BigInt(0);

    // Get the board from the chessboard.
    let board = chessBoard.board;

    // Loop over all the squares on the board.
    for (let i = 0; i < 8; i++) {
        for (let j = 0; j < 8; j++) {
            // Get the piece at the current square.
            let piece = board[i][j];

            // If the square is empty, continue to the next square.
            if (piece.type === "Empty") {
                continue;
            }

            // Get the index of the piece in the piece mapping.
            let pieceIndex = pieceMapping[piece.color + piece.type];

            // XOR the hash with the Zobrist key for the piece at the current square.
            hash = hash ^ BigInt(zobristTable.pieces[i][j][pieceIndex]);
        }
    }

    // Get the castling rights from the chessboard.
    let castling = chessBoard.castling;

    // If white has kingside castling rights, XOR the hash with the Zobrist key for white kingside castling.
    if (castling.whiteKingside) {
        hash = hash ^ BigInt(zobristTable.castling[castlingMapping.whiteKingside]);
    }

    // If white has queenside castling rights, XOR the hash with the Zobrist key for white queenside castling.
    if (castling.whiteQueenside) {
        hash = hash ^ BigInt(zobristTable.castling[castlingMapping.whiteQueenside]);
    }

    // If black has kingside castling rights, XOR the hash with the Zobrist key for black kingside castling.
    if (castling.blackKingside) {
        hash = hash ^ BigInt(zobristTable.castling[castlingMapping.blackKingside]);
    }

    // If black has queenside castling rights, XOR the hash with the Zobrist key for black queenside castling.
    if (castling.blackQueenside) {
        hash = hash ^ BigInt(zobristTable.castling[castlingMapping.blackQueenside]);
    }

    // Get the en passant square from the chessboard.
    let enPassant = chessBoard.enPassant;

    // If there is an en passant square, XOR the hash with the Zobrist key for the en passant square.
    if (enPassant !== 9) {
        hash = hash ^ BigInt(zobristTable.enPassant[enPassantMapping[enPassant]]);
    }

    // Get the turn from the chessboard.
    let turn = chessBoard.turn;

    // XOR the hash with the Zobrist key for the turn.
    hash = hash ^ BigInt(zobristTable.turn[turnMapping[turn]]);

    // Return the hash.
    return hash;
}