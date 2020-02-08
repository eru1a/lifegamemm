#include "mainwindow.h"
#include <gtkmm/application.h>

int main(int argc, char *argv[]) {
    auto app = Gtk::Application::create(argc, argv);
    MainWindow win;
    win.show_all_children();
    return app->run(win);
}
