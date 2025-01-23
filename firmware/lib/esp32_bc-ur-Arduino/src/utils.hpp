//
//  utils.hpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdint.h>
#include <vector>
#include <utility>
#include <string>
#include <array>
#include <cassert>

#include "psram-allocator.hpp"

namespace ur {

using ByteVector = std::vector<uint8_t, PSRAMAllocator<uint8_t>>;
using ByteVectorVector = std::vector<ByteVector, PSRAMAllocator<ByteVector>>;
using StringVector = std::vector<std::string, PSRAMAllocator<std::string>>;

std::string join(const StringVector &strings, const std::string &separator);
StringVector split(const std::string& s, char separator);

StringVector partition(const std::string& string, size_t size);

template<typename T, typename A>
void append(std::vector<T, A>& target, const std::vector<T, A>& source) {
    target.insert(target.end(), source.begin(), source.end());
}

template<typename T, typename A, size_t N>
void append(std::vector<T, A>& target, const std::array<T, N>& source) {
    target.insert(target.end(), source.begin(), source.end());
}

template<typename T, typename A1, typename A2>
std::vector<T, A1> join(const std::vector<std::vector<T, A1>, A2>& parts) {
    size_t total_size = 0;
    for (const auto& part : parts) {
        total_size += part.size();
    }

    std::vector<T, A1> result;
    result.reserve(total_size);

    for (const auto& part : parts) {
        append(result, part);
    }

    return result;
}

template<typename T, typename A>
std::pair<std::vector<T, A>, std::vector<T, A>> split(const std::vector<T, A>& buf, size_t count) {
    const auto split_point = buf.begin() + std::min(buf.size(), count);
    return {
        std::vector<T, A>(buf.begin(), split_point),
        std::vector<T, A>(split_point, buf.end())
    };
}

template<typename T, typename A>
std::vector<T, A> take_first(const std::vector<T, A> &buf, size_t count) {
    const auto first = buf.begin();
    return std::vector<T, A>(first, first + std::min(buf.size(), count));
}

bool is_ur_type(char c);
bool is_ur_type(const std::string& s);

bool has_prefix(const std::string& s, const std::string& prefix);

}

#endif // UTILS_HPP
