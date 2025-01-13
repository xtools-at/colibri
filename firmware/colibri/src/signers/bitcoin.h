// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"
#include "../utils/crypto.h"

extern "C" {
#include <segwit_addr.h>
// #include <zkp_bip340.h>
}

std::string btcGetAddress(HDNode *node, uint32_t bipPurpose, uint32_t slip44);
std::string btcSignMessage(
    HDNode *node, std::string &message, uint32_t bipPurpose, uint32_t slip44
);
