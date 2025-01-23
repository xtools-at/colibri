//
//  fountain-utils.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include "fountain-utils.hpp"
#include "random-sampler.hpp"
#include "utils.hpp"
#include <limits>
using namespace std;

namespace ur {

static inline size_t choose_degree(size_t seq_len, Xoshiro256& rng) {
    vector<double> degree_probabilities;
    for(int i = 1; i <= seq_len; ++i) {
        degree_probabilities.push_back(1.0 / i);
    }
    auto degree_chooser = RandomSampler(degree_probabilities);
    return degree_chooser.next([&]() { return rng.next_double(); }) + 1;
}

PartIndexes choose_fragments(uint32_t seq_num, size_t seq_len, uint32_t checksum) {
    // The first `seq_len` parts are the "pure" fragments, not mixed with any
    // others. This means that if you only generate the first `seq_len` parts,
    // then you have all the parts you need to decode the message.
    if(seq_num <= seq_len) {
        return PartIndexes({seq_num - 1});
    } else {
        std::array<uint8_t, 8> seed;
        seed[0] = (seq_num >> 24) & 0xff;
        seed[1] = (seq_num >> 16) & 0xff;
        seed[2] = (seq_num >> 8) & 0xff;
        seed[3] = seq_num & 0xff;
        seed[4] = (checksum >> 24) & 0xff;
        seed[5] = (checksum >> 16) & 0xff;
        seed[6] = (checksum >> 8) & 0xff;
        seed[7] = checksum & 0xff;

        auto rng = Xoshiro256(seed);
        const auto degree = choose_degree(seq_len, rng);

        vector<size_t> indexes;
        indexes.reserve(seq_len);

        for(size_t i = 0; i < seq_len; ++i) {
            indexes.push_back(i);
        }

        // Fisher-Yates shuffle
        std::vector<size_t> shuffled_indexes;
        shuffled_indexes.reserve(degree);
        while(shuffled_indexes.size() != degree) {
            const auto index = rng.next_int(0, indexes.size() - 1);
            const auto item = indexes[index];
            indexes.erase(indexes.begin() + index);
            shuffled_indexes.push_back(item);
        }

        return PartIndexes(shuffled_indexes.begin(), shuffled_indexes.begin() + degree);
    }
}

}
