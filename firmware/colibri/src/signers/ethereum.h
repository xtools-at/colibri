// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <ArduinoJson.h>

#include "../../config.h"
#include "../../constants.h"
#include "../utils/crypto.h"

std::string ethGetAddress(HDNode* node);
std::string ethSignMessage(HDNode* node, std::string& message);
int ethSignMessage(
    HDNode* node, const uint8_t* messageBytes, size_t messageLen, uint8_t signature[RECOVERABLE_SIGNATURE_LENGTH]
);
std::string ethSignTypedDataHash(HDNode* node, std::string& domainSeparatorHash, std::string& messageHash);
WalletResponse ethSignTransaction(HDNode* node, EthTxData& tx, uint8_t signature[RECOVERABLE_SIGNATURE_LENGTH] = {0});
int ethSignRecoverableDigest(
    HDNode* node, uint8_t digest[HASH_LENGTH], uint8_t signature[RECOVERABLE_SIGNATURE_LENGTH]
);
