// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <string>

enum ChainType : uint16_t {
  UNKNOWN = 0,
  BTC = 1,
  ETH = 2,
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
};

struct WalletResponse {
  Status status;
  const char* error;
  std::string result;

  WalletResponse(Status errorCode, const char* errorMsg) : status(errorCode), error(errorMsg) {}
  WalletResponse(Status okCode, std::string result) : status(okCode), result(result) {}
  WalletResponse(std::string result) : status(Ok), result(result) {}
  WalletResponse() : status(Ok) {}
};

struct TxResponse {
  Status status;
  const char* error;
  std::string signature;
  std::string r;
  std::string s;
  std::string v;

  TxResponse(Status errorCode, const char* errorMsg) : status(errorCode), error(errorMsg) {}
  TxResponse(std::string _signature, std::string _r, std::string _s, std::string _v)
      : status(Ok), signature(_signature), r(r), s(s), v(v) {}
};

struct EthereumTxInput {
  bool hasTo;
  uint8_t* value;
  size_t valueLength;
  uint8_t* data;
  size_t dataLength;
  uint64_t chainId;
  uint8_t* pubkeyHash;
  uint8_t* nonce;
  size_t nonceLength;
  uint8_t* gasLimit;
  size_t gasLimitLength;
  uint8_t* maxFeePerGas;
  size_t maxFeePerGasLength;
  uint8_t* maxPriorityFeePerGas;
  size_t maxPriorityFeePerGasLength;
  bool isEIP1559;
};

/*
 * START ported and/or adapted code from Trezor firmware (originally licensed under GPL 3.0, see
 * `misc/GPL-3.0.txt`):
 * https://github.com/trezor/trezor-firmware/blob/29e03bd873977a498dbce79616bfb3fe4b7a0698/legacy/firmware/coins.h
 */
struct BitcoinNetwork {
  const char* name;
  const char* messageHeader;
  const char* bech32Prefix;

  bool hasSegwit;
  bool hasTaproot;
  uint64_t maxfeeKb;

  // address types > 0xFF represent a two-byte prefix in big-endian order
  uint32_t addressType;
  uint32_t addressTypeP2SH;
  uint32_t addressTypeSegwit = 6;

  uint32_t xpubMagic;
  uint32_t xpubMagicP2SH;
  uint32_t xpubMagicSegwit;

  uint32_t slip44;
  // uint32_t coinType;  // TODO: remove if unused
};
/*
 * END ported code from Trezor firmware
 */
