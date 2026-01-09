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

#include <iostream>
#include <vector>
#include <string>
#include <optional>

using namespace std;

enum class DiscColor {
    RED,
    YELLOW
};

enum class GameState{
    INPROGRESS,
    WON,
    DRAW
};

class Board {
private:
    int rows = 6;
    int cols = 7;
    vector<vector<optional<DiscColor>>> board;

    int countInDirection(int row, int col, DiscColor color, int dr, int dc){
        int count = 0;
        int r = row + dr;
        int c = col + dc;
        while(r<rows && c<cols && r>=0 && c>=0 && board[r][c].has_value() && board[r][c] == color){
            count++;
            r+=dr;
            c+=dc;
        }
        return count;
    }

public:
    Board() : board(rows, vector<optional<DiscColor>>(cols, nullopt)) {};
    bool canPlace(int column){
        if(column < 0 || column >= cols){
            return false;
        }
        return !board[0][column].has_value();
    }

    int placeDisc(int column, DiscColor color){
        if(!canPlace(column)){
            return -1;
        }
        for(int i = rows-1;i>=0;i--){
            if(!board[i][column].has_value()){
                board[i][column] = color;
                return i;
            }
        }

        return -1;
    }

    bool isFull(){
        for(int i =0;i<cols;i++){
            if(!board[0][i].has_value()){
                return false;
            }
        }
        return true;
    }

    bool checkWin(int row, int col, DiscColor color){
        int directions[][2] = {
            {0, 1},
            {1, 0},
            {1, 1},
            {-1, 1}
        };
        for(auto& dir: directions){
            int count =1;
            count += countInDirection(row, col, color, dir[0], dir[1]);
            count += countInDirection(row, col, color, -dir[0], -dir[1]);
            if(count>=4){
                return true;
            }
        }
        return false;
    }
};

class Player {
private:
    DiscColor color;
    string name;

public:
    Player(string n, DiscColor c) : name(n), color(c) {};
    string getName() const { 
        return name; 
    }
    DiscColor getColor() const { 
        return color; 
    }

};

class Game {

private:
    Player player1;
    Player player2;
    Player* currentPlayer;
    Board board;
    GameState state; // ENUM inprogress, draw, won
    Player* winner;

public:
    Game(Player p1, Player p2) : player1(p1), player2(p2) {
        currentPlayer = &player1;
        state = GameState::INPROGRESS;
        winner = nullptr;
    }

    bool makeMove(int column){
        if(state != GameState::INPROGRESS) return false;
        int row = board.placeDisc(column, currentPlayer->getColor());
        if (row == -1) {
            return false;
        }
        if(board.checkWin(row, column, currentPlayer->getColor())){
            state = GameState::WON; 
            winner = currentPlayer;
        }
        else if(board.isFull()){
            state = GameState::DRAW;
        }
        else currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
        return true;
    }

    GameState getState() const { 
        return state; 
    }
    string getCurrentPlayerName() const { 
        return currentPlayer->getName(); 
    }
    string getWinnerName() const { 
        return (winner) ? winner->getName() : "None"; 
    }

};


int main() {
    Player p1("Alice", DiscColor::RED);
    Player p2("Bob", DiscColor::YELLOW);
    Game game(p1, p2);

    // Simulation
    game.makeMove(0); // Alice
    game.makeMove(1); // Bob
    game.makeMove(0); // Alice
    game.makeMove(1); // Bob
    game.makeMove(0); // Alice
    game.makeMove(1); // Bob
    game.makeMove(0); // Alice Wins!

    if (game.getState() == GameState::WON) {
        cout << "Winner: " << game.getWinnerName() << endl;
    }

    return 0;
}

