// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "solana.h"

std::string solGetAddress(HDNode *node) {
  size_t addressLen = 45;
  char address[addressLen];

  if (!b58enc(address, &addressLen, node->public_key + 1, ED25519_PUBLICKEY_LENGTH)) {
    return "";
  }

  return std::string(address);
}
