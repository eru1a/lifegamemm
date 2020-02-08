#pragma once

#include "board_area.h"
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
    void run();
    double interval = 100;
};
