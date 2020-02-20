#pragma once

#include <string>
#include <vector>

class Pattern {
public:
    std::string name;
    std::vector<std::vector<bool>> pattern;
    std::vector<std::string> comment;
    int col, row;

    Pattern()
        : name("Cell")
        , pattern({{1}})
        , col(1)
        , row(1) {}

    Pattern(const std::string &name, const std::vector<std::vector<bool>> &pattern,
            const std::vector<std::string> &comment, int col, int row)
        : name(name)
        , pattern(pattern)
        , comment(comment)
        , col(col)
        , row(row) {}

    /// ファイル読み込み
    static Pattern load(const std::string &file);
};
