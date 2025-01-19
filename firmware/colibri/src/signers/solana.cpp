// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "solana.h"

#include "../utils/chains.h"

std::string solGetAddress(HDNode *node) {
  size_t addressLen = 44;
  char address[addressLen];

#if defined(CORE_DEBUG_LEVEL) && CORE_DEBUG_LEVEL > 0
  b58enc(address, &addressLen, node->public_key, 33);
  log_d("pubkey-base58: %s (len: %d)", address, addressLen);
  addressLen = 44;
#endif

  uint8_t hash[HASH_LENGTH];
  sha256_Raw(node->public_key, 33, hash);

  if (!b58enc(address, &addressLen, hash, HASH_LENGTH)) {
    return "";
  }

  log_d("pubkeyHash-base58: %s (len: %d)", address, addressLen);

  return std::string(address);
}
