#include "mainwindow.h"
#include <filesystem>
#include <fstream>
#include <glibmm/main.h>
#include <iostream>

MainWindow::MainWindow()
    : board(100, 100)
    , board_area(&board)
    , vbox(Gtk::ORIENTATION_VERTICAL)
    , hbox(Gtk::ORIENTATION_HORIZONTAL)
    , hbox_buttons(Gtk::ORIENTATION_HORIZONTAL)
    , button_start_or_stop("start")
    , button_step("step")
    , button_clear("clear")
    , adjustment_interval(Gtk::Adjustment::create(interval, 10, 300, 10, 100))
    , label_interval("interval (ms): ")
    , scale_interval(adjustment_interval, Gtk::ORIENTATION_HORIZONTAL)
    , hbox_interval(Gtk::ORIENTATION_HORIZONTAL) {
    set_title("Life Game");
    set_default_size(800, 600);

    button_start_or_stop.signal_clicked().connect([this] { toggle(); });
    button_step.signal_clicked().connect([this] {
        board.step();
        board_area.queue_draw();
    });
    button_clear.signal_clicked().connect([this] {
        stop();
        board.clear();
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

    // パターンリスト
    patternwindow.add(treeview);
    patternwindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    patternwindow.set_size_request(250, 600);

    treemodel = Gtk::ListStore::create(columns);
    treeview.set_model(treemodel);

    // パターンのファイル名
    // 事前にソートしておく
    std::vector<std::string> pattern_files;
    for (const auto &entry : std::filesystem::directory_iterator("./patterns"))
        pattern_files.push_back(entry.path());
    sort(pattern_files.begin(), pattern_files.end());

    for (const auto &file : pattern_files) {
        Gtk::TreeModel::Row row = *(treemodel->append());
        auto pattern = Pattern::load_cells(file);
        row[columns.name] = pattern.name;
        row[columns.pattern] = pattern;
    }

    treeview.append_column("Name", columns.name);
    auto column = treeview.get_column(0);
    if (column) {
        column->set_sort_column(columns.name);
    }

    treeview.get_selection()->signal_changed().connect([this] {
        auto iter = treeview.get_selection()->get_selected();
        Gtk::TreeModel::Row row = *iter;
        board_area.set_pattern(row[columns.pattern]);
    });
    set_pattern_cell();

    hbox_buttons.pack_start(button_start_or_stop);
    hbox_buttons.pack_start(button_step);
    hbox_buttons.pack_start(button_clear);
    hbox_buttons.pack_start(hbox_interval);

    hbox.pack_start(board_area);
    hbox.pack_start(patternwindow, false, false, 5);
    vbox.pack_start(hbox_buttons, false, false);
    vbox.pack_start(hbox);

    add(vbox);
}

bool MainWindow::on_key_press_event(GdkEventKey *event) {
    unsigned int modifier = event->state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK);
    if (!modifier && event->keyval == GDK_KEY_s) {
        // sキーでtoggle
        toggle();
    } else if (!modifier && event->keyval == GDK_KEY_n) {
        // nキーでstep
        board.step();
        board_area.queue_draw();
    } else if (modifier == GDK_CONTROL_MASK && event->keyval == GDK_KEY_q) {
        // ctrl+qで終了
        hide();
    } else if (!modifier && event->keyval == GDK_KEY_Escape) {
        // Escで手持ちのパターンをセルに戻す
        set_pattern_cell();
    }
    return true;
}

void MainWindow::start() {
    if (is_running)
        return;
    is_running = true;
    button_start_or_stop.set_label("stop");
    scale_interval.set_sensitive(false);
    run();
}

void MainWindow::stop() {
    if (!is_running)
        return;
    is_running = false;
    button_start_or_stop.set_label("start");
    scale_interval.set_sensitive(true);
}

void MainWindow::toggle() {
    if (!is_running) {
        start();
    } else {
        stop();
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

void MainWindow::set_pattern_cell() {
    for (const auto &iter : treemodel->children()) {
        Gtk::TreeModel::Row row = *iter;
        if (row[columns.name] == "Cell") {
            auto path = treemodel->get_path(iter);
            treeview.set_cursor(path);
            return;
        }
    }
}
