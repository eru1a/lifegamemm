#include "board_area.h"

BoardArea::BoardArea(Board *board)
    : board(board) {
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
    add_events(Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
    add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);
}

bool BoardArea::on_button_press_event(GdkEventButton *event) {
    int x = event->x / cs;
    int y = event->y / cs;

    if (event->button == 1) {
        board->set(x, y, true);
    } else if (event->button == 3) {
        board->set(x, y, false);
    }

    queue_draw();
    return true;
}

bool BoardArea::on_motion_notify_event(GdkEventMotion *event) {
    int x = event->x / cs;
    int y = event->y / cs;

    if (event->state & GDK_BUTTON1_MASK) {
        board->set(x, y, true);
    } else if (event->state & GDK_BUTTON3_MASK) {
        board->set(x, y, false);
    }

    queue_draw();
    return true;
}

bool BoardArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    cr->set_line_width(1);
    for (int y = 0; y < board->get_row(); y++) {
        for (int x = 0; x < board->get_col(); x++) {
            if (board->get(x, y)) {
                cr->set_source_rgb(0, 1, 0);
                cr->rectangle(x * cs, y * cs, cs, cs);
                cr->fill();
                cr->set_source_rgb(0.5, 0.5, 0.5);
                cr->rectangle(x * cs, y * cs, cs, cs);
                cr->stroke();
            }
        }
    }

    return true;
}
