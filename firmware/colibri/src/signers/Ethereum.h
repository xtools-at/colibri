// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"
#include "../utils/crypto.h"

class Ethereum {
 public:
  Ethereum() {};

  std::string getAddress(HDNode *node);
  std::string signMessage(HDNode *node, std::string &message);
  std::string signTypedDataHash(
      HDNode *node, std::string &domainSeparatorHash, std::string &messageHash
  );
  TxResponse signTransaction(HDNode *node, EthereumTxInput *input);

 private:
  std::string signRecoverableDigest(HDNode *node, uint8_t digest[HASH_LENGTH]);
};
