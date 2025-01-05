// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "Bitcoin.h"

#include "../utils/chains.h"

/*
 * START ported and/or adapted code from Trezor firmware (originally licensed under GPL 3.0, see
 * `misc/GPL-3.0.txt`):
 * https://github.com/trezor/trezor-firmware/blob/29e03bd873977a498dbce79616bfb3fe4b7a0698/legacy/firmware/crypto.c
 */

uint32_t ser_length(uint32_t len, uint8_t *out) {
  if (len < 253) {
    out[0] = len & 0xFF;
    return 1;
  }
  if (len < 0x10000) {
    out[0] = 253;
    out[1] = len & 0xFF;
    out[2] = (len >> 8) & 0xFF;
    return 3;
  }
  out[0] = 254;
  out[1] = len & 0xFF;
  out[2] = (len >> 8) & 0xFF;
  out[3] = (len >> 16) & 0xFF;
  out[4] = (len >> 24) & 0xFF;
  return 5;
}

static void cryptoMessageHash(
    const BitcoinNetwork *coin, const uint8_t *message, size_t message_len,
    uint8_t hash[HASHER_DIGEST_LENGTH]
) {
  Hasher hasher;
  const curve_info *curve = get_curve_by_name(SECP256K1_NAME);

  hasher_Init(&hasher, curve->hasher_sign);
  hasher_Update(&hasher, (const uint8_t *)coin->messageHeader, strlen(coin->messageHeader));
  uint8_t varint[5] = {0};
  uint32_t l = ser_length(message_len, varint);
  hasher_Update(&hasher, varint, l);
  hasher_Update(&hasher, message, message_len);
  hasher_Final(&hasher, hash);
}

int cryptoMessageSign(
    HDNode *node, uint32_t bipPurpose, uint32_t slip44, const uint8_t *message, size_t message_len,
    uint8_t *signature
) {
  uint8_t script_type_info = 0;
  switch (bipPurpose) {
    case 49:
      // segwit-in-p2sh
      script_type_info = 4;
      break;
    case 84:
      // segwit
      script_type_info = 8;
      break;
    default:
      script_type_info = 0;
      break;
  }

  uint8_t hash[HASHER_DIGEST_LENGTH] = {0};
  const BitcoinNetwork *coin = getBitcoinNetwork(slip44);
  cryptoMessageHash(coin, message, message_len, hash);

  uint8_t pby = 0;
  int result = hdnode_sign_digest(node, hash, signature + 1, &pby, NULL);
  if (result == 0) {
    signature[0] = 31 + pby + script_type_info;
  }
  return result;
}

std::string Bitcoin::getAddress(HDNode *node, uint32_t bipPurpose, uint32_t slip44) {
  char address[MAX_ADDRESS_LENGTH];

  const BitcoinNetwork *network = getBitcoinNetwork(slip44);

  /*
  if (bipPurpose == 86) {
    // Taproot address (P2TR/BIP86)
    uint8_t tweakedPubkey[HASH_LENGTH];
    zkp_bip340_tweak_public_key(node->public_key + 1, NULL, tweakedPubkey);
    if (!segwit_addr_encode(address, "bc", 1, tweakedPubkey, HASH_LENGTH)) {
      return "";
    }
  } else
  */
  if (bipPurpose == 84) {
    // BIP-84: Segwit P2WPKH address
    uint8_t rawAddress[HASH_LENGTH];
    hdnode_get_address_raw(node, network->addressTypeSegwit, rawAddress);

    if (!segwit_addr_encode(address, network->bech32Prefix, 0, rawAddress, ADDRESS_LENGTH)) {
      log_e("Failed to encode Segwit address");
      return "";
    }
  } else if (bipPurpose == 49) {
    // BIP-49: P2WPKH-nested-in-P2SH address
    ecdsa_get_address_segwit_p2sh(
        node->public_key, network->addressTypeP2SH, node->curve->hasher_pubkey,
        node->curve->hasher_base58, address, MAX_ADDRESS_LENGTH
    );
  } else {
    // BIP-44: legacy P2PKH address
    ecdsa_get_address(
        node->public_key, network->addressType, node->curve->hasher_pubkey,
        node->curve->hasher_base58, address, MAX_ADDRESS_LENGTH
    );
  }
  return std::string(address);
};
/*
 * END ported code from Trezor firmware
 */

std::string Bitcoin::signMessage(
    HDNode *node, std::string &message, uint32_t bipPurpose, uint32_t slip44
) {
  uint8_t signature[RECOVERABLE_SIGNATURE_LENGTH];
  const char *msg = message.c_str();

  if (cryptoMessageSign(node, bipPurpose, slip44, (uint8_t *)msg, message.length(), signature) !=
      0) {
    return "";
  }

  return toHex(signature, RECOVERABLE_SIGNATURE_LENGTH, true);
}
