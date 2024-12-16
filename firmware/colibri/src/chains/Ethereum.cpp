// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "Ethereum.h"

std::string EthereumChain::getAddress(HDNode *node) {
  uint8_t ethereumAddress[ADDRESS_LENGTH];
  hdnode_get_ethereum_pubkeyhash(node, ethereumAddress);

  return toHex(ethereumAddress, ADDRESS_LENGTH, true);
};

std::string EthereumChain::signDigest(HDNode *node, uint8_t digest[HASH_LENGTH]) {
  // sign digest
  uint8_t sig[RECOVERABLE_SIGNATURE_LENGTH];
  uint8_t v = 0;
  hdnode_sign_digest(node, digest, sig, &v, NULL);

  // fix recovery bit for Ethereum's ecRecover
  sig[RECOVERABLE_SIGNATURE_LENGTH - 1] = (v + 27);

  return toHex(sig, RECOVERABLE_SIGNATURE_LENGTH, true);
};

/*
 * START ported code from Trezor legacy firmware (originally licensed under GPL 3.0, see
 * `./GPL-3.0.txt`):
 * https://github.com/trezor/trezor-firmware/blob/29e03bd873977a498dbce79616bfb3fe4b7a0698/legacy/firmware/ethereum.h
 */
static void hashEthereumMessage(const uint8_t *message, size_t message_len, uint8_t hash[32]) {
  struct SHA3_CTX ctx = {0};
  sha3_256_Init(&ctx);
  sha3_Update(&ctx, (const uint8_t *)"\x19" "Ethereum Signed Message:\n", 26);
  uint8_t c = 0;
  if (message_len >= 1000000000) {
    c = '0' + message_len / 1000000000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 100000000) {
    c = '0' + message_len / 100000000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 10000000) {
    c = '0' + message_len / 10000000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 1000000) {
    c = '0' + message_len / 1000000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 100000) {
    c = '0' + message_len / 100000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 10000) {
    c = '0' + message_len / 10000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 1000) {
    c = '0' + message_len / 1000 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 100) {
    c = '0' + message_len / 100 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  if (message_len >= 10) {
    c = '0' + message_len / 10 % 10;
    sha3_Update(&ctx, &c, 1);
  }
  c = '0' + message_len % 10;
  sha3_Update(&ctx, &c, 1);
  sha3_Update(&ctx, message, message_len);
  keccak_Final(&ctx, hash);
}

std::string EthereumChain::signMessage(HDNode *node, std::string &message) {
  uint8_t digest[HASH_LENGTH];
  const char *msg = message.c_str();

  hashEthereumMessage((uint8_t *)msg, message.length(), digest);

  return signDigest(node, digest);
}

static void hashEthereumTypedData(
    const uint8_t domain_separator_hash[32], const uint8_t message_hash[32], bool has_message_hash,
    uint8_t hash[32]
) {
  struct SHA3_CTX ctx = {0};
  sha3_256_Init(&ctx);
  sha3_Update(&ctx, (const uint8_t *)"\x19\x01", 2);
  sha3_Update(&ctx, domain_separator_hash, 32);
  if (has_message_hash) {
    sha3_Update(&ctx, message_hash, 32);
  }
  keccak_Final(&ctx, hash);
}

std::string EthereumChain::signTypedData(
    HDNode *node, std::string &domainSeparatorHash, std::string &messageHash
) {
  uint8_t digest[HASH_LENGTH];

  uint8_t mhBytes[HASH_LENGTH];
  uint8_t dshBytes[HASH_LENGTH];
  fromHex(domainSeparatorHash.c_str(), dshBytes, HASH_LENGTH);

  bool hasMessageHash = messageHash.length() > 0 && messageHash != "0x";
  if (hasMessageHash) {
    fromHex(messageHash.c_str(), mhBytes, HASH_LENGTH);
  }

  hashEthereumTypedData(dshBytes, mhBytes, hasMessageHash, digest);

  return signDigest(node, digest);
}

/*
 * END ported code from Trezor legacy firmware
 */
