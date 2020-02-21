#pragma once

#include <cstdint>
#include <vector>

// http://vivi.dyndns.org/tech/games/LifeGame.html
// 番兵は上下と左と右下
//    ┏━━━━━━━━━━━┓
//    ┃         番人         ┃
//    ┃  ┌────────┬┨
//    ┃  │・(0, 0)        │┃パディング
//    ┃  │                │┃(w が T のサイズの倍数で無い場合）
//    ┃  │                │┃
//    ┃  │     (w-1,h-1)・│┃
//    ┃  └────────┴┸━┓
//    ┃         番人             ┃
//    ┗━━━━━━━━━━━━━┛
//     ←      hSize      →

class Board {
public:
    Board(int col, int row);

    // 0-indexedでアクセス
    bool get(int x, int y) const;
    void set(int x, int y, bool cell);
    int get_col() const { return col; }
    int get_row() const { return row; }
    /// 範囲外チェック
    /// 範囲内ならtrue
    bool check_bound(int x, int y) { return 0 <= x && x < col && 0 <= y && y < row; }

    void step();
    void clear();

    /// サイズ変更
    void resize(int new_col, int new_row);

private:
    using board_t = std::vector<uint8_t>;
    // board_tに格納される型のビット数
    const int bits = 8;

    int col, row;

    // 左の番兵を含めた1行のサイズ
    // 左の番兵は、左端の1byteの中の0番目のビットを使うので
    // hsize = (col + 1 + (bits - 1)) / bits で ((col + bits) / bits) となる
    int hsize;

    // boardのサイズ
    // hsizeが上下の番兵を合わせて(row+2)個分あり、
    // さらに右下の番兵としてbyteを1つ追加するので
    // size = hsize * (row + 2) + 1
    int size;

    // 盤面をbyteの配列で表現
    // 1byteにx方向のセルが8個分入っている
    board_t board;

    /// 引数のbに対してsetを実行
    void set_sub(board_t &b, int x, int y, bool cell);
};
