#pragma once

#include "board_area.h"
#include <gtkmm/box.h>
#include <gtkmm/button.h>
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
    double interval = 100;

    Gtk::Box vbox, hbox;
    Gtk::Button button_start_or_stop;
    Gtk::Button button_step;

    void run_toggle();
    void run();
};
