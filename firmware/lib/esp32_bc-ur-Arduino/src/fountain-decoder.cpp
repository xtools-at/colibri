//
//  fountain-decoder.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include "fountain-decoder.hpp"
#include <utility>
#include <algorithm>
#include <string>
#include <cmath>
#include <numeric>
#include <esp_crc.h>

using namespace std;

namespace ur {

FountainDecoder::FountainDecoder() { }

FountainDecoder::Part::Part(const FountainEncoder::Part& p)
    : indexes_(choose_fragments(p.seq_num(), p.seq_len(), p.checksum()))
    , data_(p.data())
{
}

FountainDecoder::Part::Part(PartIndexes& indexes, ByteVector& data)
    : indexes_(indexes)
    , data_(data)
{
}

const ByteVector FountainDecoder::join_fragments(const ByteVectorVector& fragments, size_t message_len) {
    auto message = join(fragments);
    return take_first(message, message_len);
}

double FountainDecoder::estimated_percent_complete() const {
    if(is_complete()) return 1;
    if(!_expected_part_indexes.has_value()) return 0;
    auto estimated_input_parts = expected_part_count() * 1.75;
    return min(0.99, processed_parts_count_ / estimated_input_parts);
}

bool FountainDecoder::receive_part(FountainEncoder::Part& encoder_part) {
    // Don't process the part if we're already done
    if(is_complete()) return false;

    // Don't continue if this part doesn't validate
    if(!validate_part(encoder_part)) return false;

    // Add this part to the queue
    auto p = Part(encoder_part);
    last_part_indexes_ = p.indexes();
    enqueue(p);

    // Process the queue until we're done or the queue is empty
    while(!is_complete() && !_queued_parts.empty()) {
        process_queue_item();
    }

    // Keep track of how many parts we've processed
    ++processed_parts_count_;

    return true;
}

void FountainDecoder::enqueue(Part &&p) {
    _queued_parts.push_back(p);
}

void FountainDecoder::enqueue(const Part &p) {
    _queued_parts.push_back(p);
}

void FountainDecoder::process_queue_item() {
    auto part = _queued_parts.front();
    _queued_parts.pop_front();
    if(part.is_simple()) {
        process_simple_part(part);
    } else {
        process_mixed_part(part);
    }
}

void FountainDecoder::reduce_mixed_by(const Part& p) {
    PartVector reduced_parts;
    reduced_parts.reserve(_mixed_parts.size());

    for (auto it = _mixed_parts.begin(); it != _mixed_parts.end(); ++it) {
        reduced_parts.push_back(reduce_part_by_part(it->second, p));
    }

    PartDict new_mixed;

    for (auto& reduced_part : reduced_parts) {
        if (reduced_part.is_simple()) {
            enqueue(reduced_part);
        } else {
            new_mixed.emplace(reduced_part.indexes(), move(reduced_part));
        }
    }

    _mixed_parts = move(new_mixed);
}


FountainDecoder::Part FountainDecoder::reduce_part_by_part(const Part& a, const Part& b) const {
    if (is_strict_subset(b.indexes(), a.indexes())) {
        auto new_indexes = set_difference(a.indexes(), b.indexes());

        ByteVector new_data = a.data();
        const auto& s = b.data();
        const size_t count = new_data.size();

        for (size_t i = 0; i < count; ++i) {
            new_data[i] ^= s[i];
        }

        return Part(new_indexes, new_data);
    } else {
        return a;
    }
}

void FountainDecoder::process_simple_part(Part& p) {
    // Don't process duplicate parts
    auto fragment_index = p.index();
    if (received_part_indexes_.find(fragment_index) != received_part_indexes_.end()) return;

    // Record this part
    _simple_parts.emplace(p.indexes(), p);
    received_part_indexes_.insert(fragment_index);

    // If we've received all the parts
    if (received_part_indexes_ == _expected_part_indexes) {
        // Reassemble the message from its fragments
        PartVector sorted_parts;
        sorted_parts.reserve(_simple_parts.size());
        for (const auto& elem : _simple_parts) {
            sorted_parts.push_back(elem.second);
        }

        sort(sorted_parts.begin(), sorted_parts.end(),
            [](const Part& a, const Part& b) -> bool {
                return a.index() < b.index();
            }
        );

        ByteVectorVector fragments;
        fragments.reserve(sorted_parts.size());
        for (const auto& part : sorted_parts) {
            fragments.push_back(part.data());
        }

        auto message = join_fragments(fragments, *_expected_message_len);

        // Verify the message checksum and note success or failure
        auto checksum = esp_crc32_le(0, message.data(), message.size());
        if (checksum == _expected_checksum) {
            result_ = move(message);
        } else {
            result_ = InvalidChecksum();
        }
    } else {
        // Reduce all the mixed parts by this part
        reduce_mixed_by(p);
    }
}

void FountainDecoder::process_mixed_part(const Part& p) {
    // Don't process duplicate parts
    if (any_of(_mixed_parts.begin(), _mixed_parts.end(), [&](const auto& r) { return r.first == p.indexes(); })) {
        return;
    }
    // Reduce this part by all the others
    Part p2 = p;
    for (const auto& r : _simple_parts) {
        p2 = reduce_part_by_part(p2, r.second);
    }
    for (const auto& r : _mixed_parts) {
        p2 = reduce_part_by_part(p2, r.second);
    }

    // If the part is now simple
    if (p2.is_simple()) {
        // Add it to the queue
        enqueue(p2);
    } else {
        // Reduce all the mixed parts by this one
        reduce_mixed_by(p2);
        // Record this new mixed part
        _mixed_parts.emplace(p2.indexes(), p2);
    }
}

bool FountainDecoder::validate_part(const FountainEncoder::Part& p) {
    if (!p.is_valid()) { return false; }

    // If this is the first part we've seen
    if(!_expected_part_indexes.has_value()) {
        // Record the things that all the other parts we see will have to match to be valid.
        _expected_part_indexes = PartIndexes();
        for(size_t i = 0; i < p.seq_len(); ++i) { _expected_part_indexes->insert(i); }
        _expected_message_len = p.message_len();
        _expected_checksum = p.checksum();
        _expected_fragment_len = p.data().size();
    } else {
        // If this part's values don't match the first part's values, throw away the part
        if(expected_part_count() != p.seq_len()) return false;
        if(_expected_message_len != p.message_len()) return false;
        if(_expected_checksum != p.checksum()) return false;
        if(_expected_fragment_len != p.data().size()) return false;
    }
    // This part should be processed
    return true;
}
}
