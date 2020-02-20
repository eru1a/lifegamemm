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

// ある文字まで読み込む
void read_while(std::istream &buf, char ch) {
    char c;
    while (buf >> c) {
        if (c == ch)
            break;
    }
}

Pattern load_cells(const std::string &file) {
    std::ifstream ifs(file);
    assert(ifs);

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
        pattern[i].resize(col);

    return Pattern(name, pattern, comment, col, row);
}

Pattern load_rle(const std::string &file) {
    std::ifstream ifs(file);
    assert(ifs);

    std::filesystem::path path = file;
    // #Nが含まれていなければファイル名をnameとする
    std::string name = path.stem();

    int col = 0;
    int row = 0;
    std::vector<std::string> comment;

    std::string line;

    // header
    while (std::getline(ifs, line) && starts_with(line, "#")) {
        if (starts_with(line, "#N")) {
            name = line.substr(2);
            trim(name);
        } else if (starts_with(line, "#C") || starts_with(line, "#c")) {
            std::string comment1 = line.substr(2);
            trim(comment1);
            comment.push_back(comment1);
        }
        // 他は無視
    }

    // x, y
    if (starts_with(line, "x")) {
        std::stringstream ss(line);
        read_while(ss, '=');
        ss >> col;
        // 次にyが来るのを前提にしてるけど問題あるかも
        read_while(ss, '=');
        ss >> row;
        // ruleは無視
    }

    // board
    int cur_col = 0;
    int cur_row = 0;
    std::vector<std::vector<bool>> pattern(row, std::vector<bool>(col, false));
    while (!ifs.eof()) {
        int cnt = 1;
        if (isdigit(ifs.peek())) {
            ifs >> cnt;
        }
        char ch = ifs.get();
        if (ch == 'o') {
            // 生きているセル
            for (int i = 0; i < cnt; i++) {
                pattern[cur_row][cur_col + i] = true;
            }
            cur_col += cnt;
        } else if (ch == 'b') {
            // 死んでいるセル
            cur_col += cnt;
        } else if (ch == '$') {
            // 改行($の前に数字が来ることもあるっぽい)
            cur_row += cnt;
            cur_col = 0;
        } else if (ch == '!') {
            // 終了
            break;
        }
    }

    return Pattern(name, pattern, comment, col, row);
}

Pattern Pattern::load(const std::string &file) {
    if (ends_with(file, ".cells")) {
        return load_cells(file);
    } else if (ends_with(file, ".rle")) {
        return load_rle(file);
    } else {
        assert(false);
    }
}
