// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

/*
 * For more information on the BC-UR specification and the EIP-4527 extension, see also:
 * - https://developer.blockchaincommons.com/ur/
 * - https://eips.ethereum.org/EIPS/eip-4527
 */

#include "bcur.h"

/*
 * START ported and/or adapted code from Jade wallet firmware (licensed under MIT, see
 * `licenses/Blockstream.MIT.txt`):
 * https://github.com/Blockstream/Jade/blob/348f972c77314fd6e2fc170d43168fef3cf65cf1/main/bcur.c
 */

#define BCUR_NUM_FRAGMENTS(num_pure_fragments) \
  (num_pure_fragments <= 300 ? 4 * num_pure_fragments / 3 : num_pure_fragments + 100)

#define BCUR_MAX_FRAGMENT_SIZE(capacity, type) ((capacity - strlen(type) - 12 - 42) / 2)

static std::vector<std::string> createUr(const char *urType, uint8_t *cbor, size_t cborLen) {
  // create UR
  uint8_t urEncoder[URENCODER_SIZE];
  urcreate_placement_encoder(
      urEncoder, sizeof(urEncoder), urType, cbor, cborLen, BCUR_MAX_FRAGMENT_SIZE(114, urType), 0, 8
  );  // 114 -> QR code v4 max capacity
  const size_t min_num_fragments =
      urseqlen_encoder(urEncoder);  // the number of 'pure' data fragments
  const size_t num_fragments =
      BCUR_NUM_FRAGMENTS(min_num_fragments);  // add some fountain-code fragments
  // ASSERT(num_fragments >= min_num_fragments);

  // encode all fragments
  const bool forceUppercase = true;
  std::vector<std::string> ursVector;
  for (int i = 0; i < num_fragments; ++i) {
    char *fragment = NULL;
    urnext_part_encoder(urEncoder, forceUppercase, &fragment);

    // push encoded fragment into vector
    log_d("Encoded UR fragment #%d: %s", i, fragment);
    ursVector.push_back(std::string(fragment));

    // free memory
    urfree_encoded_encoder(fragment);
  }

  return ursVector;
}

static void encodeHdKey(
    CborEncoder *encoder, HDNode *node, uint8_t pubkeyAccount[PUBLICKEY_LENGTH],
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
    default:  // 44 / P2PKH
      cbor_encode_tag(encoder, 403);
  }
}

std::vector<std::string> getUrHdKey(
    HDNode *node, uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string &hdPath, uint8_t hdPathDepth,
    const uint32_t fingerprints[7]
) {
  uint8_t cbor[128];

  CborEncoder encoder;
  cbor_encoder_init(&encoder, cbor, sizeof(cbor), 0);

  encodeHdKey(&encoder, node, pubkeyAccount, hdPath, hdPathDepth, fingerprints);

  // get CBOR length
  size_t cborLen = cbor_encoder_get_buffer_size(&encoder, cbor);

  return createUr(UR_TYPE_HDKEY, cbor, cborLen);
}

std::vector<std::string> getUrAccount(
    HDNode *node, uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string &hdPath, uint8_t hdPathDepth,
    const uint32_t fingerprints[7]
) {
  CborEncoder encoder;
  uint8_t cbor[128];
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
    }

    // Close the root map
    e = cbor_encoder_close_container(&encoder, &root_map_encoder);
    // ASSERT(e == CborNoError);
  }
  size_t cborLen = cbor_encoder_get_buffer_size(&encoder, cbor);

  return createUr(UR_TYPE_ACCOUNT, cbor, cborLen);
}

/*
 * END ported code from Jade Wallet firmware
 */

std::vector<std::string> getUrEthSignature(const uint8_t *signature, const uint8_t uuid[16]) {
  CborEncoder encoder;
  uint8_t cbor[128];

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

  // get CBOR length
  size_t cborLen = cbor_encoder_get_buffer_size(&encoder, cbor);

  return createUr(UR_TYPE_ETH_SIGNATURE, cbor, cborLen);
}

/*
 * UR decoding
 */
BcUrDecoder::BcUrDecoder() { init(); }

void BcUrDecoder::init() { urcreate_placement_decoder(&decoderCtx, URDECODER_SIZE); }

bool BcUrDecoder::addFragment(const std::string &fragment) {
  if (isComplete()) {
    return false;
  }

  // check if BC-UR fragment
  const size_t prefixLen = strlen(UR_PREFIX);
  if (fragment.length() <= prefixLen ||
      std::equal(
          fragment.begin(), fragment.begin() + prefixLen, UR_PREFIX, UR_PREFIX + prefixLen,
          [](char a, char b) { return tolower(a) == tolower(b); }
      )) {
    return false;
  }

  // receive BC-UR fragment
  return urreceive_part_decoder(decoderCtx, fragment.c_str()) == 0;
}

bool BcUrDecoder::isComplete() const {
  bool complete = uris_complete_decoder((uint8_t *)decoderCtx);

  return complete;
}

std::vector<uint8_t> BcUrDecoder::decode() {
  if (!isComplete()) {
    return std::vector<uint8_t>();
  }

  uint8_t *data;
  size_t dataLen;
  urresult_ur_decoder(&decoderCtx, &data, &dataLen, &urType);

  urfree_placement_decoder(&decoderCtx);

  return std::vector<uint8_t>(data, data + dataLen);
}

static void parseEthSignRequest(CborValue &map, EthSignRequest &request) {
  // `eth-sign-request`
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
        request.signData = std::vector<uint8_t>(data, data + len);
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

EthSignRequest EthSignRequestDecoder::parseRequest() {
  EthSignRequest request;
  CborParser parser;
  CborValue value;

  if (!isComplete()) {
    return request;
  }

  std::vector<uint8_t> cbor = decode();

  cbor_parser_init(cbor.data(), cbor.size(), 0, &parser, &value);

  CborValue map;
  cbor_value_enter_container(&value, &map);

  // check UR type and parse accordingly
  if (strncasecmp(urType, UR_TYPE_ETH_SIGNATURE, sizeof(UR_TYPE_ETH_SIGNATURE)) == 0) {
    parseEthSignRequest(map, request);
  }

  return request;
}

/*
// Example usage
void processUrFragment(const std::string &fragment) {
  static EthSignRequestDecoder decoder;

  if (decoder.addFragment(fragment)) {
    if (decoder.isComplete()) {
      EthSignRequest request = decoder.parseRequest();
      // Process request
    }
  }
}
*/
