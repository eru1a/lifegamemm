#pragma once

#include "board.h"
#include <gtkmm/drawingarea.h>

class BoardArea : public Gtk::DrawingArea {
public:
    BoardArea(Board *board);
    virtual ~BoardArea() {}

    void set_pattern(const std::vector<std::vector<bool>> &p) { pattern = p; }

private:
    bool on_button_press_event(GdkEventButton *event) override;
    bool on_motion_notify_event(GdkEventMotion *event) override;
    bool on_scroll_event(GdkEventScroll *event) override;
    bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;

private:
    Board *board;
    /// セルサイズ
    double cs = 20;
    /// 倍率
    double scale = 1.0;

    // これらpairにすべき?
    // TODO: offset_x, offset_yの初期値を真ん中にする
    double offset_x = 0, offset_y = 0;
    double offset_prev_x = 0, offset_prev_y = 0;
    double event_press_x = 0, event_press_y = 0;
    double mouse_pos_x = 0, mouse_pos_y = 0;

    std::vector<std::vector<bool>> pattern = {{1}};

    /// マウスの座標にパターンを置く
    void put();
    /// マウスの座標のセルを消す
    void remove();

    /// 与えられた座標からオフセットと倍率を考慮した座標を返す
    std::pair<int, int> get_xy(double x, double y) const;
    /// 与えられた座標からオフセットと倍率を考慮したボード上のマスの座標を返す
    std::pair<int, int> get_pos(double x, double y) const;
};
