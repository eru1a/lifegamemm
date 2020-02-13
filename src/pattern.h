#pragma once

#include <string>
#include <vector>

class Pattern {
public:
    std::string name;
    std::vector<std::string> comment;
    std::vector<std::vector<bool>> pattern;

    Pattern()
        : name("Cell")
        , pattern({{1}}) {}

    /// .celssファイル読み込み
    static Pattern load_cells(const std::string &file);
};
