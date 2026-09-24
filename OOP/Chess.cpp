class Position {
private:
    int row;
    int col;
public:
    Position(int r, int c){
        if(r < 0 || c < 0 || r>= 8 || c >= 8) throw invalid_argument("Invalid row or col provided");
        row = r;
        col = c;
    };
};

enum class Color {
    WHITE,
    BLACK
};

class Board{
private:
    Color currentPlayerColor;
    Piece* grid[8][8];

public:
    bool isEmpty(Position p){
        return !grid[p.row][p.col];
    }

    bool isOpponent(Position p){
        if(isEmpty(p)) return false;
        return currentPlayerColor != grid[p.row][p.col]->color;
    }
    
    bool isOwnPiece(Position p){
        if(isEmpty(p)) return false;
        return currentPlayerColor == grid[p.row][p.col]->color;
    }
};

class Piece{
public:
    Color color;
    virtual vector<Position> getValidMoves(Position current, Board& board) = 0;
    virtual ~Piece() {}
}

class Rook : public Piece{
public:
    vector<Position> getValidMoves(Position current, Board& board) override {
        vector<vector<int>> dirs = {{0,-1}, {-1,0}, {1, 0}, {0, 1}};
        vector<Position> validMoves;
        for(int d=0;d<4;d++){
            int r = current.row;
            int c = current.col;
            while(true) {
                r += dirs[d][0];
                c += dirs[d][1];
                if(r < 0 || r >= 8 || c < 0 || c >= 8) break;  // stepped off the board
                Position p = Position(r, c);
                if(board.isEmpty(p)) validMoves.push_back(p);
                else if(board.isOpponent(p)){
                    validMoves.push_back(p);
                    break;
                }
                else{
                    break;
                }
            }
        }
        return validMoves;
    }
}

class Knight : public Piece{
public:
    vector<Position> getValidMoves(Position current, Board& board) override {
        vector<vector<int>> dirs = {{1,-2}, {1, 2}, {-1,2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};
        vector<Position> validMoves;
        for(int d=0;d<8;d++){
            int r = current.row + dirs[d][0];
            int c = current.col + dirs[d][1];
            if(r < 0 || r >= 8 || c < 0 || c >= 8) continue;  // stepped off the board
            Position p = Position(r, c);
            if(board.isEmpty(p) || board.isOpponent(p)) validMoves.push_back(p);
        }
        return validMoves;
    }
}