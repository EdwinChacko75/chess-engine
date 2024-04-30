import {Game} from "./game.js";
import {Player} from "./player.js";


function startGame() {
    document.getElementById('startNewGame').removeEventListener('click', startGame);
    let white = new Player('player1', 'white');
    let black = new Player('player2', 'black');
    let game = new Game(white, black, 'multiplayer');
    game.startGame(white);
    document.getElementById('startNewGame').addEventListener('click', startGame);
}

document.addEventListener("DOMContentLoaded", startGame());
document.getElementById('startNewGame').addEventListener('click', startGame);


document.addEventListener("DOMContentLoaded", (event)=> {
    var canvas = document.getElementById('myCanvas');
    var ctx=canvas.getContext('2d');
    
    window.onload = function() {
    
        // ChessBoard.arr(ctx, 10, 10, 100, 100, 10, 'red');
        // ChessBoard.arr(ctx, 100, 10, 140, 140, 3, 'black');
    }
});

// document.getElementById('flipBoardButton').addEventListener('click', () => {
//     chessBoard.flipBoard(chessBoard.board);  
// });
