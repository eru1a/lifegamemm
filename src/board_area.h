#pragma once

#include "board.h"
#include <gtkmm/drawingarea.h>

class BoardArea : public Gtk::DrawingArea {
public:
    BoardArea(Board *board);
    virtual ~BoardArea() {}

private:
    bool on_draw(const Cairo::RefPtr<Cairo::Context> &cr) override;
    bool on_button_press_event(GdkEventButton *event) override;
    bool on_motion_notify_event(GdkEventMotion *event) override;

private:
    Board *board;

    /// セルサイズ
    double cs = 20;

    // これらpairにすべき?
    double offset_x = 0, offset_y = 0;
    double offset_prev_x = 0, offset_prev_y = 0;
    double event_press_x = 0, event_press_y = 0;

    /// 与えられた座標からボード上のマスの座標を返す
    std::pair<int, int> get_pos(double x, double y) const;
};
