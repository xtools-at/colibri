// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

/*
 * For more information on the BC-UR specification and the EIP-4527 extension, see also:
 * - https://developer.blockchaincommons.com/ur/
 * - https://github.com/BlockchainCommons/bc-ur
 * - https://eips.ethereum.org/EIPS/eip-4527
 */

#include "bcur.h"

#ifdef INTERFACE_QR

/*
 * START ported and/or adapted code from Jade wallet firmware (licensed under MIT, see
 * `licenses/Blockstream.MIT.txt`):
 * https://github.com/Blockstream/Jade/blob/348f972c77314fd6e2fc170d43168fef3cf65cf1/main/bcur.c
 */
  #define BCUR_NUM_FRAGMENTS(num_pure_fragments) \
    (num_pure_fragments <= 300 ? 4 * num_pure_fragments / 3 : num_pure_fragments + 100)
  #define BCUR_MAX_FRAGMENT_SIZE(capacity, type) ((capacity - strlen(type) - 12 - 42) / 2)

static void encodeHdKey(
    CborEncoder *encoder, HDNode *node, const uint8_t pubkeyAccount[PUBLICKEY_LENGTH],
    std::string &hdPath, uint8_t hdPathDepth, const uint32_t fingerprints[7]
) {
  // init key map
  CborEncoder key_map_encoder;
  CborError e = cbor_encoder_create_map(encoder, &key_map_encoder, 5);

  // account pubkey
  e = cbor_encode_uint(&key_map_encoder, 3);
  e = cbor_encode_byte_string(&key_map_encoder, pubkeyAccount, PUBLICKEY_LENGTH);

  // chaincode
  e = cbor_encode_uint(&key_map_encoder, 4);
  e = cbor_encode_byte_string(&key_map_encoder, node->chain_code, sizeof(node->chain_code));

  // key path
  e = cbor_encode_uint(&key_map_encoder, 6);
  {
    // key path
    cbor_encode_tag(&key_map_encoder, 304);
    CborEncoder key_path_map_encoder;
    e = cbor_encoder_create_map(&key_map_encoder, &key_path_map_encoder, 3);

    // - path array
    {
      e = cbor_encode_uint(&key_path_map_encoder, 1);
      CborEncoder key_path_array_encoder;
      e = cbor_encoder_create_array(
          &key_path_map_encoder, &key_path_array_encoder, 2 * hdPathDepth
      );
      for (int i = 0; i < hdPathDepth; ++i) {
        bool isHardened = false;
        uint32_t pathSegment = extractHdPathSegment(hdPath.c_str(), i + 1, isHardened);

        e = cbor_encode_uint(&key_path_array_encoder, pathSegment);
        e = cbor_encode_boolean(&key_path_array_encoder, isHardened);
      }

      // Close the path array
      e = cbor_encoder_close_container(&key_path_map_encoder, &key_path_array_encoder);
    }

    // TODO: - add account fingerprint
    e = cbor_encode_uint(&key_path_map_encoder, 2);
    e = cbor_encode_uint(&key_path_map_encoder, fingerprints[hdPathDepth]);

    // TODO: - add depth - BTC only
    e = cbor_encode_uint(&key_path_map_encoder, 3);
    e = cbor_encode_uint(&key_path_map_encoder, hdPathDepth);
  }

  // TODO: parent fingerprint - immediate parent
  uint32_t parentFp = fingerprints[hdPathDepth > 1 ? hdPathDepth - 1 : 0];
  e = cbor_encode_uint(&key_map_encoder, 8);
  e = cbor_encode_uint(&key_map_encoder, parentFp);

  // set wallet name
  e = cbor_encode_uint(&key_map_encoder, 9);
  e = cbor_encode_text_stringz(&key_map_encoder, HW_DEVICE_NAME);

  // close the key map
  e = cbor_encoder_close_container(encoder, &key_map_encoder);
}

static void encodeScriptVariantTag(CborEncoder *encoder, const uint32_t bipPurpose) {
  switch (bipPurpose) {
    case 84:  // P2WPKH
      cbor_encode_tag(encoder, 404);
      break;
    case 49:  // P2SH-P2WPKH
      cbor_encode_tag(encoder, 400);
      cbor_encode_tag(encoder, 404);
      break;
    default:  // case 44: P2PKH
      cbor_encode_tag(encoder, 403);
      break;
  }
}

UR getUrHdKey(
    HDNode *node, uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string &hdPath, uint8_t hdPathDepth,
    const uint32_t fingerprints[7]
) {
  uint8_t cbor[128];
  CborEncoder encoder;
  cbor_encoder_init(&encoder, cbor, sizeof(cbor), 0);

  encodeHdKey(&encoder, node, pubkeyAccount, hdPath, hdPathDepth, fingerprints);

  // create UR object
  size_t cborLen = cbor_encoder_get_buffer_size(&encoder, cbor);
  UR ur(std::string(UR_TYPE_HDKEY), ByteVector(cbor, cbor + cborLen));

  urfree_encoder(&encoder);

  return ur;
}

