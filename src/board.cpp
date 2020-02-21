#include "board.h"

Board::Board(int col, int row)
    : col(col)
    , row(row)
    , hsize((col + bits) / bits)
    , size(hsize * (row + 2) + 1)
    , board(board_t(size)) {}

bool Board::get(int x, int y) const {
    // この辺少々値を変えても問題なく動いちゃうことが多くて合ってるかどうか分からない
    // ちゃんとテストを書くべきなのだろうけれど...
    size_t offset = (x + 1) / bits + (y + 1) * hsize;
    uint8_t mask = (1 << (bits - 1)) >> ((x + 1) & (bits - 1));
    return (board[offset] & mask) != 0;
}

void Board::set_sub(board_t &b, int x, int y, bool cell) {
    size_t offset = (x + 1) / bits + (y + 1) * hsize;
    uint8_t mask = (1 << (bits - 1)) >> ((x + 1) & (bits - 1));
    if (cell)
        b[offset] |= mask;
    else
        b[offset] &= ~mask;
}

void Board::set(int x, int y, bool cell) { set_sub(board, x, y, cell); }

void Board::step() {
    board_t next(size);
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
                set_sub(next, x, y, true);
            else
                set_sub(next, x, y, false);
        }
    }
    board.swap(next);
}

void Board::clear() { board = board_t(size); }

void Board::resize(int new_col, int new_row) {
    col = new_col;
    row = new_row;
    hsize = (new_col + bits) / bits;
    size = hsize * (new_row + 2) + 1;
    board = board_t(size);
}
