#pragma once

#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>

class ResizeDialog : public Gtk::Dialog {
public:
    ResizeDialog(const Glib::ustring &title, bool modal = false);

    /// Entryに入力されたcolとrowを返す
    /// 値が不正だったら両方とも-1となる(optionalにすべき?)
    std::pair<int, int> get_col_row() const;

private:
    Gtk::Label label_col, label_row;
    Gtk::Entry entry_col, entry_row;
    Gtk::Grid grid;
};
