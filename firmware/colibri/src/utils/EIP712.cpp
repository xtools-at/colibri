// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "EIP712.h"

// ==== Primitive Checks ====

bool EIP712::isPrimitive(const char* type) {
  return strncmp(type, "uint", 4) == 0 || strncmp(type, "int", 3) == 0 || strcmp(type, "address") == 0 ||
      strcmp(type, "bool") == 0 || strcmp(type, "string") == 0 || strncmp(type, "bytes", 5) == 0;
}

bool EIP712::isArrayType(const char* type) { return strchr(type, '[') != NULL; }

void EIP712::getBaseType(const char* type, char* out) {
  int i = 0;
  while (type[i] && type[i] != '[') {
    out[i] = type[i];
    i++;
  }
  out[i] = 0;
}

// ==== Hex ====

bool EIP712::hexToBytes(const char* hex, uint8_t* out, size_t outLen) {
  if (strlen(hex) < outLen * 2 + 2) return false;

  hex += 2;  // skip 0x

  for (size_t i = 0; i < outLen; i++) {
    sscanf(hex + 2 * i, "%2hhx", &out[i]);
  }
  return true;
}

// ==== Encoding ====

void EIP712::encodeUint(uint64_t value, uint8_t out[32]) {
  memset(out, 0, 32);
  for (int i = 0; i < 8; i++) out[31 - i] = (value >> (8 * i)) & 0xff;
}

void EIP712::encodeBool(bool v, uint8_t out[32]) {
  memset(out, 0, 32);
  out[31] = v ? 1 : 0;
}

void EIP712::encodeAddress(const char* hex, uint8_t out[32]) {
  memset(out, 0, 32);
  hexToBytes(hex, out + 12, 20);
}

void EIP712::encodeString(const char* str, uint8_t out[32]) {
  struct SHA3_CTX ctx = {0};
  sha3_256_Init(&ctx);

  sha3_Update(&ctx, (const uint8_t*)str, strlen(str));
  keccak_Final(&ctx, out);
}

// ==== Dependency Resolution ====

int EIP712::findDependencies(
    JsonObject types, const char* primary, char deps[EIP712_MAX_DEPS][EIP712_MAX_TYPE_NAME], int depCount
) {
  JsonArray fields = types[primary];

  for (JsonObject field : fields) {
    const char* type = field["type"];
    char base[EIP712_MAX_TYPE_NAME];
    getBaseType(type, base);

    if (isPrimitive(base)) continue;

    bool exists = false;
    for (int i = 0; i < depCount; i++)
      if (strcmp(deps[i], base) == 0) exists = true;

    if (!exists) {
      if (depCount >= EIP712_MAX_DEPS) return depCount;

      strcpy(deps[depCount++], base);

      depCount = findDependencies(types, base, deps, depCount);
    }
  }

  return depCount;
}

void EIP712::sortDeps(char deps[EIP712_MAX_DEPS][EIP712_MAX_TYPE_NAME], int count) {
  for (int i = 0; i < count - 1; i++)
    for (int j = i + 1; j < count; j++)
      if (strcmp(deps[i], deps[j]) > 0) {
        char tmp[EIP712_MAX_TYPE_NAME];
        strcpy(tmp, deps[i]);
        strcpy(deps[i], deps[j]);
        strcpy(deps[j], tmp);
      }
}

// ==== Type Encoding ====

void EIP712::encodeSingleType(JsonObject types, const char* type, char* out) {
  strcat(out, type);
  strcat(out, "(");

  JsonArray fields = types[type];

  bool first = true;

  for (JsonObject field : fields) {
    if (!first) strcat(out, ",");
    first = false;

    strcat(out, field["type"]);
    strcat(out, " ");
    strcat(out, field["name"]);
  }

  strcat(out, ")");
}

void EIP712::encodeType(JsonObject types, const char* primary, char* out) {
  out[0] = 0;

  char deps[EIP712_MAX_DEPS][EIP712_MAX_TYPE_NAME];
  int depCount = 0;

  depCount = findDependencies(types, primary, deps, depCount);

  sortDeps(deps, depCount);

  encodeSingleType(types, primary, out);

  for (int i = 0; i < depCount; i++) encodeSingleType(types, deps[i], out);
}

