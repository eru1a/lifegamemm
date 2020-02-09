#include "mainwindow.h"
#include <glibmm/main.h>

MainWindow::MainWindow()
    : board(100, 100)
    , board_area(&board)
    , vbox(Gtk::ORIENTATION_VERTICAL)
    , hbox(Gtk::ORIENTATION_HORIZONTAL)
    , button_start_or_stop("start")
    , button_step("step")
    , adjustment_interval(Gtk::Adjustment::create(interval, 10, 300, 10, 100))
    , label_interval("interval (ms): ")
    , scale_interval(adjustment_interval, Gtk::ORIENTATION_HORIZONTAL)
    , hbox_interval(Gtk::ORIENTATION_HORIZONTAL) {
    set_title("Life Game");
    set_default_size(800, 600);

    button_start_or_stop.signal_clicked().connect([this] { run_toggle(); });
    button_step.signal_clicked().connect([this] {
        board.step();
        board_area.queue_draw();
    });

    scale_interval.set_digits(0);
    scale_interval.set_value_pos(Gtk::POS_LEFT);
    scale_interval.signal_value_changed().connect([this] {
        // 10刻みにする
        interval = int(scale_interval.get_value()) / 10 * 10;
        scale_interval.set_value(interval);
    });
    hbox_interval.pack_start(label_interval, false, false);
    hbox_interval.pack_start(scale_interval);

    hbox.pack_start(button_start_or_stop);
    hbox.pack_start(button_step);
    hbox.pack_start(hbox_interval);
    vbox.pack_start(hbox, false, false);
    vbox.pack_start(board_area);
    add(vbox);
}

bool MainWindow::on_key_press_event(GdkEventKey *event) {
    unsigned int modifier = event->state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK);
    if (!modifier && event->keyval == GDK_KEY_s) {
        // sキーでrun_toggle
        run_toggle();
    } else if (!modifier && event->keyval == GDK_KEY_n) {
        // nキーでstep
        board.step();
        board_area.queue_draw();
    } else if (modifier == GDK_CONTROL_MASK && event->keyval == GDK_KEY_q) {
        // ctrl+qで終了
        hide();
    }
    return true;
}

void MainWindow::run_toggle() {
    if (!is_running) {
        is_running = true;
        button_start_or_stop.set_label("stop");
        scale_interval.set_sensitive(false);
        run();
    } else {
        is_running = false;
        button_start_or_stop.set_label("start");
        scale_interval.set_sensitive(true);
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
