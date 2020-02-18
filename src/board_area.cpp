#include "board_area.h"
#include <cassert>

BoardArea::BoardArea(Board *board)
    : board(board) {
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
    add_events(Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK);
    add_events(Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK);
    add_events(Gdk::BUTTON_PRESS_MASK | Gdk::SCROLL_MASK | Gdk::SMOOTH_SCROLL_MASK);
}

Pos BoardArea::get_xy(const ScreenPos &p) const {
    return Pos{(p.x - offset.x) / scale, (p.y - offset.y) / scale};
}

BoardPos BoardArea::get_pos(const ScreenPos &p) const {
    auto pos = get_xy(p);
    return BoardPos{int(pos.x / cs), int(pos.y / cs)};
}

BoardPos BoardArea::get_board_pos_from_pos(const Pos &p) const {
    return BoardPos{int(p.x / cs), int(p.y / cs)};
}

void BoardArea::put() {
    auto board_pos = get_pos(mouse_pos);
    pattern_iter([&](int i, int j, bool cell) {
        if (!cell)
            return;
        int x = board_pos.x + j;
        int y = board_pos.y + i;
        if (!board->check_bound(x, y))
            return;
        board->set(x, y, true);
    });
}

void BoardArea::remove() {
    auto board_pos = get_pos(mouse_pos);
    if (board->check_bound(board_pos.x, board_pos.y))
        board->set(board_pos.x, board_pos.y, false);
}

void BoardArea::rotate() {
    // 時計回りに90度回転した新しいパターン
    std::vector<std::vector<bool>> p(pattern.col, std::vector<bool>(pattern.row));
    for (int i = 0; i < pattern.row; i++) {
        for (int j = 0; j < pattern.col; j++) {
            p[j][pattern.row - 1 - i] = pattern.pattern[i][j];
        }
    }
    std::swap(pattern.col, pattern.row);
    pattern.pattern = std::move(p);
    r = (r + 1) % 4;
    queue_draw();
}

void BoardArea::edit_start() {
    select_start_pos = std::nullopt;
    select_end_pos = std::nullopt;
    state = State::Edit;
}

void BoardArea::select_start() {
    select_start_pos = std::nullopt;
    select_end_pos = std::nullopt;
    state = State::SelectStart;
}

bool BoardArea::on_button_press_event(GdkEventButton *event) {
    mouse_pos = {event->x, event->y};

    if (event->button == 2) {
        // 中クリックの場合offset_prevとevent_pressを更新
        offset_prev = offset;
        event_press = {event->x, event->y};
    } else if (event->button == 1) {
        // 左クリックの場合
        switch (state) {
        case State::Edit:
            // 編集中ならセットされているパターンを置く
            put();
            break;
        case State::SelectStart: {
            // 選択開始時ならマウスの座標を選択開始の座標としてstateをSelectにする
            // TODO: 盤面からはみ出ないようにする
            select_start_pos = get_xy(mouse_pos);
            state = State::Select;
            break;
        }
        case State::Select:
            // 起きないはず
            assert(false);
            break;
        case State::SelectDone: {
            // 選択終了時ならマウスの座標を選択開始の座標としてselectをSelectにする
            select_start_pos = get_xy(mouse_pos);
            select_end_pos = std::nullopt;
            state = State::Select;
            break;
        }
        }
    } else if (event->button == 3) {
        // 右クリックの場合はマウスの座標のセルを消す
        remove();
    }

    queue_draw();
    return true;
}

bool BoardArea::on_button_release_event(GdkEventButton *event) {
    if (!(state == State::Select && event->button == 1))
        return true;

    assert(select_start_pos.has_value());
    select_end_pos = get_xy(ScreenPos{event->x, event->y});
    state = State::SelectDone;

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
        // 左クリックの場合
        switch (state) {
        case State::Edit:
            // 編集中ならセットされているパターンを置く
            put();
            break;
        default:
            break;
        }
    } else if (event->state & GDK_BUTTON3_MASK) {
        // 右クリックの場合はマウスの座標のセルを消す
        remove();
    }

    queue_draw();
    return true;
}

bool BoardArea::on_scroll_event(GdkEventScroll *event) {
    mouse_pos = {event->x, event->y};

    auto pos0 = get_xy(mouse_pos);
    scale *= (event->delta_y > 0) ? 0.8 : 1.2;
    auto pos1 = get_xy(mouse_pos);
    offset.x -= (pos0.x - pos1.x) * scale;
    offset.y -= (pos0.y - pos1.y) * scale;

    queue_draw();
    return true;
}

// 選択された範囲の左上の座標と右下の座標を返す
std::pair<Pos, Pos> get_selected_start_end(const Pos &p1, const Pos &p2) {
    Pos start{std::min(p1.x, p2.x), std::min(p1.y, p2.y)};
    Pos end{std::max(p1.x, p2.x), std::max(p1.y, p2.y)};
    return {start, end};
}

