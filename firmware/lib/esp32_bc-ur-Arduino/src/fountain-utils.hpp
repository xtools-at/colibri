//
//  fountain-utils.hpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#ifndef BC_UR_FOUNTAIN_UTILS_HPP
#define BC_UR_FOUNTAIN_UTILS_HPP

#include <functional>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>
#include <stdint.h>
#include "psram-allocator.hpp"
#include "xoshiro256.hpp"

namespace ur {

typedef std::set<size_t, std::less<size_t>, PSRAMAllocator<size_t>> PartIndexes;

// Return `true` if `a` is a strict subset of `b`.
template<typename T, typename C, typename A>
bool is_strict_subset(const std::set<T, C, A>& a, const std::set<T, C, A>& b) {
    if(a == b) { return false; }
    return std::includes(b.begin(), b.end(), a.begin(), a.end());
}

template<typename T, typename C, typename A>
std::set<T, C, A> set_difference(const std::set<T, C, A>& a, const std::set<T, C, A>& b) {
    std::set<T, C, A> result;
    std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(result, result.begin()));
    return result;
}

template<typename T, typename C, typename A>
bool contains(const std::set<T, C, A>& s, const T& v) {
    return s.find(v) != s.end();
}

PartIndexes choose_fragments(uint32_t seq_num, size_t seq_len, uint32_t checksum);

}

#endif // BC_UR_FOUNTAIN_UTILS_HPP
