// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"
#include "helpers.h"
#include "mbedtls/aes.h"
#include "mbedtls/platform.h"
#include "trng.h"

extern "C" {
#include <bip32.h>
#include <bip39.h>
#include <curves.h>
#include <memzero.h>
#include <secp256k1.h>
#include <sha3.h>
}

void doubleHash(
    const uint8_t *data, uint16_t length, uint8_t *result, const uint8_t salt[AES_IV_SIZE] = {0}
);

void aesEncrypt(uint8_t *in, size_t len, uint8_t *out, uint8_t *key, uint8_t *iv);

void aesDecrypt(uint8_t *in, size_t len, uint8_t *out, uint8_t *key, uint8_t *iv);

std::string generateMnemonic(uint8_t words = DEFAULT_MNEMONIC_WORDS);

bool signHash(HDNode *node, const uint8_t *hash, uint8_t signature[RECOVERABLE_SIGNATURE_LENGTH]);
