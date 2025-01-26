// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"

#ifdef INTERFACE_QR

  #include <cdecoder.h>
  #include <cencoder.h>
  #include <tinycbor.h>

  #include <bc-ur.hpp>

  #include "../../constants.h"
  #include "./chains.h"

extern "C" {
  #include <bip32.h>
}

using namespace ur;

UR getUrHdKey(
    HDNode *node, const uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string &hdPath,
    uint8_t hdPathDepth, const uint32_t fingerprints[7]
);

UR getUrAccount(
    HDNode *node, const uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string &hdPath,
    uint8_t hdPathDepth, const uint32_t fingerprints[7]
);

UR getUrEthSignature(const uint8_t *signature, const uint8_t uuid[16] = {0});

UREncoder getUREncoder(UR &ur, size_t maxFragmentLen = 114);

void parseUr(UR &ur);  // TODO: implement request parsing

/*
struct EthSignRequest {
  std::vector<uint8_t> signData;
  uint32_t dataType;
  uint32_t chainId;
  std::string derivationPath;
  std::vector<uint8_t> requestId;  // optional UUID
  std::vector<uint8_t> address;  // optional
  std::string origin;  // optional
};
*/

#endif
