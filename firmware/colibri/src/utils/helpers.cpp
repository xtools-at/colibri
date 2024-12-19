// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "helpers.h"

bool isAllZero(const uint8_t* value, size_t len) {
  if (len == 0) return true;

  for (size_t i = 0; i < len; i++) {
    if (value[i] != 0) {
      return false;
    }
  }

  return true;
}
