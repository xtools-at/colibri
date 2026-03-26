// SPDX-License-Identifier: AGPL-3.0-or-later
// Contains ported code from Jade wallet firmware licensed under MIT, see notes below.

#pragma once

#include "bcur.h"

/*
 * START ported and/or adapted code from Jade wallet firmware (licensed under MIT, see
 * `licenses/Blockstream.MIT.txt`):
 * https://github.com/Blockstream/Jade/blob/348f972c77314fd6e2fc170d43168fef3cf65cf1/main/bcur.c
 */
#define BCUR_NUM_FRAGMENTS(num_pure_fragments) \
  (num_pure_fragments <= 300 ? 4 * num_pure_fragments / 3 : num_pure_fragments + 100)
#define BCUR_MAX_FRAGMENT_SIZE(capacity, typeLen) ((capacity - typeLen - 12 - 42) / 2)

static void encodeHdKey(
    CborEncoder* encoder, HDNode* node, const uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string& hdPath,
    uint8_t hdPathDepth, const uint32_t fingerprints[7]
) {
  // init key map
  CborEncoder key_map_encoder;
  CborError e = cbor_encoder_create_map(encoder, &key_map_encoder, 5);
  log_d("CBOR create result: %s", e == CborNoError ? "success" : "error");

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
      e = cbor_encoder_create_array(&key_path_map_encoder, &key_path_array_encoder, 2 * hdPathDepth);
      const char* hdPathCStr = hdPath.c_str();

      for (int i = 0; i < hdPathDepth; ++i) {
        bool isHardened = false;
        uint32_t pathSegment = extractHdPathSegment(hdPathCStr, i + 1, isHardened);

        e = cbor_encode_uint(&key_path_array_encoder, pathSegment);
        e = cbor_encode_boolean(&key_path_array_encoder, isHardened);
      }

      // Close the path array
      e = cbor_encoder_close_container(&key_path_map_encoder, &key_path_array_encoder);
      urfree_encoder(&key_path_array_encoder);
    }

    // - add account fingerprint
    e = cbor_encode_uint(&key_path_map_encoder, 2);
    e = cbor_encode_uint(&key_path_map_encoder, fingerprints[hdPathDepth]);

    // - add depth - BTC only
    e = cbor_encode_uint(&key_path_map_encoder, 3);
    e = cbor_encode_uint(&key_path_map_encoder, hdPathDepth);

    // close the key path map
    e = cbor_encoder_close_container(&key_map_encoder, &key_path_map_encoder);
    urfree_encoder(&key_path_map_encoder);
  }

  // parent fingerprint - immediate parent
  uint32_t parentFp = fingerprints[hdPathDepth > 1 ? hdPathDepth - 1 : 0];
  e = cbor_encode_uint(&key_map_encoder, 8);
  e = cbor_encode_uint(&key_map_encoder, parentFp);

  // set wallet name
  e = cbor_encode_uint(&key_map_encoder, 9);
  e = cbor_encode_text_stringz(&key_map_encoder, HW_DEVICE_NAME);

  // close the key map
  e = cbor_encoder_close_container(encoder, &key_map_encoder);
  log_d("CBOR close result: %s", e == CborNoError ? "success" : "error");
  urfree_encoder(&key_map_encoder);
}

