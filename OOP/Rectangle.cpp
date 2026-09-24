class Rectangle{
private:
    // bottom left coordinates
    int x1;
    int y1;
    // top right coordinates
    int x2;
    int y2;
public:
    Rectangle(int x1, int y1, int x2, int y2) {
        this->x1 = x1;
        this->y1 = y1;
        this->x2 = x2;
        this->y2 = y2;
    }

    bool contains(Rectangle r){
        return (x1 <= r.x1 && y1 <= r.y1 && x2 >= r.x2 && y2 >= r.y2);
    }

    bool overlaps(Rectangle r){
        return !(r.x2 < x1 || r.y2 < y1 || x2 < r.x1 || y2 < r.y1);
    }
}