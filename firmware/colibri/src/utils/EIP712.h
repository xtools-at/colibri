// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"

#include <ArduinoJson.h>

extern "C" {
#include <sha3.h>
}

// ==== Security Limits (Hardware Wallet Safe) ====

#define EIP712_MAX_TYPESTRING      512
#define EIP712_MAX_DEPS            10
#define EIP712_MAX_FIELDS          16
#define EIP712_MAX_ARRAY_ELEMENTS  16
#define EIP712_MAX_STRUCT_DEPTH    4
#define EIP712_MAX_TYPE_NAME       32

class EIP712
{
public:
    // Main entry
    static bool computeTypedDataHash(
        JsonDocument& doc,
        uint8_t out[32]
    );

private:

    // ==== Struct Hashing ====
    static bool hashStruct(
        JsonObject types,
        const char* primaryType,
        JsonObject data,
        uint8_t out[32],
        int depth
    );

    static bool hashArray(
        JsonObject types,
        const char* baseType,
        JsonArray array,
        uint8_t out[32],
        int depth
    );

    // ==== Type Handling ====
    static void computeTypeHash(
        JsonObject types,
        const char* primaryType,
        uint8_t out[32]
    );

    static void encodeType(
        JsonObject types,
        const char* primaryType,
        char* out
    );

    static void encodeSingleType(
        JsonObject types,
        const char* type,
        char* out
    );

    static int findDependencies(
        JsonObject types,
        const char* primary,
        char deps[EIP712_MAX_DEPS][EIP712_MAX_TYPE_NAME],
        int depCount
    );

    static void sortDeps(
        char deps[EIP712_MAX_DEPS][EIP712_MAX_TYPE_NAME],
        int count
    );

    // ==== Encoding Helpers ====
    static void encodeUint(uint64_t value, uint8_t out[32]);
    static void encodeBool(bool v, uint8_t out[32]);
    static void encodeAddress(const char* hex, uint8_t out[32]);
    static void encodeString(const char* str, uint8_t out[32]);

    static bool isArrayType(const char* type);
    static bool isPrimitive(const char* type);
    static void getBaseType(const char* type, char* out);

    static bool hexToBytes(const char* hex, uint8_t* out, size_t outLen);
};
