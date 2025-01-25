// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <cdecoder.h>
#include <cencoder.h>
#include <tinycbor.h>

#include "../../config.h"
#include "../../constants.h"
#include "./chains.h"

extern "C" {
#include <bip32.h>
}

std::vector<std::string> getUrHdKey(
    HDNode *node, uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string &hdPath, uint8_t hdPathDepth,
    const uint32_t fingerprints[7]
);
std::vector<std::string> getUrAccount(
    HDNode *node, uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string &hdPath, uint8_t hdPathDepth,
    const uint32_t fingerprints[7]
);

std::vector<std::string> getUrEthSignature(const uint8_t *signature, const uint8_t uuid[16] = {0});

// bool decodeEthSignRequest();
struct EthSignRequest {
  std::vector<uint8_t> signData;
  uint32_t dataType;
  uint32_t chainId;
  std::string derivationPath;
  std::vector<uint8_t> requestId;  // optional UUID
  std::vector<uint8_t> address;  // optional
  std::string origin;  // optional
};

class BcUrDecoder {
 public:
  const char *urType;

  BcUrDecoder();
  virtual ~BcUrDecoder() = default;
  void init();
  bool addFragment(const std::string &fragment);
  bool isComplete() const;
  std::vector<uint8_t> decode();

 protected:
  uint8_t decoderCtx[URDECODER_SIZE];
};

class EthSignRequestDecoder : public BcUrDecoder {
 public:
  EthSignRequestDecoder() : BcUrDecoder() {};
  EthSignRequest parseRequest();
};
