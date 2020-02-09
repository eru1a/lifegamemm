#pragma once

#include "board_area.h"
#include <gtkmm/adjustment.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>
#include <gtkmm/scale.h>
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

    Gtk::Box vbox, hbox;
    Gtk::Button button_start_or_stop;
    Gtk::Button button_step;

    int interval = 100;
    Glib::RefPtr<Gtk::Adjustment> adjustment_interval;
    Gtk::Label label_interval;
    Gtk::Scale scale_interval;
    Gtk::Box hbox_interval;

    void run_toggle();
    void run();
};
