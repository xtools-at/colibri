// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"
#include "../utils/crypto.h"

#define POLKADOT_CKS_LENGTH 2
#define POLKADOT_UNKNOWN_NETWORK 0x4004

std::string dotGetAddress(
    HDNode *node, uint32_t slip44, uint16_t networkPrefixOverride = POLKADOT_UNKNOWN_NETWORK
);
