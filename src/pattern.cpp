#include "pattern.h"
#include <algorithm>
#include <cassert>
#include <filesystem>
#include <fstream>

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

void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
}

void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(),
            s.end());
}

void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
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
    int col = 0;
    while (std::getline(ifs, line)) {
        trim(line);
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
                } else if (c == 'O') {
                    row.push_back(true);
                }
            }
            col = std::max(col, int(row.size()));
            pattern.push_back(row);
        }
    }
    int row = pattern.size();
    for (int i = 0; i < row; i++)
        pattern.resize(col);

    return Pattern(name, pattern, comment, row, col);
}