static void encodeScriptVariantTag(CborEncoder* encoder, const uint32_t bipPurpose) {
  log_d("encoding script variant tag for purpose %d", bipPurpose);
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

static std::string encodeUr(UR& ur, size_t maxFragmentLen = 384) {
  UREncoder urEncoder = UREncoder(ur, BCUR_MAX_FRAGMENT_SIZE(maxFragmentLen, ur.type().length()));
  std::string result = urEncoder.next_part();
  if (!urEncoder.is_complete()) {
    log_e("UR encoding not complete after first part, but only single part expected for this use case");
  }

  toUpperCase(result);

  urfree_encoder(&urEncoder);

  return result;
}

std::string getUrHdKey(
    HDNode* node, const uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string& hdPath, const uint32_t fingerprints[7]
) {
  uint8_t cbor[128];
  CborEncoder encoder;
  cbor_encoder_init(&encoder, cbor, sizeof(cbor), 0);

  encodeHdKey(&encoder, node, pubkeyAccount, hdPath, 3, fingerprints);

  // create UR object
  size_t cborLen = cbor_encoder_get_buffer_size(&encoder, cbor);
  log_d("CBOR data length: %d", cborLen);
  UR ur(std::string(UR_TYPE_HDKEY), ByteVector(cbor, cbor + cborLen));

  urfree_encoder(&encoder);

  return encodeUr(ur);
}

// TODO: BTC
std::string getUrAccount(
    HDNode* node, const uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string& hdPath, const uint32_t fingerprints[7]
) {
  uint8_t cbor[128];
  CborEncoder encoder;
  cbor_encoder_init(&encoder, cbor, sizeof(cbor), 0);
  {
    uint8_t hdPathDepth = 3;
    // Fingerprint and list of output descriptors
    CborEncoder root_map_encoder;
    CborError e = cbor_encoder_create_map(&encoder, &root_map_encoder, 2);
    // ASSERT(e == CborNoError);
    log_d("CBOR create result: %s", e == CborNoError ? "success" : "error");

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
      log_d("CBOR array close result: %s", e == CborNoError ? "success" : "error");
      urfree_encoder(&key_array_encoder);
    }

    // Close the root map
    e = cbor_encoder_close_container(&encoder, &root_map_encoder);
    log_d("CBOR root map close result: %s", e == CborNoError ? "success" : "error");
    urfree_encoder(&root_map_encoder);
  }

  // create UR object
  size_t cborLen = cbor_encoder_get_buffer_size(&encoder, cbor);
  log_d("CBOR data length: %d", cborLen);
  UR ur(std::string(UR_TYPE_ACCOUNT), ByteVector(cbor, cbor + cborLen));

  urfree_encoder(&encoder);

  return encodeUr(ur);
}

/*
 * END ported code from Jade Wallet firmware
 */

// TODO: signature request response
std::string getUrEthSignature(const uint8_t* signature, const uint8_t uuid[16]) {
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
  urfree_encoder(&key_map_encoder);

  // get UR object
  size_t cborLen = cbor_encoder_get_buffer_size(&encoder, cbor);
  UR ur(std::string(UR_TYPE_ETH_SIGNATURE), ByteVector(cbor, cbor + cborLen));

  urfree_encoder(&encoder);

  return encodeUr(ur);
}

