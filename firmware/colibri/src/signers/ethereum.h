// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <ArduinoJson.h>

#include "../../config.h"
#include "../../constants.h"
#include "../utils/crypto.h"

std::string ethGetAddress(HDNode *node);
std::string ethSignMessage(HDNode *node, std::string &message);
std::string ethSignTypedDataHash(
    HDNode *node, std::string &domainSeparatorHash, std::string &messageHash
);
WalletResponse ethSignTransaction(HDNode *node, JsonArrayConst input);
