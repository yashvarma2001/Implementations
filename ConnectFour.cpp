/*
    "Build the object-oriented design for a two-player Connect Four game. 
    Players take turns dropping discs into a 7-column, 6-row board. 
    The first to align four of their own discs vertically, horizontally, or diagonally wins."

    1. Primary Capabilities
    2. Error Handling
    3. Scope boundaries

Requirements:
    1. Two players take turns dropping discs into a 7-column, 6-row board
    2. A disc falls to the lowest available row in the chosen column
    3. The game ends when:
        - A player gets four discs in a row (vertical, horizontal, or diagonal). They win.
        - The board is full. It's a draw.
    4. Invalid moves should be rejected clearly:
        - Dropping in a full column.
        - Moving out of turn.
        - Moving after the game is over.

Out of scope: 
    - UI support
    - Concurrent games
    - Move history
    - Undo
    - Board size configuration

Entities:
    - Players
    - Board
    - Game
    - Disc

SRP - Single responsibility principle

For each method, follow a consistent pattern:
Define the core logic - The happy path that fulfills the requirement.
Consider edge cases - What can go wrong? Invalid inputs, illegal states, boundary conditions.

*/

enum class DiscColor {
    RED,
    YELLOW
};

class Board {
private:
    int rows = 6;
    int columns = 7;
    vector<vector<DiscColor>> board;

public:
    boolean canPlace(int column){

    }

    int placeDisc(int column, DiscColor color){

    }

    boolean isFull(){

    }

    boolean checkWin(DiscColor color){

    }
}

class Player {
private:
    DiscColor color;
    string name;


}

class Game {

private:
    Player player1;
    Player player2;
    Player currentPlayer;
    Board board;
    GameState state; // ENUM inprogress, draw, won
    Player winner;

public:
    Game(Player player1, Player player2){
        this->player1 = player1;
        this->player2 = player2;
    }

    boolean makeMove(Player player, int column){
        if(state!=Inprogrees) return false;
        if(payer!=curretnplayer) return false;
        int row = Board.placeDisc(column, player.color);
        if(board.checkWin()) state = WON; winner = player;
        else if(board.isFull) state = draw;
        else currentPlayer = (player == player1) ? player2 : player1;
    }



}

