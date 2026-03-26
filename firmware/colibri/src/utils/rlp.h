// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <stddef.h>
#include <stdint.h>

#include "../../config.h"
#include "../../types.h"
#include "helpers.h"

typedef struct {
  const uint8_t* ptr;
  size_t len;
} RLPItem;

typedef struct {
  const uint8_t* buffer;
  size_t length;
  size_t offset;
} RLPParser;

bool decodeLegacyTx(const uint8_t* data, size_t dataLen, EthTxData& tx);

bool decodeEip1559Tx(const uint8_t* data, size_t dataLen, EthTxData& tx);
