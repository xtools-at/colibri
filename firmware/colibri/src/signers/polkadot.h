// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"
#include "../utils/crypto.h"

std::string dotGetAddress(HDNode *node, uint32_t slip44, uint8_t prefixOverride = 0);
