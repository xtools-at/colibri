//
//  bytewords.cpp
//
//  Copyright © 2020 by Blockchain Commons, LLC
//  Licensed under the "BSD-2-Clause Plus Patent License"
//

#include "bytewords.hpp"
#include "utils.hpp"
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <esp_crc.h>

namespace ur {

using namespace std;

static const char* bytewords = "ableacidalsoapexaquaarchatomauntawayaxisbackbaldbarnbeltbetabiasbluebodybragbrewbulbbuzzcalmcashcatschefcityclawcodecolacookcostcruxcurlcuspcyandarkdatadaysdelidicedietdoordowndrawdropdrumdulldutyeacheasyechoedgeepicevenexamexiteyesfactfairfernfigsfilmfishfizzflapflewfluxfoxyfreefrogfuelfundgalagamegeargemsgiftgirlglowgoodgraygrimgurugushgyrohalfhanghardhawkheathelphighhillholyhopehornhutsicedideaidleinchinkyintoirisironitemjadejazzjoinjoltjowljudojugsjumpjunkjurykeepkenokeptkeyskickkilnkingkitekiwiknoblamblavalazyleaflegsliarlimplionlistlogoloudloveluaulucklungmainmanymathmazememomenumeowmildmintmissmonknailnavyneednewsnextnoonnotenumbobeyoboeomitonyxopenovalowlspaidpartpeckplaypluspoempoolposepuffpumapurrquadquizraceramprealredorichroadrockroofrubyruinrunsrustsafesagascarsetssilkskewslotsoapsolosongstubsurfswantacotasktaxitenttiedtimetinytoiltombtoystriptunatwinuglyundouniturgeuservastveryvetovialvibeviewvisavoidvowswallwandwarmwaspwavewaxywebswhatwhenwhizwolfworkyankyawnyellyogayurtzapszerozestzinczonezoom";


static const int16_t _lookup[] = {
    4, 14, 29, 37, -1, -1, 73, -1, 99, -1, -1, 128, -1, -1, -1, 177,
    -1, -1, 194, 217, -1, 230, -1, -1, 248, -1, -1, 20, -1, -1, -1, -1,
    -1, -1, -1, -1, 126, 127, -1, 160, -1, -1, -1, -1, 203, 214, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, 53, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 253, 1, 11,
    -1, -1, -1, 72, 80, 88, 98, -1, -1, 137, 149, 155, -1, 168, 179, 186,
    -1, 210, -1, 231, 234, -1, -1, -1, 0, 16, 28, 40, 52, 69, 74, 95,
    100, 107, 124, 138, 145, 159, 162, 175, -1, 181, 193, 211, 222, 228, 237, -1,
    -1, 254, -1, -1, 25, -1, -1, -1, -1, 86, -1, -1, -1, 130, -1, -1,
    -1, 176, -1, 188, 204, -1, -1, -1, 243, -1, -1, -1, -1, 18, -1, -1,
    -1, 70, -1, 87, -1, -1, 123, 141, -1, -1, -1, -1, -1, -1, 202, -1,
    -1, -1, -1, -1, -1, -1, 5, -1, 23, -1, 49, 63, 84, 92, 101, -1,
    -1, -1, 144, -1, -1, -1, -1, 185, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, 39, -1, -1, -1, -1, -1, -1, 125, -1, -1, -1, -1, -1,
    -1, -1, -1, 208, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, 10, 30, 36, -1, -1, -1, 89, -1, 115, 121, 140,
    152, -1, -1, 170, -1, 187, 197, 207, -1, -1, 244, -1, 245, -1, -1, -1,
    33, 47, -1, 71, 78, 93, -1, 111, -1, -1, -1, 153, 166, 174, -1, 183,
    -1, 213, -1, 227, 233, -1, 247, -1, 6, -1, 22, 46, 55, 62, 82, -1,
    106, -1, -1, -1, -1, -1, -1, 173, -1, -1, -1, -1, -1, -1, 235, -1,
    -1, 255, -1, 12, 35, 43, 54, 60, -1, 96, 105, 109, 122, 134, 142, 158,
    165, -1, -1, 190, 205, 218, -1, -1, 241, -1, 246, -1, 2, -1, -1, -1,
    51, -1, 85, -1, 103, 112, 118, 136, 146, -1, -1, -1, -1, 184, 201, 206,
    220, 226, -1, -1, -1, 251, -1, -1, 34, 45, -1, 65, -1, 91, -1, 114,
    117, 133, -1, -1, -1, -1, -1, 182, 200, 216, -1, -1, 236, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 42, -1, 59,
    75, -1, -1, -1, -1, 132, -1, -1, -1, 178, -1, -1, 195, -1, 223, -1,
    -1, -1, -1, -1, 9, 15, 24, 38, 57, 61, 76, 97, 104, 113, 120, 131,
    151, 156, 167, 172, -1, 191, 196, 215, -1, 232, 239, -1, -1, 250, 7, 13,
    31, 41, 56, 58, 77, 90, -1, 110, 119, 135, 150, 157, 163, 169, -1, 192,
    199, 209, 221, 224, 240, -1, 249, 252, -1, -1, -1, -1, -1, -1, 83, -1,
    -1, -1, -1, 139, 147, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 19, 27, 44,
    -1, 66, 79, -1, -1, -1, -1, -1, 148, -1, -1, -1, -1, -1, 198, -1,
    -1, 229, -1, -1, -1, -1, 3, -1, 32, -1, -1, 67, -1, -1, -1, -1,
    -1, -1, -1, -1, 164, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    8, 17, 26, 48, 50, 68, 81, 94, 102, 116, -1, 129, 143, 154, 161, 171,
    -1, 189, -1, 212, 219, 225, 238, -1, -1, -1, -1, 21, -1, -1, -1, 64,
    -1, -1, -1, 108, -1, -1, -1, -1, -1, -1, 180, -1, -1, -1, -1, -1,
    242, -1, -1, -1
};

static inline bool decode_word(const string& word, size_t word_len, uint8_t& output) {
    constexpr size_t dim = 26;

    if (word.length() != word_len) {
        return false;
    }

    int x = tolower(word[0]) - 'a';
    int y = tolower(word[word_len == 4 ? 3 : 1]) - 'a';

    if (static_cast<unsigned>(x) >= dim || static_cast<unsigned>(y) >= dim) {
        return false;
    }

    size_t offset = y * dim + x;
    int16_t value = _lookup[offset];
    if (value == -1) {
        return false;
    }

    if (word_len == 4) {
        const char* byteword = bytewords + value * 4;
        if (tolower(word[1]) != byteword[1] || tolower(word[2]) != byteword[2]) {
            return false;
        }
    }

    output = static_cast<uint8_t>(value);
    return true;
}

static inline ByteVector crc32_bytes(const ByteVector &buf) {
    const uint32_t checksum = __builtin_bswap32(esp_crc32_le(0, buf.data(), buf.size()));
    ByteVector result(sizeof(checksum));
    memcpy(result.data(), &checksum, sizeof(checksum));
    return result;
}

string Bytewords::encode(style style, const ByteVector& bytes) {
    auto crc_buf = crc32_bytes(bytes);
    ByteVector result = bytes;
    append(result, crc_buf);

    if (style == minimal) {
        std::string r;
        r.reserve(result.size() * 2);
        for (uint8_t byte : result) {
            const char* p = &bytewords[byte * 4];
            r.push_back(p[0]);
            r.push_back(p[3]);
        }
        return r;
    }

    assert(style == standard || style == uri);

    StringVector words;
    words.reserve(result.size());
    for (uint8_t byte : result) {
        words.emplace_back(&bytewords[byte * 4], 4);
    }
    return join(words, style == standard ? " " : "-");
}

ByteVector Bytewords::decode(style style, const string& s) {
    assert(style == standard || style == uri || style == minimal);
    const size_t word_len = (style == minimal) ? 2 : 4;
    const char separator = (style == standard) ? ' ' : (style == uri) ? '-' : 0;

    StringVector words = (word_len == 4) ? split(s, separator) : partition(s, 2);

    const size_t num_words = words.size();
    if (num_words < 5) return ByteVector();

    ByteVector buf;
    buf.reserve(num_words);

    for (const auto& word : words) {
        uint8_t output;
        if (!decode_word(word, word_len, output)) {
            return ByteVector();
        }
        buf.push_back(output);
    }

    if (buf.size() < 5) return ByteVector();

    const auto body_size = buf.size() - 4;
    const ByteVector body(buf.begin(), buf.begin() + body_size);
    const ByteVector body_checksum(buf.begin() + body_size, buf.end());

    const auto checksum = crc32_bytes(body);

    if (std::equal(body_checksum.begin(), body_checksum.end(), checksum.begin())) {
        return body;
    }
    return ByteVector();
}


}
