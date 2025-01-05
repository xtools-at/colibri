// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"

bool isAllZero(const uint8_t *value, size_t len);
std::string toHex(const uint8_t *data, size_t len, bool add0xPrefix = false);
size_t fromHex(const char *hex, uint8_t *bytes, size_t maxBytesLen = 0);
void uint32ToBytes(uint32_t value, uint8_t output[4]);
