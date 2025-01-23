//
//  xoshiro256.hpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#ifndef XOSHIRO256_HPP
#define XOSHIRO256_HPP

#include <cstdint>
#include <array>
#include <string>
#include "utils.hpp"

namespace ur {

class Xoshiro256 {
public:
    explicit Xoshiro256(const std::array<uint8_t, 8>& a);

    double next_double();
    uint64_t next_int(uint64_t low, uint64_t high);

private:
    std::array<uint64_t,4> s;
};

}

#endif // XOSHIRO256_HPP
