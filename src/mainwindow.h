#pragma once

#include "board_area.h"
#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
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

    Gtk::Box vbox, hbox, hbox_buttons;
    Gtk::Button button_start_or_stop;
    Gtk::Button button_step;
    Gtk::Button button_clear;

    int interval = 100;
    Glib::RefPtr<Gtk::Adjustment> adjustment_interval;
    Gtk::Label label_interval;
    Gtk::Scale scale_interval;
    Gtk::Box hbox_interval;

    class ModelColumns : public Gtk::TreeModel::ColumnRecord {
    public:
        ModelColumns() {
            add(name);
            add(pattern);
        }
        Gtk::TreeModelColumn<std::string> name;
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
};
