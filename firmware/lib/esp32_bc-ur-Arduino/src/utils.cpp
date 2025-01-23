//
//  utils.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include "utils.hpp"

#include <array>
#include <vector>
#include <algorithm>
#include <cctype>

using namespace std;

namespace ur {

string join(const StringVector &strings, const string &separator) {
    string result;
    size_t total_size = 0;
    for (const auto& s : strings) {
        total_size += s.size();
    }
    if (!strings.empty()) {
        result.reserve(total_size
                        + (separator.size() * (strings.size() - 1))
                        + 1);  // extra 1 in case of nul-terminator
    }
    bool first = true;
    for (const auto& s : strings) {
        if (!first) {
            result += separator;
        }
        result += s;
        first = false;
    }
    return result;
}

StringVector split(const string& s, char separator) {
    StringVector result;

    size_t start = 0;
    size_t end = 0;

    while ((end = s.find(separator, start)) != string::npos) {
        if (end != start) {
            result.emplace_back(s.substr(start, end - start));
        }
        start = end + 1;
    }

    if (start < s.size()) {
        result.emplace_back(s.substr(start));
    }

    return result;
}

StringVector partition(const string& s, size_t size) {
    StringVector result;
    result.reserve((s.length() + size - 1) / size);

    auto start = s.begin();
    auto end = s.end();

    while (start < end) {
        auto next = (start + size < end) ? start + size : end;
        result.emplace_back(start, next);
        start = next;
    }

    return result;
}

bool is_ur_type(char c) {
    return ('a' <= c && c <= 'z') || ('0' <= c && c <= '9') || (c == '-');
}

bool is_ur_type(const string& s) {
    return none_of(s.begin(), s.end(), [](auto c) { return !is_ur_type(c); });
}

bool has_prefix(const string& s, const string& prefix) {
    return s.compare(0, prefix.size(), prefix) == 0;
}

}