// `eth-sign-request`
int parseEthSignRequest(UR& ur, UrEthSignRequest& request) {
  // init parser
  CborParser parser;
  CborValue root;
  CborError e;

  cbor_parser_init(ur.cbor().data(), ur.cbor().size(), 0, &parser, &root);

  // open map
  CborValue map;
  cbor_value_enter_container(&root, &map);

  while (!cbor_value_at_end(&map)) {
    int64_t key = 0;
    CborType type = cbor_value_get_type(&map);

    if (type == CborIntegerType) {
      cbor_value_get_int64(&map, &key);
      cbor_value_advance(&map);

      while (cbor_value_get_type(&map) == CborTagType) {
        cbor_value_advance(&map);
      }
    }

    switch (key) {
      // uuid
      case 1: {
        if (cbor_value_get_type(&map) != CborByteStringType) {
          log_e("Expected byte string for request-id, got type %d", cbor_value_get_type(&map));
          break;
        }

        size_t len = 16;
        e = cbor_value_copy_byte_string(&map, request.id, &len, NULL);

        if (e != CborNoError) log_e("Error parsing request-id: %d", e);
        log_d("Parsed request-id (UUID): %s", toHex(request.id, len).c_str());

        break;
      }

      // sign data
      case 2: {
        if (cbor_value_get_type(&map) != CborByteStringType) {
          log_e("Expected byte string for data, got type %d", cbor_value_get_type(&map));
          break;
        }

        e = cbor_value_get_string_length(&map, &request.dataLen);
        e = cbor_value_copy_byte_string(&map, request.data, &request.dataLen, NULL);

        if (e != CborNoError) log_e("Error parsing sign data: %d", e);
        log_d("Parsed sign data (len=%d): %s", (int)request.dataLen, toHex(request.data, request.dataLen).c_str());

        break;
      }

      // sign data type: integer // 1 = legacy tx; 2 = typed data; 3 = message; 4 = typed tx
      case 3: {
        int64_t dataType;
        e = cbor_value_get_int64(&map, &dataType);
        request.dataType = (uint32_t)dataType;

        log_d("Parsed UR type: %d", request.dataType);

        break;
      }

      // Ethereum chain id
      case 4: {
        int64_t chainId;
        e = cbor_value_get_int64(&map, &chainId);
        request.chainId = (uint32_t)chainId;

        log_d("Parsed chain-id: %d", request.chainId);

        break;
      }

      // hdPath Map
      case 5: {
        // Map hdPath/fingerprint
        CborValue hdPathMap;
        if (!cbor_value_is_container(&map)) {
          log_e("Expected container for hdPath map, got type %d", cbor_value_get_type(&map));
          break;
        }
        e = cbor_value_enter_container(&map, &hdPathMap);
        if (e != CborNoError) {
          log_e("Error entering hdPath map: %d", e);
          break;
        }

        // 1 - hd path array
        e = cbor_value_advance(&hdPathMap);
        CborValue hdPathArray;
        if (!cbor_value_is_container(&hdPathMap)) {
          log_e("Expected container for hdPath array, got type %d", cbor_value_get_type(&hdPathMap));
          break;
        }
        e = cbor_value_enter_container(&hdPathMap, &hdPathArray);
        if (e != CborNoError) {
          log_e("Error entering hdPath array: %d", e);
          break;
        }

        std::string hdPathStr = "m";
        while (!cbor_value_at_end(&hdPathArray)) {
          if (cbor_value_get_type(&hdPathArray) == CborIntegerType) {
            int64_t pathSegment;
            cbor_value_get_int64(&hdPathArray, &pathSegment);
            log_d("Parsed path segment: %d", (int)pathSegment);

            hdPathStr += "/" + std::to_string(pathSegment);
          } else if (cbor_value_is_boolean(&hdPathArray)) {
            bool res;
            cbor_value_get_boolean(&hdPathArray, &res);
            log_d("isHardened: %s", res ? "true" : "false");

            hdPathStr += (res ? "'" : "");
          } else {
            log_e(
                "Unexpected type in hdPath array, expected int and bool pairs, got type %d\n",
                cbor_value_get_type(&hdPathArray)
            );
            break;
          }

          cbor_value_advance(&hdPathArray);
        }
        request.hdPath = hdPathStr;
        log_d("Parsed hdPath: %s", request.hdPath.c_str());

        e = cbor_value_leave_container(&hdPathMap, &hdPathArray);

        // 2 - account fingerprint
        cbor_value_advance(&hdPathMap);
        int64_t accountFingerprint;
        e = cbor_value_get_int64(&hdPathMap, &accountFingerprint);
        uint32_t fp = (uint32_t)accountFingerprint;

        request.fingerprint = fp;
        log_d("Parsed account fingerprint: %u / 0x%08x", fp, fp);

        // cleanup
        while (!cbor_value_at_end(&hdPathMap)) {
          cbor_value_advance(&hdPathMap);
        }
        e = cbor_value_leave_container(&map, &hdPathMap);

        continue;
      }

      // wallet address
      case 6: {
        if (cbor_value_get_type(&map) != CborByteStringType) {
          log_e("Expected byte string for address, got type %d", cbor_value_get_type(&map));
          continue;
        }

        size_t len = ADDRESS_LENGTH;
        e = cbor_value_copy_byte_string(&map, request.address, &len, NULL);

        if (e != CborNoError) log_e("Error parsing address: %d", e);
        log_d("Parsed address: %s", toHex(request.address, len).c_str());

        break;
      }
    }
    cbor_value_advance(&map);
  }

  return e;
}
