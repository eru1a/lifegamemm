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
};
