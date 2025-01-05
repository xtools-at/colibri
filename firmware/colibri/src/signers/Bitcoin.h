// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"
#include "../utils/crypto.h"

extern "C" {
#include <segwit_addr.h>
// #include <zkp_bip340.h>
}

class Bitcoin {
 public:
  Bitcoin() {};

  std::string getAddress(HDNode *node, uint32_t bipPurpose, uint32_t slip44);
  std::string signMessage(HDNode *node, std::string &message, uint32_t bipPurpose, uint32_t slip44);
};