void EIP712::computeTypeHash(JsonObject types, const char* primaryType, uint8_t out[32]) {
  char buffer[EIP712_MAX_TYPESTRING];
  encodeType(types, primaryType, buffer);

  struct SHA3_CTX ctx = {0};
  sha3_256_Init(&ctx);
  sha3_Update(&ctx, (const uint8_t*)buffer, strlen(buffer));
  keccak_Final(&ctx, out);
}

// ==== Struct Hash ====

bool EIP712::hashArray(JsonObject types, const char* baseType, JsonArray array, uint8_t out[32], int depth) {
  if (depth > EIP712_MAX_STRUCT_DEPTH) return false;
  if (array.size() > EIP712_MAX_ARRAY_ELEMENTS) return false;

  struct SHA3_CTX ctx = {0};
  sha3_256_Init(&ctx);

  for (JsonVariant v : array) {
    uint8_t elem[32];

    if (strcmp(baseType, "string") == 0)
      encodeString(v.as<const char*>(), elem);
    else if (strcmp(baseType, "address") == 0)
      encodeAddress(v.as<const char*>(), elem);
    else if (strncmp(baseType, "uint", 4) == 0)
      encodeUint(v.as<uint64_t>(), elem);
    else if (strcmp(baseType, "bool") == 0)
      encodeBool(v.as<bool>(), elem);
    else {
      if (!hashStruct(types, baseType, v.as<JsonObject>(), elem, depth + 1)) return false;
    }

    sha3_Update(&ctx, elem, 32);
  }

  keccak_Final(&ctx, out);

  return true;
}

bool EIP712::hashStruct(JsonObject types, const char* primaryType, JsonObject data, uint8_t out[32], int depth) {
  if (depth > EIP712_MAX_STRUCT_DEPTH) return false;

  struct SHA3_CTX ctx = {0};
  sha3_256_Init(&ctx);

  uint8_t typeHash[32];
  computeTypeHash(types, primaryType, typeHash);

  sha3_Update(&ctx, typeHash, 32);

  JsonArray fields = types[primaryType];

  if (fields.size() > EIP712_MAX_FIELDS) return false;

  for (JsonObject field : fields) {
    const char* name = field["name"];
    const char* type = field["type"];

    uint8_t fieldHash[32];

    if (isArrayType(type)) {
      char base[EIP712_MAX_TYPE_NAME];
      getBaseType(type, base);

      if (!hashArray(types, base, data[name].as<JsonArray>(), fieldHash, depth + 1)) return false;
    } else if (strcmp(type, "string") == 0)
      encodeString(data[name], fieldHash);
    else if (strcmp(type, "address") == 0)
      encodeAddress(data[name], fieldHash);
    else if (strncmp(type, "uint", 4) == 0)
      encodeUint(data[name], fieldHash);
    else if (strcmp(type, "bool") == 0)
      encodeBool(data[name], fieldHash);
    else {
      if (!hashStruct(types, type, data[name], fieldHash, depth + 1)) return false;
    }

    sha3_Update(&ctx, fieldHash, 32);
  }

  keccak_Final(&ctx, out);

  return true;
}

// ==== Final Typed Data Hash ====

bool EIP712::computeTypedDataHash(JsonDocument& doc, uint8_t out[32]) {
  uint8_t domainHash[32];
  uint8_t messageHash[32];

  if (!hashStruct(doc["types"], "EIP712Domain", doc["domain"], domainHash, 0)) return false;

  if (!hashStruct(doc["types"], doc["primaryType"], doc["message"], messageHash, 0)) return false;

  struct SHA3_CTX ctx = {0};
  sha3_256_Init(&ctx);

  uint8_t prefix[2] = {0x19, 0x01};

  sha3_Update(&ctx, prefix, 2);
  sha3_Update(&ctx, domainHash, 32);
  sha3_Update(&ctx, messageHash, 32);

  keccak_Final(&ctx, out);

  return true;
}
