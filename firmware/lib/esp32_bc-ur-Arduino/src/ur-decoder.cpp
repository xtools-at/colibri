//
//  ur-decoder.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include "ur-decoder.hpp"
#include "bytewords.hpp"
#include "utils.hpp"

using namespace std;

extern "C" {

void urcreate_decoder(void** const decoder) {
    assert(decoder && !*decoder);
    *decoder = new(nothrow) ur::URDecoder();
    assert(*decoder);
}

void urcreate_placement_decoder(void* const decoder, size_t decoder_len) {
    assert(decoder && decoder_len == sizeof(ur::URDecoder));
    void* tmp = new(decoder) ur::URDecoder();
    assert(tmp);
}

void urfree_decoder(void* const decoder) {
    if (decoder) {
        ur::URDecoder* urdecoder = (ur::URDecoder*) decoder;
        delete urdecoder;
    }
}

void urfree_placement_decoder(void* const decoder) {
    assert(decoder);
    ur::URDecoder* urdecoder = (ur::URDecoder*) decoder;
    urdecoder->~URDecoder();
}

bool urreceive_part_decoder(void* const decoder, const char* s) {
    assert(decoder);
    ur::URDecoder* urdecoder = (ur::URDecoder*) decoder;
    const string part(s);
    return urdecoder->receive_part(part);
}

size_t urprocessed_parts_count_decoder(void* const decoder) {
    assert(decoder);
    ur::URDecoder* urdecoder = (ur::URDecoder*) decoder;
    return urdecoder->processed_parts_count();
}

size_t urreceived_parts_count_decoder(void* const decoder) {
    assert(decoder);
    ur::URDecoder* urdecoder = (ur::URDecoder*) decoder;
    return urdecoder->received_part_indexes().size();
}

size_t urexpected_part_count_decoder(void* const decoder) {
    assert(decoder);
    ur::URDecoder* urdecoder = (ur::URDecoder*) decoder;
    return urdecoder->expected_part_count();
}

double urestimated_percent_complete_decoder(void* const decoder) {
    assert(decoder);
    ur::URDecoder* urdecoder = (ur::URDecoder*) decoder;
    return urdecoder->estimated_percent_complete();
}

bool uris_success_decoder(void* const decoder) {
    assert(decoder);
    ur::URDecoder* urdecoder = (ur::URDecoder*) decoder;
    return urdecoder->is_success();
}

bool uris_failure_decoder(void* const decoder) {
    assert(decoder);
    ur::URDecoder* urdecoder = (ur::URDecoder*) decoder;
    return urdecoder->is_failure();
}

bool uris_complete_decoder(void* const decoder) {
    assert(decoder);
    ur::URDecoder* urdecoder = (ur::URDecoder*) decoder;
    return urdecoder->is_complete();
}

void urresult_ur_decoder(void* const decoder, uint8_t** result, size_t* result_len, const char** type) {
    assert(decoder);
    ur::URDecoder* urdecoder = (ur::URDecoder*) decoder;
    const ur::UR& ur = urdecoder->result_ur();
    (*type) = ur.type().c_str();
    const ur::ByteVector& res = ur.cbor();
    (*result) = (uint8_t*)res.data();
    (*result_len) = res.size();
}

}

namespace ur {

UR URDecoder::decode(const string& s) {
    auto [type, components] = parse(s);
    auto body = !components.empty() ? components.front() : string();
    return decode(type, move(body));
}

URDecoder::URDecoder() { }

UR URDecoder::decode(const string& type, const string& body) {
    auto cbor = Bytewords::decode(Bytewords::style::minimal, body);
    return UR(type, cbor);
}

pair<string, StringVector> URDecoder::parse(const string& s) {
    // Don't consider case
    string lowered;
    lowered.resize(s.size());
    transform(s.begin(), s.end(), lowered.begin(), [](unsigned char c){ return tolower(c); });

    // Validate URI scheme
    if(!has_prefix(lowered, "ur:") || 3 >= lowered.length()) {
       return {};
    }

    // Split the remainder into path components
    auto components = split(lowered.substr(3), '/');

    // Make sure there are at least two path components
    if(components.size() < 2) {
       return pair(string(), StringVector());
    }
    // Validate the type
    auto type = components.front();
    if(!is_ur_type(type)) {
       return {};
    }

    auto comps = StringVector(components.begin() + 1, components.end());
    return pair(type, comps);
}

pair<uint32_t, size_t> URDecoder::parse_sequence_component(const string& s) {
   auto comps = split(s, '-');
   if(comps.size() != 2) {
       return pair(0,0);
   }
   uint32_t seq_num = stoul(comps[0]);
   size_t seq_len = stoul(comps[1]);
   if(seq_num < 1 || seq_len < 1) {
       return pair(0,0);
   }
   return pair(seq_num, seq_len);
}

bool URDecoder::validate_part(const string& type) {
    if(!expected_type_.has_value()) {
        if(!is_ur_type(type)) return false;
        expected_type_ = type;
        return true;
    } else {
        return type == expected_type_;
    }
}

bool URDecoder::receive_part(const string& s) {
    // Don't process the part if we're already done
    if(result_.has_value()) return false;

    // Don't continue if this part doesn't validate
    auto [type, components] = parse(s);
    if(!validate_part(type)) return false;

    // If this is a single-part UR then we're done
    if(components.size() == 1) {
        auto body = components.front();
        const UR result = decode(type, body);
        if (!result.is_valid()) {
            return false;
        }

        result_ = result;
        return true;
    }

    // Multi-part URs must have two path components: seq/fragment
    if(components.size() != 2) {
        return false;
    }
    auto seq = components[0];
    auto fragment = components[1];

    // Parse the sequence component and the fragment, and
    // make sure they agree.
    auto [seq_num, seq_len] = parse_sequence_component(seq);
    if (!seq_num || !seq_len) {
        return false;
    }
    auto cbor = Bytewords::decode(Bytewords::style::minimal, fragment);
    if (cbor.empty()) {
        return false;
    }
    auto part = FountainEncoder::Part(cbor);
    if(!part.is_valid() || seq_num != part.seq_num() || seq_len != part.seq_len()) return false;

    // Process the part
    if(!fountain_decoder.receive_part(part)) return false;

    if(fountain_decoder.is_success()) {
        const UR result(type, fountain_decoder.result_message());
        assert(result.is_valid());
        result_ = result;
    } else if(fountain_decoder.is_failure()) {
        result_ = fountain_decoder.result_error();
    }

    return true;
}

}
