// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "solana.h"

#include "../utils/chains.h"

extern "C" {
#include <base58.h>
}

std::string solGetAddress(HDNode *node) {
  size_t addressLen = 44;
  char address[addressLen];

  uint8_t hash[HASH_LENGTH];
  sha256_Raw(node->public_key, 33, hash);

  if (!b58enc(address, &addressLen, hash, HASH_LENGTH)) {
    return "";
  }

  return std::string(address);
}
