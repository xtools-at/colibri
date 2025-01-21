// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "chains.h"

const int slip44Ethereum[] = {
    60,  // Ethereum
    61,  // Ethereum Classic
    714,  // Binance Smart Chain
    966,  // Polygon
};

const std::map<uint32_t, uint16_t> slip44ToPolkadotNetwork = {
    {354, 0},  // Polkadot
    {434, 2},  // Kusama
    {810, 4},  // Astar
    {788, 6},  // Bifrost
    {686, 8},  // Karura
    {787, 10},  // Acala
    {595, 12},  // Polymesh
    {2015, 13},  // Integritee
    {747, 36},  // Centrifuge
    {1003, 37},  // Nodle
    {3338, 1221},  // Peaq
    {794, 2032},  // Interlay
    // {?, 66},  // Crust
    // {?, 77},  // Manta
    // {?, 128},  // Clover
    // {?, 172},  // Parallel Finance
    // {?, 1328},  // Ajuna
};

/*
 * START ported and/or adapted code from Trezor firmware (originally licensed under GPL 3.0, see
 * `licenses/GPL-3.0.txt`):
 * https://github.com/trezor/trezor-firmware/blob/29e03bd873977a498dbce79616bfb3fe4b7a0698/legacy/firmware/crypto.c
 * https://github.com/trezor/trezor-firmware/blob/29e03bd873977a498dbce79616bfb3fe4b7a0698/legacy/firmware/coin_info.c.mako

 */

const BitcoinNetwork bitcoinNetworks[7] = {
    {
        .name = "BTC",
        // the "\x18" prefix is computed by taking the length of the header (here: 24) and
        // converting it to hex (0x18)
        .messageHeader = "\x18"
                         "Bitcoin Signed Message:\n",
        .bech32Prefix = "bc",

        .hasSegwit = true,
        .hasTaproot = true,
        .maxfeeKb = 2000000,

        .addressType = 0,
        .addressTypeP2SH = 5,

        .xpubMagic = 76067358,  // 0x0488b21e
        .xpubMagicP2SH = 77429938,  // 0x049d7cb2
        .xpubMagicSegwit = 78792518,  // 0x04b24746

        .slip44 = 0,
    },
    {
        .name = "TEST BTC",
        .messageHeader = "\x18"
                         "Bitcoin Signed Message:\n",
        .bech32Prefix = "tb",

        .hasSegwit = true,
        .hasTaproot = true,
        .maxfeeKb = 10000000,

        .addressType = 111,
        .addressTypeP2SH = 196,

        .xpubMagic = 70617039,
        .xpubMagicP2SH = 71979618,
        .xpubMagicSegwit = 73342198,

        .slip44 = 1,
    },
    {
        .name = "LTC",
        .messageHeader = "\x19"
                         "Litecoin Signed Message:\n",
        .bech32Prefix = "ltc",

        .hasSegwit = true,
        .hasTaproot = false,
        .maxfeeKb = 67000000,

        .addressType = 48,
        .addressTypeP2SH = 50,
        .xpubMagic = 27108450,
        .xpubMagicP2SH = 28471030,
        .xpubMagicSegwit = 78792518,

        .slip44 = 2,
    },
    {
        .name = "DOGE",
        .messageHeader = "\x19"
                         "Dogecoin Signed Message:\n",
        .bech32Prefix = "doge",

        .hasSegwit = false,
        .hasTaproot = false,
        .maxfeeKb = 1200000000000,

        .addressType = 30,
        .addressTypeP2SH = 22,
        .xpubMagic = 49990397,

        .slip44 = 3,
    },
    {
        .name = "DASH",
        .messageHeader = "\x19"
                         "DarkCoin Signed Message:\n",
        .bech32Prefix = "dash",

        .hasSegwit = false,
        .hasTaproot = false,
        .maxfeeKb = 45000000,

        .addressType = 76,
        .addressTypeP2SH = 16,
        .xpubMagic = 50221772,

        .slip44 = 5,
    },
    {
        .name = "DGB",
        .messageHeader = "\x19"
                         "DigiByte Signed Message:\n",
        .bech32Prefix = "dgb",

        .hasSegwit = true,
        .hasTaproot = false,
        .maxfeeKb = 130000000000,

        .addressType = 30,
        .addressTypeP2SH = 63,
        .xpubMagic = 76067358,
        .xpubMagicP2SH = 77429938,
        .xpubMagicSegwit = 78792518,

        .slip44 = 20,
    },
    {
        .name = "ZEC",
        .messageHeader = "\x16"
                         "Zcash Signed Message:\n",
        .bech32Prefix = "zec",

        .hasSegwit = false,
        .hasTaproot = false,
        .maxfeeKb = 51000000,

        .addressType = 7352,
        .addressTypeP2SH = 7357,
        .xpubMagic = 76067358,

        .slip44 = 133,
    }
};
/*
 * END ported code from Trezor firmware
 */

