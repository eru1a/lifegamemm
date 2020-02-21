#include "resize_dialog.h"
#include <algorithm>

ResizeDialog::ResizeDialog(const Glib::ustring &title, bool modal)
    : Gtk::Dialog(title, modal)
    , label_col("col: ")
    , label_row("row: ") {

    grid.attach(label_col, 0, 0, 1, 1);
    grid.attach(entry_col, 1, 0, 1, 1);
    grid.attach(label_row, 0, 1, 1, 1);
    grid.attach(entry_row, 1, 1, 1, 1);

    get_vbox()->add(grid);

    add_button("Ok", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);

    show_all_children();
}

bool is_digits(const std::string &str) { return std::all_of(str.begin(), str.end(), isdigit); }

std::pair<int, int> ResizeDialog::get_col_row() const {
    std::string str_col = entry_col.get_text();
    std::string str_row = entry_row.get_text();
    if (!is_digits(str_col) || !is_digits(str_row))
        return {-1, -1};
    int col = std::stoi(str_col);
    int row = std::stoi(str_row);
    if (col >= 0 && row >= 0)
        return {col, row};
    else
        return {-1, -1};
}
