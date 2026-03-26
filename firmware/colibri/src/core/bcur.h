// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

/*
 * For more information on the BC-UR specification and the EIP-4527 extension, see also:
 * - https://developer.blockchaincommons.com/ur/
 * - https://github.com/BlockchainCommons/bc-ur
 * - https://eips.ethereum.org/EIPS/eip-4527
 */

#include "../../config.h"
// config first

#include <tinycbor.h>

#include <bc-ur.hpp>

#include "../../constants.h"
#include "../utils/chains.h"
#include "../utils/helpers.h"
#include "../utils/rlp.h"

extern "C" {
#include <bip32.h>
#include <cdecoder.h>
#include <cencoder.h>
}

using namespace ur;

std::string getUrHdKey(
    HDNode* node, const uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string& hdPath, const uint32_t fingerprints[7]
);

std::string getUrAccount(
    HDNode* node, const uint8_t pubkeyAccount[PUBLICKEY_LENGTH], std::string& hdPath, const uint32_t fingerprints[7]
);

std::string getUrEthSignature(const uint8_t* signature, const uint8_t uuid[16] = {0});

int parseEthSignRequest(UR& ur, UrEthSignRequest& request);
