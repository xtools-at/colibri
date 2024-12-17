// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "AbstractChain.h"
// needs to be included first

class EthereumChain : public Chain {
 public:
  EthereumChain() : Chain() {};

  std::string getAddress(HDNode *node);
  std::string signMessage(HDNode *node, std::string &message);
  std::string signTypedDataHash(
      HDNode *node, std::string &domainSeparatorHash, std::string &messageHash
  );

 private:
  std::string signDigest(HDNode *node, uint8_t digest[HASH_LENGTH]);
};
