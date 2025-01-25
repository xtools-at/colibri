// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <map>

#include "../../config.h"

extern "C" {
#include <curves.h>
}

ChainType getChainType(uint32_t slip44);
const char* getChainCurveType(ChainType chainType);
const BitcoinNetwork* getBitcoinNetwork(uint32_t slip44);
const uint16_t getPolkadotNetwork(uint32_t slip44);
uint32_t extractHdPathSegment(const char* hdPath, uint8_t seg, bool& isHardened);
uint32_t extractHdPathSegment(const char* hdPath, uint8_t seg);
uint32_t getSlip44(const char* hdPath);
uint32_t getXpubMagicNumber(const char* hdPath);
