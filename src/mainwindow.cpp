#include "mainwindow.h"
#include <filesystem>
#include <fstream>
#include <glibmm/main.h>
#include <gtkmm/treemodelsort.h>
#include <iostream>

MainWindow::MainWindow()
    : board(300, 300)
    , board_area(&board)
    , vbox(Gtk::ORIENTATION_VERTICAL)
    , paned(Gtk::ORIENTATION_HORIZONTAL)
    , button_start_or_stop("start")
    , button_step("step")
    , button_clear("clear")
    , hbox_buttons(Gtk::ORIENTATION_HORIZONTAL)
    , button_grid("grid")
    , rb_edit("edit")
    , rb_select("select")
    , hbox_edit_select(Gtk::ORIENTATION_HORIZONTAL)
    , adjustment_interval(Gtk::Adjustment::create(interval, 10, 300, 10, 100))
    , label_interval("interval (ms): ")
    , scale_interval(adjustment_interval, Gtk::ORIENTATION_HORIZONTAL)
    , hbox_interval(Gtk::ORIENTATION_HORIZONTAL) {

    set_title("Life Game");
    set_default_size(800, 600);
    board_area.set_size_request(500, 600);

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
    button_grid.set_active();
    button_grid.signal_clicked().connect([this] {
        if (button_grid.get_active())
            board_area.grid_on();
        else
            board_area.grid_off();
        board_area.queue_draw();
    });

    rb_select.join_group(rb_edit);
    rb_edit.set_active();
    rb_edit.signal_clicked().connect([this] { board_area.edit_start(); });
    rb_select.signal_clicked().connect([this] { board_area.select_start(); });

    scale_interval.set_digits(0);
    scale_interval.set_value_pos(Gtk::POS_LEFT);
    scale_interval.signal_value_changed().connect([this] {
        // 10刻みにする
        interval = int(scale_interval.get_value()) / 10 * 10;
        scale_interval.set_value(interval);
    });

    // パターンリスト
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
        row[columns.size] = {pattern.col, pattern.row};
        row[columns.pattern] = pattern;
    }

    // Name
    treeview.append_column("Name", columns.name);
    // Nameのソート
    auto column_name = treeview.get_column(0);
    if (column_name) {
        column_name->set_sort_column(columns.name);
    }

    // Size
    Gtk::TreeViewColumn *view_column_size = Gtk::make_managed<Gtk::TreeViewColumn>("Size");
    treeview.append_column(*view_column_size);
    Gtk::CellRendererText *text_column = Gtk::make_managed<Gtk::CellRendererText>();
    view_column_size->pack_start(*text_column, false);
    view_column_size->set_cell_data_func(
        *text_column, sigc::mem_fun(*this, &MainWindow::on_cell_data_size_name));
    // Sizeのソート
    auto column_size = treeview.get_column(1);
    if (column_size) {
        treemodel->set_sort_func(1, sigc::mem_fun(*this, &MainWindow::on_size_compare));
        column_size->set_sort_column(columns.size);
    }

    // columnをクリックしたらパターンをセットし編集中にする
    treeview.get_selection()->signal_changed().connect([this] {
        auto iter = treeview.get_selection()->get_selected();
        Gtk::TreeModel::Row row = *iter;
        board_area.set_pattern(row[columns.pattern]);
        board_area.edit_start();
        board_area.queue_draw();
        rb_edit.set_active();
    });
    set_pattern_cell();

    hbox_edit_select.pack_start(rb_edit);
    hbox_edit_select.pack_start(rb_select);

    hbox_interval.pack_start(label_interval, false, false);
    hbox_interval.pack_start(scale_interval);

    hbox_buttons.pack_start(button_start_or_stop, false, false, 3);
    hbox_buttons.pack_start(button_step, false, false, 3);
    hbox_buttons.pack_start(button_clear, false, false, 3);
    hbox_buttons.pack_start(button_grid, false, false, 3);
    hbox_buttons.pack_start(hbox_edit_select, false, false, 3);
    hbox_buttons.pack_start(hbox_interval);

    patternwindow.add(treeview);
    patternwindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    paned.add1(board_area);
    paned.add2(patternwindow);

    vbox.pack_start(hbox_buttons, false, false);
    vbox.pack_start(paned);

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
    } else if (!modifier && event->keyval == GDK_KEY_r) {
        // rキーでrotate
        board_area.rotate();
    } else if (modifier == GDK_CONTROL_MASK && event->keyval == GDK_KEY_c) {
        // ctrl+cでコピー
        board_area.copy();
        board_area.queue_draw();
        rb_edit.set_active();
        // TODO: TreeViewのcursorをunsetしたい
    } else if (modifier == GDK_CONTROL_MASK && event->keyval == GDK_KEY_x) {
        // ctrl+xでカット
        board_area.cut();
        board_area.queue_draw();
        rb_edit.set_active();
    } else if (modifier == GDK_CONTROL_MASK && event->keyval == GDK_KEY_q) {
        // ctrl+qで終了
        hide();
    } else if (!modifier && event->keyval == GDK_KEY_Escape) {
        // Escで手持ちのパターンをセルに戻す
        set_pattern_cell();
        board_area.edit_start();
        board_area.queue_draw();
        rb_edit.set_active();
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

void MainWindow::on_cell_data_size_name(Gtk::CellRenderer *renderer,
                                        const Gtk::TreeModel::iterator &iter) {
    Gtk::TreeModel::Row row = *iter;
    std::pair<int, int> size = row[columns.size];
    std::string text_size = std::to_string(size.first) + " x " + std::to_string(size.second);
    Gtk::CellRendererText *text = dynamic_cast<Gtk::CellRendererText *>(renderer);
    if (text) {
        text->property_markup() = text_size;
    }
}

int MainWindow::on_size_compare(const Gtk::TreeModel::iterator &iter1,
                                const Gtk::TreeModel::iterator &iter2) {
    Gtk::TreeModel::Row row1 = *iter1;
    Gtk::TreeModel::Row row2 = *iter2;
    std::pair<int, int> size1 = row1[columns.size];
    std::pair<int, int> size2 = row2[columns.size];
    return size1.first * size1.second - size2.first * size2.second;
}
