// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "solana.h"

#include "../utils/chains.h"

std::string solGetAddress(HDNode *node) {
  size_t addressLen = 45;
  char address[addressLen];

  if (!b58enc(address, &addressLen, node->public_key + 1, 32)) return "";

  return std::string(address);
}
