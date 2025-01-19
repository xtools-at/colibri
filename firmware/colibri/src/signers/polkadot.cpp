// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "polkadot.h"

#include "../utils/chains.h"

extern "C" {
#include <blake2b.h>
}

// Function to derive Polkadot address from HDNode
std::string dotGetAddress(HDNode *node, uint32_t slip44, uint8_t prefixOverride) {
  uint8_t addressWithChecksum[35];

  // set prefix
  uint8_t prefix = 0x00;  // default: Polkadot
  if (slip44 == 434) {
    // Kusama
    prefix = 0x02;
  }
  if (prefixOverride) prefix = prefixOverride;
  log_d("Polkadot address prefix: %d", prefix);

  // Prepend the network identifier (0 for Polkadot mainnet)
  addressWithChecksum[0] = prefix;
  memcpy(addressWithChecksum + 1, node->public_key + 1, 32);

  // Compute the checksum // TODO: this needs fixing
  uint8_t checksum[2];
  blake2b(addressWithChecksum, 33, checksum, 2);
  memcpy(addressWithChecksum + 33, checksum, 2);

  log_d(
      "Polkadot full address bytes: %s",
      toHex(addressWithChecksum, sizeof(addressWithChecksum)).c_str()
  );

  // Encode the address in base58
  char address[49];
  size_t addressLen = sizeof(address);

  if (!b58enc(address, &addressLen, addressWithChecksum, sizeof(addressWithChecksum))) {
    log_e("Failed to encode Polkadot address, addressLen = %d", addressLen);
    return "";
  }

  log_d("Polkadot base58 address length: %d", addressLen);

  return std::string(address);
}
