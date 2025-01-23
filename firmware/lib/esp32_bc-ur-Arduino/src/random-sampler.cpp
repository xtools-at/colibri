//
//  random-sampler.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include "random-sampler.hpp"
#include <numeric>
#include <algorithm>
#include <cassert>

using namespace std;

namespace ur {

RandomSampler::RandomSampler(std::vector<double> probs) {
    for(const auto& p : probs) {
        assert(p >= 0);
    }

    // Normalize given probabilities
    double sum = std::accumulate(probs.begin(), probs.end(), 0.0);
    assert(sum > 0);

    size_t n = probs.size();
    std::vector<double> P(n);
    std::transform(probs.begin(), probs.end(), P.begin(), [&](double d) { return d * double(n) / sum; });

    std::vector<int> S;
    std::vector<int> L;
    S.reserve(n);
    L.reserve(n);

    // Set separate index lists for small and large probabilities:
    for(size_t i = n; i-- > 0;) {
        // at variance from Schwarz, we reverse the index order
        if(P[i] < 1) {
            S.push_back(i);
        } else {
            L.push_back(i);
        }
    }

    // Work through index lists
    std::vector<double> _probs(n, 0);
    std::vector<int> _aliases(n, 0);
    while(!S.empty() && !L.empty()) {
        auto a = S.back(); S.pop_back(); // Schwarz's l
        auto g = L.back(); L.pop_back(); // Schwarz's g
        _probs[a] = P[a];
        _aliases[a] = g;
        P[g] += P[a] - 1;
        if(P[g] < 1) {
            S.push_back(g);
        } else {
            L.push_back(g);
        }
    }

    while(!L.empty()) {
        _probs[L.back()] = 1;
        L.pop_back();
    }

    while(!S.empty()) {
        // can only happen through numeric instability
        _probs[S.back()] = 1;
        S.pop_back();
    }

    this->probs_ = std::move(_probs);
    this->aliases_ = std::move(_aliases);
}

int RandomSampler::next(std::function<double()> rng) {
    auto r1 = rng();
    auto r2 = rng();
    auto n = probs_.size();
    auto i = int(double(n) * r1);
    return r2 < probs_[i] ? i : aliases_[i];
}

}
