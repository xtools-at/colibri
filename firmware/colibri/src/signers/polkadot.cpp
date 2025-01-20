// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "polkadot.h"

extern "C" {
#include <blake2b.h>
}

const char* ss58ContextPrefix = "SS58PRE";
const uint8_t ss58PrefixLen = 7;

static bool dotGetChecksum(
    const uint8_t* addressWithNetwork, uint8_t addressLen, uint8_t checksum[POLKADOT_CKS_LENGTH]
) {
  uint8_t hash[64];
  uint8_t cksInput[ss58PrefixLen + addressLen];
  memcpy(cksInput, ss58ContextPrefix, ss58PrefixLen);
  memcpy(cksInput + ss58PrefixLen, addressWithNetwork, addressLen);
  log_v("Polkadot checksum input bytes: %s", toHex(cksInput, sizeof(cksInput)).c_str(), true);

  if (blake2b(cksInput, sizeof(cksInput), hash, 64) < 0) {
    log_e("Blake2b hash failed while computing Polkadot address checksum");
    return false;
  }

  memcpy(checksum, hash, POLKADOT_CKS_LENGTH);

  return true;
}

static uint8_t getNetworkPrefixLength(uint16_t network) {
  if (network < 0x40) return 1;
  if (network < 0x4000) return 2;
  return 0;
}

// Function to derive Polkadot address from HDNode
std::string dotGetAddress(HDNode* node, uint32_t slip44, uint16_t networkPrefixOverride) {
  // determine network prefix
  uint16_t network = 0;  // default: Polkadot
  if (slip44 == 434) {
    // Kusama
    network = 2;
  }
  if (&networkPrefixOverride != nullptr && networkPrefixOverride != POLKADOT_UNKNOWN_NETWORK) {
    network = networkPrefixOverride;
  }
  log_v("Polkadot address prefix: %d", network);

  uint8_t networkPrefixLen = getNetworkPrefixLength(network);
  if (networkPrefixLen == 0) {
    log_e("Polkadot network prefix out of bounds: %d", network);
    return "";
  }

  uint8_t addressBytes[networkPrefixLen + ED25519_PUBLICKEY_LENGTH + POLKADOT_CKS_LENGTH];

  // prepend the network identifier (0 for Polkadot mainnet)
  if (networkPrefixLen == 1) {
    uint8_t networkByte = network;
    addressBytes[0] = networkByte;
  } else {
    memcpy(addressBytes, &network, networkPrefixLen);
  }

  // copy Ed25519 public key from HD node
  memcpy(addressBytes + networkPrefixLen, node->public_key + 1, ED25519_PUBLICKEY_LENGTH);

  // compute the checksum
  uint8_t checksum[POLKADOT_CKS_LENGTH];
  if (!dotGetChecksum(addressBytes, ED25519_PUBLICKEY_LENGTH + networkPrefixLen, checksum))
    return "";
  memcpy(addressBytes + ED25519_PUBLICKEY_LENGTH + networkPrefixLen, checksum, POLKADOT_CKS_LENGTH);

  log_v("Polkadot full address bytes: %s", toHex(addressBytes, sizeof(addressBytes)).c_str(), true);

  // encode the address in base58
  char address[49];
  size_t addressLen = sizeof(address);

  if (!b58enc(address, &addressLen, addressBytes, sizeof(addressBytes))) {
    log_e("Failed to encode Polkadot address, addressLen = %d", addressLen);
    return "";
  }

  return std::string(address);
}
