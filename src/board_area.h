#pragma once

#include "board.h"
#include "pattern.h"
#include <functional>
#include <gtkmm/drawingarea.h>

enum class State {
    /// 編集
    Edit,
    /// 選択が始まった状態(MainWindow側のRadioButtonのeditをクリックした直後)
    SelectStart,
    /// 選択中(開始地点が決まった状態)
    Select,
    /// 選択が終わった状態
    SelectDone,
};

/// 画面上の座標
struct ScreenPos {
    double x, y;
};

/// ライフゲームの盤面の左上からの座標
/// つまりScreenPosにオフセットと倍率を考慮した座標
struct Pos {
    double x, y;
};

/// 盤面の座標
struct BoardPos {
    int x, y;
};

class BoardArea : public Gtk::DrawingArea {
public:
    BoardArea(Board *board);
    virtual ~BoardArea() {}

    /// パターンをセット
    void set_pattern(const Pattern &p) { pattern = p; }
    /// 時計回りに90度回転させる
    void rotate();

    /// stateをEditにする
    void edit_start();
    /// stateをSelectStartにする
    void select_start();
    /// 選択された範囲にあるパターンをコピー
    /// stateがSelectDoneでなければ無効
    void copy();
    /// 選択された範囲にあるパターンをカット
    /// stateがSelectDoneでなければ無効
    void cut();

private:
    bool on_button_press_event(GdkEventButton *event) override;
    bool on_button_release_event(GdkEventButton *event) override;
    bool on_motion_notify_event(GdkEventMotion *event) override;
    bool on_scroll_event(GdkEventScroll *event) override;
    bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;

    /// 選択している範囲を赤い四角形で描画
    void draw_selected_rectangle(const Cairo::RefPtr<Cairo::Context> &cr, const Pos &start,
                                 const Pos &end);
    /// 選択された生きているセルを描画
    void draw_selected_cells(const Cairo::RefPtr<Cairo::Context> &cr, const Pos &start,
                             const Pos &end);

private:
    Board *board;
    /// セルのサイズ
    double cs = 20;
    /// 倍率
    double scale = 1.0;
    /// 回転数(0~3)
    int r = 0;
    /// 状態
    State state = State::Edit;

    /// patternの各要素に対してfを実行
    void pattern_iter(const std::function<void(int, int, bool)> &f);

    Pos offset;
    Pos offset_prev = {0, 0};
    ScreenPos event_press = {0, 0};
    ScreenPos mouse_pos = {0, 0};

    /// 選択の開始地点
    /// stateがSelectまたはSelectDoneでない場合はnullopt
    std::optional<Pos> select_start_pos;
    /// 選択の終点地点
    /// stateがSelectDoneでない場合はnullopt
    std::optional<Pos> select_end_pos;

    Pattern pattern;

    /// マウスの座標にパターンを置く
    void put();
    /// マウスの座標のセルを消す
    void remove();

    /// ScreenPos -> Pos
    Pos get_xy(const ScreenPos &p) const;
    /// ScreenPos -> BoardPos
    BoardPos get_pos(const ScreenPos &p) const;
    /// Pos -> BoardPos
    BoardPos get_board_pos_from_pos(const Pos &p) const;

    /// copyとcutがほぼ共通の処理なので補助関数を用意した
    void copy_sub(bool cut = false);
};
