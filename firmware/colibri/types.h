// SPDX-License-Identifier: AGPL-3.0-or-later
// Contains ported code from Trezor firmware licensed under GPL 3.0, see notes below.
#pragma once

#include <string>

#include "config.h"

enum ChainType : uint16_t {
  UNKNOWN = 0,
  BTC = 1,
  ETH = 2,
  SOL = 3,
  DOT = 4,
};

enum Permission {
  Always = 0,
  AfterPasswordSetup = 1,
  AfterUnlock = 2,
  AfterKeysSetupAndUnlock = 3,
};

enum Status {
  // default
  Ok = 1,
  GeneralError = 0,
  // common
  ParseError = -32700,
  InvalidRequest = -32600,
  MethodNotFound = -32601,
  InvalidParams = -32602,
  InternalError = -32603,
  // custom
  UserRejected = -32000,
  Unauthorized = -32001,
  NotImplemented = -32002,
};

enum RgbColor {
  Off = 0x000000,  // Black
  Default = 0xFFFFFF,  // White
  Connecting = 0x800080,  // Purple
  Busy = 0xADD8E6,  // Light blue
  Idle = 0x009966,  // Teal
  Hot = 0xEE3300,  // Orange
  Warning = 0xFF0000,  // Red
  Success = 0x00FF00,  // Green
};

enum Connection : uint8_t {
  NotConnected = 0,
  DebugSerialConnected = 1,
  BleConnected = 2,
  UsbConnected = 3,
};

struct WalletResponse {
  Status status;
  const char* error;
  std::string result;

  WalletResponse(Status errorCode, const char* errorMsg) : status(errorCode), error(errorMsg) {}
  WalletResponse(Status okCode, std::string result) : status(okCode), result(result) {}
  WalletResponse(std::string result) : status(Ok), result(result) {}
  WalletResponse() : status(Ok) {}
  WalletResponse(Status code) : status(code) {}
};

struct UrEthSignRequest {
  uint8_t data[SIGN_REQUEST_MAX_DATA_SIZE];
  size_t dataLen;
  uint32_t dataType;
  uint32_t chainId;
  uint32_t fingerprint;
  std::string hdPath;
  uint8_t id[16];  // optional UUID
  uint8_t address[ADDRESS_LENGTH];  // optional
};

struct EthTxData {
  uint64_t chainId;
  uint8_t to[ADDRESS_LENGTH];
  uint8_t value[32];
  size_t valueSize;
  uint8_t data[SIGN_REQUEST_MAX_DATA_SIZE];
  size_t dataSize;
  uint8_t nonce[32];
  size_t nonceSize;
  uint8_t gasLimit[32];
  size_t gasLimitSize;
  uint8_t maxGasFee[32];
  size_t maxGasFeeSize;
  bool isEip1559;
  uint8_t maxPriorityFeePerGas[32];
  size_t maxPriorityFeePerGasSize;
};

/*
 * START ported and/or adapted code from Trezor firmware (licensed under GPL 3.0, see
 * `licenses/Trezor.GPL-3.0.txt`):
 * https://github.com/trezor/trezor-firmware/blob/29e03bd873977a498dbce79616bfb3fe4b7a0698/legacy/firmware/coins.h
 */
struct BitcoinNetwork {
  const char* name;
  const char* messageHeader;
  const char* bech32Prefix;

  bool hasSegwit;
  bool hasTaproot;
  uint64_t maxfeeKb;

  uint32_t addressType;
  uint32_t addressTypeP2SH;

  uint32_t xpubMagic;
  uint32_t xpubMagicP2SH;
  uint32_t xpubMagicSegwit;

  uint32_t slip44;
};
/*
 * END ported code from Trezor firmware
 */
