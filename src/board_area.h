#pragma once

#include "board.h"
#include "pattern.h"
#include <functional>
#include <gtkmm/drawingarea.h>

class BoardArea : public Gtk::DrawingArea {
public:
    BoardArea(Board *board);
    virtual ~BoardArea() {}

    /// パターンをセット
    void set_pattern(const Pattern &p) { pattern = p; }
    /// 時計回りに90度回転させる
    void rotate();

private:
    bool on_button_press_event(GdkEventButton *event) override;
    bool on_motion_notify_event(GdkEventMotion *event) override;
    bool on_scroll_event(GdkEventScroll *event) override;
    bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;

private:
    Board *board;
    /// セルのサイズ
    double cs = 20;
    /// 倍率
    double scale = 1.0;
    /// 回転数(0~3)
    int r = 0;

    /// patternの各要素に対してfを実行
    void pattern_iter(const std::function<void(int, int, bool)> &f);

    struct P {
        double x, y;
    };

    P offset;
    P offset_prev = {0, 0};
    P event_press = {0, 0};
    P mouse_pos = {0, 0};

    Pattern pattern;

    /// マウスの座標にパターンを置く
    void put();
    /// マウスの座標のセルを消す
    void remove();

    /// 与えられた座標からオフセットと倍率を考慮した座標を返す
    std::pair<int, int> get_xy(double x, double y) const;
    /// 与えられた座標からオフセットと倍率を考慮したボード上のマスの座標を返す
    std::pair<int, int> get_pos(double x, double y) const;
};
