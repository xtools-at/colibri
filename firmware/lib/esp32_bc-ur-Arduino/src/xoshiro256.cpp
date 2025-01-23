//
//  xoshiro256.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include "xoshiro256.hpp"
#include <limits>
#include <cstring>
#include <mbedtls/sha256.h>

/*  Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

To the extent possible under law, the author has dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.

See <http://creativecommons.org/publicdomain/zero/1.0/>. */

/* This is xoshiro256** 1.0, one of our all-purpose, rock-solid
   generators. It has excellent (sub-ns) speed, a state (256 bits) that is
   large enough for any parallel application, and it passes all tests we
   are aware of.

   For generating just floating-point numbers, xoshiro256+ is even faster.

   The state must be seeded so that it is not everywhere zero. If you have
   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
   output to fill s. */

namespace ur {

static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

Xoshiro256::Xoshiro256(const std::array<uint8_t, 8>& a) {
    std::array<uint8_t, 32> r;
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, a.data(), 8);
    mbedtls_sha256_finish(&ctx, r.data());
    mbedtls_sha256_free(&ctx);
    for(int i = 0; i < 4; ++i) {
        auto o = i * 8;
        uint64_t v = 0;
        for(int n = 0; n < 8; ++n) {
            v <<= 8;
            v |= r[o + n];
        }
        s[i] = v;
    }
}

static inline uint64_t next(std::array<uint64_t,4>& s) {
	const uint64_t result = rotl(s[1] * 5, 7) * 9;

	const uint64_t t = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= t;

	s[3] = rotl(s[3], 45);

	return result;
}

uint64_t Xoshiro256::next_int(uint64_t low, uint64_t high) {
    return uint64_t(next_double() * (high - low + 1)) + low;
}

double Xoshiro256::next_double() {
    const auto m = ((double)std::numeric_limits<uint64_t>::max()) + 1;
    return next(s) / m;
}

}
