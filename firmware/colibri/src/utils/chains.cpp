#pragma once

#include "chains.h"

const int slip44Ethereum[] = {
    60,  // Ethereum
    1,  // Testnets
    714,  // Binance Smart Chain
    966,  // Polygon
    61,  // Ethereum Classic
};

ChainType getChainType(uint32_t slip44) {
  size_t len = sizeof(slip44Ethereum) / sizeof(slip44Ethereum[0]);

  for (int i = 0; i < len; i++) {
    if (slip44 == slip44Ethereum[i]) {
      return ChainType::ETH;
    }
  }

  return ChainType::BTC;
}