bool BoardArea::on_draw(const Cairo::RefPtr<Cairo::Context> &cr) {
    Cairo::Matrix matrix(1.0, 0.0, 0.0, 1.0, 0.0, 0.0);
    // 倍率
    matrix.scale(scale, scale);
    // 移動
    matrix.translate(offset.x / scale, offset.y / scale);

    cr->transform(matrix);

    cr->set_line_width(1);
    // 背景を黒にする
    cr->set_source_rgb(0, 0, 0);
    cr->rectangle(0, 0, board->get_col() * cs, board->get_row() * cs);
    cr->fill();

    // 盤面の描画
    for (int y = 0; y < board->get_row(); y++) {
        for (int x = 0; x < board->get_col(); x++) {
            if (board->get(x, y)) {
                cr->set_source_rgb(0, 1, 0);
                cr->rectangle(x * cs, y * cs, cs, cs);
            }
        }
    }
    cr->fill();

    switch (state) {
    case State::Edit: {
        // セットされてるパターンを描画
        auto board_pos = get_pos(mouse_pos);
        cr->set_source_rgba(0, 0, 1, 0.7);
        pattern_iter([&](int i, int j, bool cell) {
            if (!cell)
                return;
            int x = board_pos.x + j;
            int y = board_pos.y + i;
            if (!board->check_bound(x, y))
                return;
            cr->rectangle(x * cs, y * cs, cs, cs);
        });
        cr->fill();
        break;
    }
    case State::SelectStart:
        break;
    case State::Select: {
        // select_start_pos ~ mouse_pos
        assert(select_start_pos.has_value());
        auto pos = get_xy(mouse_pos);
        auto [start, end] = get_selected_start_end(select_start_pos.value(), pos);
        draw_selected_rectangle(cr, start, end);
        draw_selected_cells(cr, start, end);
        break;
    }
    case State::SelectDone: {
        // select_start_pos ~ select_end_pos
        assert(select_start_pos.has_value());
        assert(select_end_pos.has_value());
        auto [start, end] =
            get_selected_start_end(select_start_pos.value(), select_end_pos.value());
        draw_selected_rectangle(cr, start, end);
        draw_selected_cells(cr, start, end);
        break;
    }
    }

    // グリッド
    if (draw_grid) {
        cr->set_line_width(1);
        cr->set_source_rgba(0.5, 0.5, 0.5, 0.5);
        for (int y = 0; y < board->get_row(); y++) {
            cr->move_to(0, y * cs);
            cr->line_to(board->get_col() * cs, y * cs);
        }
        for (int x = 0; x < board->get_col(); x++) {
            cr->move_to(x * cs, 0);
            cr->line_to(x * cs, board->get_row() * cs);
        }
        cr->stroke();
    }

    return true;
}

void BoardArea::draw_selected_rectangle(const Cairo::RefPtr<Cairo::Context> &cr, const Pos &start,
                                        const Pos &end) {
    cr->set_line_width(3);
    cr->set_source_rgba(1, 0, 0, 0.7);
    cr->rectangle(start.x, start.y, end.x - start.x, end.y - start.y);
    cr->stroke();
}

void BoardArea::draw_selected_cells(const Cairo::RefPtr<Cairo::Context> &cr, const Pos &start,
                                    const Pos &end) {
    cr->set_line_width(1);
    auto board_start_pos = get_board_pos_from_pos(start);
    auto board_end_pos = get_board_pos_from_pos(end);
    for (int y = board_start_pos.y; y <= board_end_pos.y; y++) {
        for (int x = board_start_pos.x; x <= board_end_pos.x; x++) {
            if (board->check_bound(x, y) && board->get(x, y)) {
                cr->set_source_rgb(1, 1, 0);
                cr->rectangle(x * cs, y * cs, cs, cs);
            }
        }
    }
    cr->fill();
}

void BoardArea::copy_sub(bool cut) {
    if (state != State::SelectDone)
        return;
    assert(select_start_pos.has_value());
    assert(select_end_pos.has_value());

    auto [start, end] = get_selected_start_end(select_start_pos.value(), select_end_pos.value());
    auto board_start_pos = get_board_pos_from_pos(start);
    auto board_end_pos = get_board_pos_from_pos(end);

    // TODO: 余分な空白を削る
    std::vector<std::vector<bool>> p;
    for (int y = board_start_pos.y; y <= board_end_pos.y; y++) {
        if (!board->check_bound(0, y))
            continue;
        std::vector<bool> row;
        for (int x = board_start_pos.x; x <= board_end_pos.x; x++) {
            if (board->check_bound(x, y)) {
                row.push_back(board->get(x, y));
                if (cut)
                    board->set(x, y, false);
            }
        }
        p.push_back(row);
    }
    std::string name = cut ? "Cut" : "Copy";
    int row = p.size();
    int col = row > 0 ? p[0].size() : 0;

    pattern = Pattern(name, p, {}, col, row);
    state = State::Edit;
}

void BoardArea::copy() { copy_sub(); }
void BoardArea::cut() { copy_sub(true); }

void BoardArea::pattern_iter(const std::function<void(int, int, bool)> &f) {
    for (int i = 0; i < pattern.row; i++) {
        for (int j = 0; j < pattern.col; j++) {
            f(i, j, pattern.pattern[i][j]);
        }
    }
}
