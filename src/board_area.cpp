#include "board_area.h"

BoardArea::BoardArea(Board *board)
    : board(board) {
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
    add_events(Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
    add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK | Gdk::SMOOTH_SCROLL_MASK);
}

std::pair<int, int> BoardArea::get_xy(double x, double y) const {
    return {(x - offset.x) / scale, (y - offset.y) / scale};
}

std::pair<int, int> BoardArea::get_pos(double x, double y) const {
    auto [x1, y1] = get_xy(x, y);
    return {x1 / cs, y1 / cs};
}

void BoardArea::put() {
    auto [pos_x, pos_y] = get_pos(mouse_pos.x, mouse_pos.y);
    for (size_t i = 0; i < pattern.size(); i++) {
        for (size_t j = 0; j < pattern[0].size(); j++) {
            if (!pattern[i][j])
                continue;
            int x = pos_x + i;
            int y = pos_y + j;
            if (!board->check_bound(x, y))
                continue;
            board->set(x, y, true);
        }
    }
}

void BoardArea::remove() {
    auto [pos_x, pos_y] = get_pos(mouse_pos.x, mouse_pos.y);
    if (board->check_bound(pos_x, pos_y))
        board->set(pos_x, pos_y, false);
}

bool BoardArea::on_button_press_event(GdkEventButton *event) {
    mouse_pos = {event->x, event->y};

    if (event->button == 2) {
        // 中クリックの場合offset_prevとevent_pressを更新
        offset_prev = offset;
        event_press = {event->x, event->y};
    } else if (event->button == 1) {
        // 左クリックの場合セットされているパターンを置く
        put();
    } else if (event->button == 3) {
        // 右クリックの場合はマウスの座標のセルを消す
        remove();
    }

    queue_draw();
    return true;
}

bool BoardArea::on_motion_notify_event(GdkEventMotion *event) {
    mouse_pos = {event->x, event->y};

    if (event->state & GDK_BUTTON2_MASK) {
        // 中クリックの場合offsetを更新
        offset.x = offset_prev.x - (event_press.x - event->x);
        offset.y = offset_prev.y - (event_press.y - event->y);
    } else if (event->state & GDK_BUTTON1_MASK) {
        // 左クリックの場合セットされているパターンを置く
        put();
    } else if (event->state & GDK_BUTTON3_MASK) {
        // 右クリックの場合はマウスの座標のセルを消す
        remove();
    }

    queue_draw();
    return true;
}

bool BoardArea::on_scroll_event(GdkEventScroll *event) {
    mouse_pos = {event->x, event->y};

    auto [x0, y0] = get_xy(event->x, event->y);
    scale *= (event->delta_y > 0) ? 0.8 : 1.2;
    auto [x1, y1] = get_xy(event->x, event->y);
    offset.x -= (x0 - x1) * scale;
    offset.y -= (y0 - y1) * scale;

    queue_draw();
    return true;
}

bool BoardArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    cr->set_line_width(1);

    Cairo::Matrix matrix(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    // 倍率
    matrix.scale(scale, scale);
    // 移動
    matrix.translate(offset.x / scale, offset.y / scale);

    cr->transform(matrix);

    // 背景を黒にする
    cr->set_source_rgb(0, 0, 0);
    cr->rectangle(0, 0, board->get_col() * cs, board->get_row() * cs);
    cr->fill();

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

    auto [pos_x, pos_y] = get_pos(mouse_pos.x, mouse_pos.y);
    cr->set_source_rgba(0, 0, 1, 0.7);
    for (size_t y = 0; y < pattern.size(); y++) {
        for (size_t x = 0; x < pattern[0].size(); x++) {
            if (!pattern[y][x])
                continue;
            if (!board->check_bound(pos_x + x, pos_y + y))
                continue;
            cr->rectangle((pos_x + x) * cs, (pos_y + y) * cs, cs, cs);
        }
    }
    cr->fill();

    return true;
}
