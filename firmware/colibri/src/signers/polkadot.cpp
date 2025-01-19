// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "polkadot.h"

#include "../utils/chains.h"

extern "C" {
#include <blake2b.h>
}

// Function to derive Polkadot address from HDNode
std::string dotGetAddress(HDNode *node) {
  uint8_t address_with_checksum[35];

  // Prepend the network identifier (0 for Polkadot mainnet)
  address_with_checksum[0] = 0;  // Polkadot mainnet prefix

  // Polkadot address is derived from the public key using Blake2b
  blake2b(node->public_key + 1, 32, address_with_checksum + 1, 32);

  // Compute the checksum
  uint8_t checksum[2];
  blake2b(address_with_checksum, 33, checksum, 2);
  // Append the checksum to the address
  memcpy(address_with_checksum + 33, checksum, 2);

  log_d(
      "Polkadot address bytes: %s",
      toHex(address_with_checksum, sizeof(address_with_checksum)).c_str()
  );

  // Encode the address in base58
  char address[48];
  size_t address_len = sizeof(address);

  if (!b58enc(address, &address_len, address_with_checksum, sizeof(address_with_checksum))) {
    log_e("Failed to encode Polkadot address, addressLen = %d", address_len);
    return "";
  }

  return std::string(address);
}
