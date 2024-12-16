#pragma once

#include "crypto.h"

void doubleHash(
    const uint8_t *data, uint16_t length, uint8_t *result, const uint8_t salt[AES_IV_SIZE]
) {
  uint16_t bufLen = HASH_LENGTH + AES_IV_SIZE;
  uint8_t hashBuffer[bufLen];

  // SHA 256
  sha3_256(data, length, hashBuffer);

  // Add salt to the hashBuffer
  memcpy(hashBuffer + HASH_LENGTH, salt, AES_IV_SIZE);

  // hash again
  sha3_256(hashBuffer, bufLen, result);

  memzero(hashBuffer, bufLen);
}

void aesEncrypt(uint8_t *in, size_t len, uint8_t *out, uint8_t *key, uint8_t *iv) {
  mbedtls_aes_context ctx;
  mbedtls_aes_init(&ctx);
  mbedtls_aes_setkey_enc(&ctx, key, 256);

  size_t iv_offset = 0;
  mbedtls_aes_crypt_cfb128(&ctx, MBEDTLS_AES_ENCRYPT, len, &iv_offset, iv, in, out);

  mbedtls_aes_free(&ctx);
}

void aesDecrypt(uint8_t *in, size_t len, uint8_t *out, uint8_t *key, uint8_t *iv) {
  mbedtls_aes_context ctx;
  mbedtls_aes_init(&ctx);
  mbedtls_aes_setkey_dec(&ctx, key, 256);

  size_t iv_offset = 0;
  mbedtls_aes_crypt_cfb128(&ctx, MBEDTLS_AES_DECRYPT, len, &iv_offset, iv, in, out);

  mbedtls_aes_free(&ctx);
}

std::string generateMnemonic(uint8_t words) {
  int strength = 128;

  switch (words) {
    case 12:
      strength = 128;
      break;
    case 18:
      strength = 192;
      break;
    case 24:
      strength = 256;
      break;
    default:
      return "";
  }

  uint8_t entropy[32] = {0};
  generateEntropy(entropy);
  // log_s("Entropy: %s", toHex(entropy, sizeof(entropy)).c_str());

  const char *mnemonic = mnemonic_from_data(entropy, strength / 8);

  std::string m = std::string(mnemonic);
  memzero(entropy, sizeof(entropy));
  mnemonic_clear();

  // log_s("Generated mnemonic phrase: %s", m.c_str());

  return m;
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
