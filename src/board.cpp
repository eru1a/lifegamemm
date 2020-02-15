#include "board.h"
#include <memory>

Board::Board(int col, int row)
    : col(col)
    , row(row)
    // 番兵を含めるので2つ余分に取る(呼び出す側は番兵の存在を考慮しない)
    , board(std::vector<std::vector<bool>>(row + 2, std::vector<bool>(col + 2, false))) {}

bool Board::get(int x, int y) const { return board[y + 1][x + 1]; }

void Board::set(int x, int y, bool cell) { board[y + 1][x + 1] = cell; }

void Board::step() {
    std::vector<std::vector<bool>> next =
        std::vector<std::vector<bool>>(row + 2, std::vector<bool>(col + 2, false));
    for (int y = 0; y < row; y++) {
        for (int x = 0; x < col; x++) {
            int cnt = 0;
            cnt += get(x - 1, y - 1);
            cnt += get(x - 1, y + 1);
            cnt += get(x - 1, y);
            cnt += get(x + 1, y - 1);
            cnt += get(x + 1, y + 1);
            cnt += get(x + 1, y);
            cnt += get(x, y - 1);
            cnt += get(x, y + 1);
            if (cnt == 3 || (cnt == 2 && get(x, y)))
                next[y + 1][x + 1] = true;
            else
                next[y + 1][x + 1] = false;
        }
    }
    board = std::move(next);
}

void Board::clear() {
    board = std::vector<std::vector<bool>>(row + 2, std::vector<bool>(col + 2, false));
}
