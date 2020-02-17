#pragma once

#include "board_area.h"
#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/paned.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/scale.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treeview.h>
#include <gtkmm/window.h>

class MainWindow : public Gtk::Window {
public:
    MainWindow();
    virtual ~MainWindow() {}

private:
    bool on_key_press_event(GdkEventKey *event) override;

private:
    Board board;
    BoardArea board_area;
    bool is_running = false;

    Gtk::Box vbox;
    Gtk::Paned paned;

    // ボタン関連
    Gtk::Button button_start_or_stop;
    Gtk::Button button_step;
    Gtk::Button button_clear;
    Gtk::Box hbox_buttons;

    // edit, select
    Gtk::RadioButton rb_edit;
    Gtk::RadioButton rb_select;
    Gtk::Box hbox_edit_select;

    // 速度関連
    int interval = 50;
    Glib::RefPtr<Gtk::Adjustment> adjustment_interval;
    Gtk::Label label_interval;
    Gtk::Scale scale_interval;
    Gtk::Box hbox_interval;

    // リスト関連
    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns() {
            add(name);
            add(size);
            add(pattern);
        }
        Gtk::TreeModelColumn<Glib::ustring> name;
        Gtk::TreeModelColumn<std::pair<int, int>> size;
        Gtk::TreeModelColumn<Pattern> pattern;
    };

    ModelColumns columns;
    Gtk::ScrolledWindow patternwindow;
    Gtk::TreeView treeview;
    Glib::RefPtr<Gtk::ListStore> treemodel;

    void start();
    void stop();
    void toggle();
    void run();

    void set_pattern_cell();
    void on_cell_data_size_name(Gtk::CellRenderer *renderer, const Gtk::TreeModel::iterator &iter);
    int on_size_compare(const Gtk::TreeModel::iterator &iter1,
                        const Gtk::TreeModel::iterator &iter2);
};
