// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"
#include "../utils/crypto.h"

// ========== Abstract Chain Class ========== //
class Chain {
 public:
  Chain() {}

  virtual std::string getAddress(HDNode *node) = 0;
  virtual std::string signMessage(HDNode *node, std::string &message) = 0;

 private:
  virtual std::string signDigest(HDNode *node, uint8_t digest[HASH_LENGTH]) = 0;
};
