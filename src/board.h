#pragma once

#include <vector>

class Board {
public:
    Board();
    Board(int col, int row);

    bool get(int x, int y) const { return board.at(y).at(x); }
    void set(int x, int y, bool cell) { board.at(y).at(x) = cell; }
    int get_col() const { return col; }
    int get_row() const { return row; }
    bool check_bound(int x, int y) { return 0 <= x && x < col && 0 <= y && y < row; }

    void step();
    void clear();

private:
    int col;
    int row;
    std::vector<std::vector<bool>> board;
    int arround(int x, int y) const;
};
