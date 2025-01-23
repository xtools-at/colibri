//
//  ur-encoder.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include "ur-encoder.hpp"
#include "bytewords.hpp"
#include "utils.hpp"
#include <cstring>

extern "C" {

void urcreate_encoder(void** const encoder, const char* type, const uint8_t* cbor, size_t cbor_len, size_t max_fragment_len, uint32_t first_seq_num, size_t min_fragment_len) {
    assert(encoder && !*encoder && type && ur::is_ur_type(type) && cbor && cbor_len && max_fragment_len && min_fragment_len && max_fragment_len > min_fragment_len);
    ur::ByteVector bv(cbor, cbor + cbor_len);
    ur::UR ur(type, bv);
    *encoder = new(std::nothrow) ur::UREncoder(ur, max_fragment_len, first_seq_num, min_fragment_len);
    assert(*encoder);
}

void urcreate_placement_encoder(void* const encoder, size_t encoder_len, const char* type, const uint8_t* cbor, size_t cbor_len, size_t max_fragment_len, uint32_t first_seq_num, size_t min_fragment_len) {
    assert(encoder && encoder_len == sizeof(ur::UREncoder) && type && ur::is_ur_type(type) && cbor && cbor_len && max_fragment_len && min_fragment_len && max_fragment_len > min_fragment_len);
    ur::ByteVector bv(cbor, cbor + cbor_len);
    ur::UR ur(type, bv);
    void* tmp = new(encoder) ur::UREncoder(ur, max_fragment_len, first_seq_num, min_fragment_len);
    assert(tmp);
}

void urfree_encoder(void* const encoder) {
    if (encoder) {
        ur::UREncoder* urencoder = (ur::UREncoder*) encoder;
        delete urencoder;
    }
}

void urfree_placement_encoder(void* const encoder) {
    assert(encoder);
    ur::UREncoder* urencoder = (ur::UREncoder*) encoder;
    urencoder->~UREncoder();
}

uint32_t urseqnum_encoder(void* const encoder)
{
    assert(encoder);
    ur::UREncoder* urencoder = (ur::UREncoder*) encoder;
    return urencoder->seq_num();
}

uint32_t urseqlen_encoder(void* const encoder)
{
    assert(encoder);
    ur::UREncoder* urencoder = (ur::UREncoder*) encoder;
    return urencoder->seq_len();
}

bool uris_complete_encoder(void* const encoder) {
    assert(encoder);
    ur::UREncoder* urencoder = (ur::UREncoder*) encoder;
    return urencoder->is_complete();
}

bool uris_single_part_encoder(void* const encoder) {
    assert(encoder);
    ur::UREncoder* urencoder = (ur::UREncoder*) encoder;
    return urencoder->is_single_part();
}

void urnext_part_encoder(void* const encoder, const bool force_uppercase, char**next) {
    assert(encoder);
    ur::UREncoder* urencoder = (ur::UREncoder*) encoder;
    std::string encoded = urencoder->next_part();
    (*next) = (char*)malloc(encoded.size() + 1);
    if (force_uppercase) {
        std::transform(encoded.begin(), encoded.end(), *next, ::toupper);
        (*next)[encoded.size()] = '\0';  // ensure terminated
    } else {
        strlcpy(*next, encoded.c_str(), encoded.size() + 1);
    }
}

void urfree_encoded_encoder(char * encoded) {
    free(encoded);
}

}


using namespace std;

namespace ur {

string UREncoder::encode(const UR& ur) {
    auto body = Bytewords::encode(Bytewords::style::minimal, ur.cbor());
    return encode_ur({ur.type(), body});
}

UREncoder::UREncoder(const UR& ur, size_t max_fragment_len, uint32_t first_seq_num, size_t min_fragment_len)
    : ur_(ur),
    fountain_encoder_(FountainEncoder(ur.cbor(), max_fragment_len, first_seq_num, min_fragment_len))
{
}

std::string UREncoder::next_part() {
    auto part = fountain_encoder_.next_part();
    if(is_single_part()) {
        return encode(ur_);
    } else {
        return encode_part(ur_.type(), part);
    }
}

string UREncoder::encode_part(const string& type, const FountainEncoder::Part& part) {
    auto seq = to_string(part.seq_num()) + "-" + to_string(part.seq_len());
    auto body = Bytewords::encode(Bytewords::style::minimal, part.cbor());
    return encode_ur({type, seq, body});
}

string UREncoder::encode_uri(const string& scheme, const StringVector& path_components) {
    auto path = join(path_components, "/");
    return join({scheme, path}, ":");
}

string UREncoder::encode_ur(const StringVector& path_components) {
    return encode_uri("ur", path_components);
}

}
