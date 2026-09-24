enum Player {
    EMPTY = 0,
    PLAYER_ONE = 1,
    PLAYER_TWO = 2
};

enum Status {
    IN_PROGRESS,
    END,
    DRAW
};

class TicTacToe{

private: 
    int n;
    vector<vector<Player>> board;
    Player p;
    Status s;
    bool checkWin(Player p, int r, int c){
        for(int j=0;j<n;j++){
            if(board[r][j] != p){
                break;
            }
            if(j == n-1){
                return true;
            }
        }
        for(int i=0;i<n;i++){
            if(board[i][c] != p){
                break;
            }
            if(i == n-1) return true;
        }
        if(r==c){
            for(int i=0;i<n;i++){
                if(board[i][i] != p){
                    break;
                }
                if(i == n-1) return true;
            }
        }
        if(r+c == n-1){
            for(int i=0;i<n;i++){
                if(board[i][n-1-i] != p){
                    break;
                }
                if(i == n-1) return true;
            }
        }
        return false;
    }

    bool checkValidMove(int r, int c){
        return (r >= 0 && c >= 0 && r<n && c<n);
    }

    void changePlayer(){
        if(p == PLAYER_ONE) p = PLAYER_TWO;
        else p = PLAYER_ONE;
    }

    bool isFull(){
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(board[i][j] == EMPTY){
                    return false;
                }
            }
        }
        return true;
    }

public:
    TicTacToe(int n){
        if (n < 3) {
            throw invalid_argument("N must be greater than equal to 3");
        }
        this->n = n;
        board = vector<vector<Player>>(n, vector<Player>(n, EMPTY));
        p = PLAYER_ONE;
        s = IN_PROGRESS;
    }

    string makeMove(Player p, int r, int c){
        if(s != IN_PROGRESS) return "Game not in progress";
        if(this->p != p) return "Invalid player";
        if(!checkValidMove(r, c)){
            return "Invalid postion provided";
        }
        if(board[r][c] != EMPTY) return "Position already filled";
        board[r][c] = p;
        if(checkWin(p, r, c)){
            s = END;
            return "Player: " + to_string(p) + " wins the game";
        }
        if(isFull()){
            s = DRAW;
            return "Tie game draw";
        }
        changePlayer();
        return "Player :" + to_string(p) + " turn now";
    }
}