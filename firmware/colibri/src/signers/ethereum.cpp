// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "ethereum.h"

std::string ethGetAddress(HDNode *node) {
  uint8_t ethereumAddress[ADDRESS_LENGTH];
  hdnode_get_ethereum_pubkeyhash(node, ethereumAddress);

  return toHex(ethereumAddress, ADDRESS_LENGTH, true);
};

static std::string ethSignRecoverableDigest(HDNode *node, uint8_t digest[HASH_LENGTH]) {
  // sign digest
  uint8_t sig[RECOVERABLE_SIGNATURE_LENGTH];
  uint8_t v = 0;
  hdnode_sign_digest(node, digest, sig, &v, NULL);

  // fix recovery bit for Ethereum's ecRecover
  sig[RECOVERABLE_SIGNATURE_LENGTH - 1] = (v + 27);

  return toHex(sig, RECOVERABLE_SIGNATURE_LENGTH, true);
};

/*
 * START ported and/or adapted code from Trezor firmware (originally licensed under GPL 3.0, see
 * `licenses/GPL-3.0.txt`):
 * https://github.com/trezor/trezor-firmware/blob/29e03bd873977a498dbce79616bfb3fe4b7a0698/legacy/firmware/ethereum.c
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

std::string ethSignMessage(HDNode *node, std::string &message) {
  uint8_t digest[HASH_LENGTH];
  const char *msg = message.c_str();

  hashEthereumMessage((uint8_t *)msg, message.length(), digest);

  return ethSignRecoverableDigest(node, digest);
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

std::string ethSignTypedDataHash(
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

  return ethSignRecoverableDigest(node, digest);
}

// ===== Ethereum transaction signing =====
static void hash_data(SHA3_CTX *ctx, const uint8_t *buf, size_t size) {
  sha3_Update(ctx, buf, size);
}

static void hash_rlp_length(SHA3_CTX *ctx, uint32_t length, uint8_t firstbyte) {
  uint8_t buf[4] = {0};
  if (length == 1 && firstbyte <= 0x7f) {
    // empty length header
  } else if (length <= 55) {
    buf[0] = 0x80 + length;
    hash_data(ctx, buf, 1);
  } else if (length <= 0xff) {
    buf[0] = 0xb7 + 1;
    buf[1] = length;
    hash_data(ctx, buf, 2);
  } else if (length <= 0xffff) {
    buf[0] = 0xb7 + 2;
    buf[1] = length >> 8;
    buf[2] = length & 0xff;
    hash_data(ctx, buf, 3);
  } else {
    buf[0] = 0xb7 + 3;
    buf[1] = length >> 16;
    buf[2] = length >> 8;
    buf[3] = length & 0xff;
    hash_data(ctx, buf, 4);
  }
}

static void hash_rlp_field(SHA3_CTX *ctx, const uint8_t *buf, size_t size) {
  hash_rlp_length(ctx, size, buf[0]);
  hash_data(ctx, buf, size);
}

static void hash_rlp_number(SHA3_CTX *ctx, uint64_t number) {
  if (!number) {
    return;
  }
  uint8_t data[8] = {0};
  data[0] = (number >> 56) & 0xff;
  data[1] = (number >> 48) & 0xff;
  data[2] = (number >> 40) & 0xff;
  data[3] = (number >> 32) & 0xff;
  data[4] = (number >> 24) & 0xff;
  data[5] = (number >> 16) & 0xff;
  data[6] = (number >> 8) & 0xff;
  data[7] = (number) & 0xff;
  int offset = 0;
  while (!data[offset]) {
    offset++;
  }
  hash_rlp_field(ctx, data + offset, 8 - offset);
}

static void hash_rlp_list_length(SHA3_CTX *ctx, uint32_t length) {
  uint8_t buf[4] = {0};
  if (length <= 55) {
    buf[0] = 0xc0 + length;
    hash_data(ctx, buf, 1);
  } else if (length <= 0xff) {
    buf[0] = 0xf7 + 1;
    buf[1] = length;
    hash_data(ctx, buf, 2);
  } else if (length <= 0xffff) {
    buf[0] = 0xf7 + 2;
    buf[1] = length >> 8;
    buf[2] = length & 0xff;
    hash_data(ctx, buf, 3);
  } else {
    buf[0] = 0xf7 + 3;
    buf[1] = length >> 16;
    buf[2] = length >> 8;
    buf[3] = length & 0xff;
    hash_data(ctx, buf, 4);
  }
}

static void sign_transaction(
    const uint8_t *privkey, uint64_t chain_id, const uint8_t *nonce, size_t nonce_size,
    const uint8_t *gas_price, size_t gas_price_size, const uint8_t *gas_limit,
    size_t gas_limit_size, const uint8_t *to, size_t to_size, const uint8_t *value,
    size_t value_size, const uint8_t *data, size_t data_size, uint8_t *signature
) {
  SHA3_CTX ctx = {0};
  sha3_256_Init(&ctx);

  // Hash the transaction fields
  hash_rlp_list_length(
      &ctx, nonce_size + gas_price_size + gas_limit_size + to_size + value_size + data_size
  );
  hash_rlp_field(&ctx, nonce, nonce_size);
  hash_rlp_field(&ctx, gas_price, gas_price_size);
  hash_rlp_field(&ctx, gas_limit, gas_limit_size);
  hash_rlp_field(&ctx, to, to_size);
  hash_rlp_field(&ctx, value, value_size);
  hash_rlp_field(&ctx, data, data_size);

  // Add chain_id, r, and s for EIP-155
  hash_rlp_number(&ctx, chain_id);
  hash_rlp_length(&ctx, 0, 0);
  hash_rlp_length(&ctx, 0, 0);

  uint8_t hash[32] = {0};
  keccak_Final(&ctx, hash);

  uint8_t v = 0;
  ecdsa_sign_digest(&secp256k1, privkey, hash, signature, &v, NULL);
  signature[RECOVERABLE_SIGNATURE_LENGTH - 1] = v + 27;
}

static void sign_eip1559_transaction(
    const uint8_t *privkey, uint64_t chain_id, const uint8_t *nonce, size_t nonce_size,
    const uint8_t *max_priority_fee, size_t max_priority_fee_size, const uint8_t *max_gas_fee,
    size_t max_gas_fee_size, const uint8_t *gas_limit, size_t gas_limit_size, const uint8_t *to,
    size_t to_size, const uint8_t *value, size_t value_size, const uint8_t *data, size_t data_size,
    uint8_t *signature
) {
  SHA3_CTX ctx = {0};
  sha3_256_Init(&ctx);

  // Hash the transaction fields
  hash_rlp_number(&ctx, 2);  // EIP1559 transaction type
  hash_rlp_list_length(
      &ctx,
      chain_id + nonce_size + max_priority_fee_size + max_gas_fee_size + gas_limit_size + to_size +
          value_size + data_size
  );
  hash_rlp_number(&ctx, chain_id);
  hash_rlp_field(&ctx, nonce, nonce_size);
  hash_rlp_field(&ctx, max_priority_fee, max_priority_fee_size);
  hash_rlp_field(&ctx, max_gas_fee, max_gas_fee_size);
  hash_rlp_field(&ctx, gas_limit, gas_limit_size);
  hash_rlp_field(&ctx, to, to_size);
  hash_rlp_field(&ctx, value, value_size);
  hash_rlp_field(&ctx, data, data_size);

  uint8_t hash[32] = {0};
  keccak_Final(&ctx, hash);

  uint8_t v = 0;
  ecdsa_sign_digest(&secp256k1, privkey, hash, signature, &v, NULL);
  signature[RECOVERABLE_SIGNATURE_LENGTH - 1] = v + 27;
}

/*
 * END ported code from Trezor firmware
 */

