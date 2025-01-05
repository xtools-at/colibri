// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "helpers.h"

bool isAllZero(const uint8_t *value, size_t len) {
  if (len == 0) return true;

  for (size_t i = 0; i < len; i++) {
    if (value[i] != 0) {
      return false;
    }
  }

  return true;
}

std::string toHex(const uint8_t *data, size_t len, bool add0xPrefix) {
  const char hex_digits[] = "0123456789abcdef";
  std::string hex(len * 2 + (add0xPrefix ? 2 : 0), '\0');
  size_t offset = 0;
  if (add0xPrefix) {
    hex[0] = '0';
    hex[1] = 'x';
    offset = 2;
  }
  for (size_t i = 0; i < len; ++i) {
    hex[2 * i + offset] = hex_digits[data[i] >> 4];
    hex[2 * i + 1 + offset] = hex_digits[data[i] & 0x0F];
  }
  return hex;
}

uint8_t hexDigitToValue(char hexDigit) {
  if (hexDigit >= '0' && hexDigit <= '9') {
    return hexDigit - '0';
  } else if (hexDigit >= 'a' && hexDigit <= 'f') {
    return hexDigit - 'a' + 10;
  } else if (hexDigit >= 'A' && hexDigit <= 'F') {
    return hexDigit - 'A' + 10;
  } else {
    return 0;
  }
}

size_t fromHex(const char *hex, uint8_t *bytes, size_t maxBytesLen) {
  size_t len = strlen(hex);
  size_t start = 0;

  // Check for leading "0x"
  if (len >= 2 && hex[0] == '0' && hex[1] == 'x') {
    start = 2;
    len -= 2;
  }

  // Ensure the length is even
  if (len % 2 != 0) {
    return 0;
  }

  size_t bytesLen = len / 2;
  if (maxBytesLen > 0 && bytesLen > maxBytesLen) {
    bytesLen = maxBytesLen;
  }

  for (size_t i = 0; i < bytesLen; ++i) {
    uint8_t high = hexDigitToValue(hex[start + 2 * i]);
    uint8_t low = hexDigitToValue(hex[start + 2 * i + 1]);
    bytes[i] = (high << 4) | low;
  }

  return bytesLen;
}

// Helper method to convert uint32_t to bytes
void uint32ToBytes(uint32_t value, uint8_t output[4]) {
  output[0] = (value >> 24) & 0xFF;
  output[1] = (value >> 16) & 0xFF;
  output[2] = (value >> 8) & 0xFF;
  output[3] = value & 0xFF;
}