ChainType getChainType(uint32_t slip44) {
  // other chain types
  switch (slip44) {
    case 501:  // Solana
      return ChainType::SOL;
    default:
      break;
  }

  // polkadot chain type
  if (slip44ToPolkadotNetwork.find(slip44) != slip44ToPolkadotNetwork.end()) {
    return ChainType::DOT;
  }

  // ethereum chain type
  size_t len = sizeof(slip44Ethereum) / sizeof(slip44Ethereum[0]);
  for (uint16_t i = 0; i < len; i++) {
    if (slip44 == slip44Ethereum[i]) {
      return ChainType::ETH;
    }
  }

  // fallback: bitcoin chain type
  return ChainType::BTC;
}

const char* getChainCurveType(ChainType chainType) {
  switch (chainType) {
    case ChainType::SOL:
    case ChainType::DOT:
      return ED25519_NAME;
    default:
      return SECP256K1_NAME;
  }
}

const BitcoinNetwork* getBitcoinNetwork(uint32_t slip44) {
  size_t len = sizeof(bitcoinNetworks) / sizeof(bitcoinNetworks[0]);

  for (uint16_t i = 0; i < len; i++) {
    if (slip44 == bitcoinNetworks[i].slip44) {
      return &(bitcoinNetworks[i]);
    }
  }

  return &(bitcoinNetworks[0]);
}

const uint16_t getPolkadotNetwork(uint32_t slip44) {
  if (slip44ToPolkadotNetwork.find(slip44) != slip44ToPolkadotNetwork.end()) {
    return slip44ToPolkadotNetwork.at(slip44);
  }

  return 42;  // generic Substrate network prefix
}

// use seg=1 for BIP purpose, seg=2 for coin type, etc.
uint32_t extractHdPathSegment(const char* hdPath, uint8_t seg) {
  std::string path = std::string(hdPath);
  size_t start = 0;

  for (uint8_t i = 0; i < seg; i++) {
    uint8_t offset = i == 0 ? 0 : 1;

    start = path.find('/', start + offset);
    if (start == std::string::npos) {
      log_e("Invalid HD path: %s", hdPath);
      return 0;
    }
  }
  start++;

  size_t end = path.find('/', start + 1);

  std::string segmentStr = path.substr(start, end - start);
  uint32_t segment = strtoul(segmentStr.c_str(), nullptr, 10);

  log_v(
      "extracted HD path segment #%d: %s -> %d / start: %d, end: %d", seg, segmentStr.c_str(),
      segment, start, end
  );

  path.clear();
  segmentStr.clear();

  return segment;
}

uint32_t getSlip44(const char* hdPath) { return extractHdPathSegment(hdPath, 2); }

uint32_t getXpubMagicNumber(const char* hdPath) {
  uint32_t bipPurpose = extractHdPathSegment(hdPath, 1);
  uint32_t slip44 = getSlip44(hdPath);

  // Ethereum uses the same magic numbers as BTC, this falls back to BTC:
  const BitcoinNetwork* network = getBitcoinNetwork(slip44);

  uint32_t magicNum = network->xpubMagic;
  if (bipPurpose == 84) {
    magicNum = network->xpubMagicSegwit;
  } else if (bipPurpose == 49) {
    magicNum = network->xpubMagicP2SH;
  }

  log_d("xpub magic number: %d / 0x%08x", magicNum, magicNum);

  return magicNum;
}
