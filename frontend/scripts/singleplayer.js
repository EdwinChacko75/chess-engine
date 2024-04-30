import {Game} from "./game.js";
import {Player, AI} from "./player.js";

let difficulty = 2;
function startGame() {
    document.getElementById('startNewGame').removeEventListener('click', startGame);
    let white = new Player('player1', 'white');
    let black = new AI('player2', 'black', difficulty);
    let game = new Game(white, black, 'singleplayer', difficulty);
    game.difficulty = difficulty;
    game.startGame(white);
    document.getElementById('startNewGame').addEventListener('click', startGame);
}

document.addEventListener("DOMContentLoaded", startGame());
document.getElementById('startNewGame').addEventListener('click', startGame);