UR getUrAccount(
    HDNode *node, const uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string &hdPath,
    uint8_t hdPathDepth, const uint32_t fingerprints[7]
) {
  uint8_t cbor[128];
  CborEncoder encoder;
  cbor_encoder_init(&encoder, cbor, 128, 0);
  {
    // Fingerprint and list of output descriptors
    CborEncoder root_map_encoder;
    CborError e = cbor_encoder_create_map(&encoder, &root_map_encoder, 2);
    // ASSERT(e == CborNoError);

    // fingerprint - immediate parent, or root parent of the path given ?
    e = cbor_encode_uint(&root_map_encoder, 1);
    e = cbor_encode_uint(&root_map_encoder, fingerprints[hdPathDepth - 1]);

    e = cbor_encode_uint(&root_map_encoder, 2);
    {
      // Just one output descriptor
      CborEncoder key_array_encoder;
      e = cbor_encoder_create_array(&root_map_encoder, &key_array_encoder, 1);

      // script-type tag(s)
      encodeScriptVariantTag(&key_array_encoder, extractHdPathSegment(hdPath.c_str(), 1));

      // Single hdkey
      cbor_encode_tag(&key_array_encoder, 303);
      encodeHdKey(&encoder, node, pubkeyAccount, hdPath, hdPathDepth, fingerprints);

      // Close the array
      e = cbor_encoder_close_container(&root_map_encoder, &key_array_encoder);
      urfree_encoder(&key_array_encoder);
    }

    // Close the root map
    e = cbor_encoder_close_container(&encoder, &root_map_encoder);
    // ASSERT(e == CborNoError);
    urfree_encoder(&root_map_encoder);
  }

  // create UR object
  size_t cborLen = cbor_encoder_get_buffer_size(&encoder, cbor);
  UR ur(std::string(UR_TYPE_ACCOUNT), ByteVector(cbor, cbor + cborLen));

  urfree_encoder(&encoder);

  return ur;
}

/*
 * END ported code from Jade Wallet firmware
 */

UR getUrEthSignature(const uint8_t *signature, const uint8_t uuid[16]) {
  uint8_t cbor[128];
  CborEncoder encoder;

  // init
  cbor_encoder_init(&encoder, cbor, 128, 0);
  CborEncoder key_map_encoder;
  CborError e = cbor_encoder_create_map(&encoder, &key_map_encoder, 2);

  // uuid
  e = cbor_encode_uint(&key_map_encoder, 1);
  e = cbor_encode_byte_string(&key_map_encoder, uuid, 16);

  // signature
  e = cbor_encode_uint(&key_map_encoder, 2);
  e = cbor_encode_byte_string(&key_map_encoder, signature, RECOVERABLE_SIGNATURE_LENGTH);

  // close
  e = cbor_encoder_close_container(&encoder, &key_map_encoder);

  // get UR object
  size_t cborLen = cbor_encoder_get_buffer_size(&encoder, cbor);
  UR ur(std::string(UR_TYPE_ETH_SIGNATURE), ByteVector(cbor, cbor + cborLen));

  urfree_encoder(&encoder);
  urfree_encoder(&key_map_encoder);

  return ur;
}

UREncoder getUREncoder(UR &ur, size_t maxFragmentLen) {
  return UREncoder(ur, BCUR_MAX_FRAGMENT_SIZE(maxFragmentLen, ur.type().c_str()), 0, 8);
}

// ========= UR Decoding =========

// TODO: `eth-sign-request`
static void parseEthSignRequest(UR &ur) {
  // init parser
  CborParser parser;
  CborValue value;
  cbor_parser_init(ur.cbor().data(), ur.cbor().size(), 0, &parser, &value);

  // open map
  CborValue map;
  cbor_value_enter_container(&value, &map);

  while (!cbor_value_at_end(&map)) {
    uint64_t key;
    cbor_value_get_uint64(&map, &key);
    cbor_value_advance(&map);

    switch (key) {
      case 1:  // request-id: uuid/bytes[16]
        break;
      case 2: {  // sign-data: bytes - contents depend on `data-type`
        size_t len;
        cbor_value_get_string_length(&map, &len);
        uint8_t data[len];
        cbor_value_copy_byte_string(&map, data, &len, NULL);
        // request.signData = std::vector<uint8_t>(data, data + len);
        break;
      }
      case 3:  // data-type: integer // 1 = legacy tx; 2 = typed data; 3 = message; 4 = typed tx
        break;
      case 4:  // Ethereum chain-id: integer
        break;
      case 5:  // derivation path: #5.304(crypto-keypath)
        break;
      case 6:  // Ethereum address: bytes[20]
        break;
      case 7:  // origin string
        break;
    }
    cbor_value_advance(&map);
  }
}

void parseUr(UR &ur) {
  const char *urType = ur.type().c_str();

  // parse different action types
  if (!strncasecmp(urType, UR_TYPE_ETH_SIGN_REQUEST, sizeof(UR_TYPE_ETH_SIGN_REQUEST))) {
    parseEthSignRequest(ur);
  } else {
    // unknown type
  }
}

/*
void howToUse() {
  // init, add parts
  URDecoder urDecoder;
  urDecoder.receive_part("ur:foo/2345");
  urDecoder.receive_part("ur:foo/6789");

  // create UR object if complete
  if (urDecoder.is_complete()) {
    if (urDecoder.is_failure()) return;

    const UR ur = urDecoder.result_ur();
    parseUr(ur);
  }
}
*/

#endif
