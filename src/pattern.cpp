#include "pattern.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>

bool starts_with(const std::string &s, const std::string &prefix) {
    size_t size = prefix.size();
    if (s.size() < size)
        return false;
    return std::equal(prefix.begin(), prefix.end(), s.begin());
}

bool ends_with(const std::string &s, const std::string &suffix) {
    if (s.size() < suffix.size())
        return false;
    return std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

Pattern Pattern::load_cells(const std::string &file) {
    std::ifstream ifs(file);
    assert(ifs);
    assert(ends_with(file, ".cells"));

    std::filesystem::path path = file;
    // !Nnameが含まれていなければファイル名をnameとする
    std::string name = path.stem();
    std::vector<std::string> comment;
    std::vector<std::vector<bool>> pattern;

    std::string line;
    while (std::getline(ifs, line)) {
        if (starts_with(line, "!Name: ")) {
            name = line.substr(7);
        } else if (starts_with(line, "!")) {
            // 空白コメントは無視
            if (line != "!")
                comment.push_back(line.substr(2));
        } else {
            std::vector<bool> row;
            for (auto c : line) {
                if (c == '.') {
                    row.push_back(false);
                } else {
                    row.push_back(true);
                }
            }
            pattern.push_back(row);
        }
    }

    // もっといい方法ある？
    Pattern p = Pattern{};
    p.name = name;
    p.comment = comment;
    p.pattern = pattern;

    return p;
}
