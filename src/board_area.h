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

    struct P {
        double x, y;
    };

    P offset;
    P offset_prev = {0, 0};
    P event_press = {0, 0};
    P mouse_pos = {0, 0};

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