WalletResponse ethSignTransaction(HDNode *node, JsonArrayConst input) {
  uint8_t signature[RECOVERABLE_SIGNATURE_LENGTH];

  if (input && input.size() >= 7) {
    // parse hex input
    uint8_t chainIdBytes[8];
    size_t chainIdLen = fromHex(input[0].as<const char *>(), chainIdBytes, 8);
    uint64_t chainId = bytesToUint64(chainIdBytes, chainIdLen);

    uint8_t to[ADDRESS_LENGTH] = {0};
    size_t toSize = fromHex(input[1].as<const char *>(), to, ADDRESS_LENGTH);

    uint8_t value[32];
    size_t valueSize = fromHex(input[2].as<const char *>(), value, 32);

    const char *dataStr = input[3];
    size_t dataLen = (strlen(dataStr) - 2) / 2;
    uint8_t data[dataLen];
    size_t dataSize = fromHex(dataStr, data, dataLen);

    uint8_t nonce[32];
    size_t nonceSize = fromHex(input[4].as<const char *>(), nonce, 32);

    uint8_t gasLimit[32];
    size_t gasLimitSize = fromHex(input[5].as<const char *>(), gasLimit, 32);

    log_d("Params (chainId: %lu)", chainId);
    log_d("Param sizes (to: %d, data: %d, chainId: %d)", toSize, dataSize, chainIdLen);

    if (input.size() == 7) {
      // EIP-155
      uint8_t gasPrice[32];
      size_t gasPriceSize = fromHex(input[6].as<const char *>(), gasPrice, 32);

      sign_transaction(
          node->private_key, chainId, nonce, nonceSize, gasPrice, gasPriceSize, gasLimit,
          gasLimitSize, to, toSize, value, valueSize, data, dataSize, signature
      );
    } else if (input.size() == 8) {
      // EIP-1559
      uint8_t maxFeePerGas[32];
      size_t maxFeePerGasSize = fromHex(input[6].as<const char *>(), maxFeePerGas, 32);

      uint8_t maxPriorityFeePerGas[32];
      size_t maxPriorityFeePerGasSize =
          fromHex(input[7].as<const char *>(), maxPriorityFeePerGas, 32);

      sign_eip1559_transaction(
          node->private_key, chainId, nonce, nonceSize, maxFeePerGas, maxFeePerGasSize,
          maxPriorityFeePerGas, maxPriorityFeePerGasSize, gasLimit, gasLimitSize, to, toSize, value,
          valueSize, data, dataSize, signature
      );
    }
  } else {
    return WalletResponse(InvalidParams, RPC_ERROR_INVALID_PARAMS);
  }

  std::string sig = toHex(signature, RECOVERABLE_SIGNATURE_LENGTH, true);
  if (sig.empty()) {
    return WalletResponse(InternalError, RPC_ERROR_SIGNATURE_FAILED);
  }

  return WalletResponse(sig);
}
