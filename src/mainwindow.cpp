#include "mainwindow.h"
#include <glibmm/main.h>

MainWindow::MainWindow()
    : board(100, 100)
    , board_area(&board) {
    set_title("Life Game");
    set_default_size(800, 600);

    add(board_area);
}

bool MainWindow::on_key_press_event(GdkEventKey *event) {
    switch (event->keyval) {
    case GDK_KEY_s:
        is_running = !is_running;
        if (is_running)
            run();
        break;
    case GDK_KEY_n:
        board.step();
        board_area.queue_draw();
        break;
    default:
        break;
    }
    return true;
}

void MainWindow::run() {
    Glib::signal_timeout().connect(
        [this] {
            if (!is_running)
                return false;
            board.step();
            board_area.queue_draw();
            return true;
        },
        interval);
}
