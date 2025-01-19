// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "crypto.h"

void doubleHash(
    const uint8_t *data, uint16_t length, uint8_t result[HASH_LENGTH],
    const uint8_t salt[AES_IV_SIZE]
) {
  const uint16_t hashBufLen = HASH_LENGTH + AES_IV_SIZE;
  uint16_t dataBufLen = length + AES_IV_SIZE;
  uint16_t bufLen = hashBufLen > dataBufLen ? hashBufLen : dataBufLen;

  uint8_t buffer[bufLen];

  // salted SHA-256 from input data
  memcpy(buffer, data, length);
  memcpy(buffer + length, salt, AES_IV_SIZE);
  sha3_256(buffer, dataBufLen, result);

  // salt and hash again
  memcpy(buffer, result, HASH_LENGTH);
  memcpy(buffer + HASH_LENGTH, salt, AES_IV_SIZE);
  sha3_256(buffer, hashBufLen, result);

  memzero(buffer, bufLen);
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
  log_ss("Entropy: %s", toHex(entropy, sizeof(entropy)).c_str());

  const char *mnemonic = mnemonic_from_data(entropy, strength / 8);

  std::string m = std::string(mnemonic);
  memzero(entropy, sizeof(entropy));
  mnemonic_clear();

  log_ss("Generated mnemonic phrase: %s", m.c_str());

  return m;
}

bool signHash(
    HDNode *node, const uint8_t *digest, uint8_t signature[RECOVERABLE_SIGNATURE_LENGTH]
) {
  log_d("Signing digest...");

  uint8_t v = 0;
  bool success = hdnode_sign_digest(node, digest, signature, &v, NULL) == 0;
  signature[RECOVERABLE_SIGNATURE_LENGTH - 1] = v;

  return success;
}
