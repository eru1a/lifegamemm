#include "mainwindow.h"
#include <glibmm/main.h>

MainWindow::MainWindow()
    : board(100, 100)
    , board_area(&board)
    , vbox(Gtk::ORIENTATION_VERTICAL)
    , hbox(Gtk::ORIENTATION_HORIZONTAL)
    , button_start_or_stop("start")
    , button_step("step") {
    set_title("Life Game");
    set_default_size(800, 600);

    button_start_or_stop.signal_clicked().connect([this] { run_toggle(); });
    button_step.signal_clicked().connect([this] {
        board.step();
        board_area.queue_draw();
    });

    hbox.pack_start(button_start_or_stop);
    hbox.pack_start(button_step);
    vbox.pack_start(hbox, false, false);
    vbox.pack_start(board_area);
    add(vbox);
}

bool MainWindow::on_key_press_event(GdkEventKey *event) {
    switch (event->keyval) {
    case GDK_KEY_s:
        run_toggle();
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

void MainWindow::run_toggle() {
    if (!is_running) {
        is_running = true;
        button_start_or_stop.set_label("stop");
        run();
    } else {
        button_start_or_stop.set_label("start");
        is_running = false;
    }
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
