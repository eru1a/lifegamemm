#include "board.h"

Board::Board()
    : col(100)
    , row(100)
    , board(std::vector<std::vector<bool>>(row, std::vector<bool>(col, false))) {}

Board::Board(int col, int row)
    : col(col)
    , row(row)
    , board(std::vector<std::vector<bool>>(row, std::vector<bool>(col, false))) {}

int Board::arround(int x, int y) const {
    int result = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0)
                continue;
            int nx = x + dx;
            int ny = y + dy;
            if (!(0 <= nx && nx < col && 0 <= ny && ny < row))
                continue;
            result += board.at(ny).at(nx);
        }
    }
    return result;
}

void Board::step() {
    auto next = board;
    for (int y = 0; y < row; y++) {
        for (int x = 0; x < col; x++) {
            switch (arround(x, y)) {
            case 2:
                break;
            case 3:
                next.at(y).at(x) = true;
                break;
            default:
                next.at(y).at(x) = false;
                break;
            }
        }
    }
    board = next;
}
