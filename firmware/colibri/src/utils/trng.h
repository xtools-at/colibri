// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <esp_random.h>

#include "../../config.h"

#if defined(RNG_ANTENNA_DISABLED)
  #include <WiFi.h>
#endif

extern "C" {
#include <memzero.h>
#include <rand.h>
#include <sha3.h>
}

void generateEntropy(uint8_t *result, uint8_t entropyLength = HASH_LENGTH);
uint32_t randomNumber();
uint32_t randomNumber(uint32_t min, uint32_t max);
