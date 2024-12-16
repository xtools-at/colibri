// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <string>

enum ChainType { UNKNOWN = 0, ETH = 1, BTC = 2 };

enum Permission {
  Always = 0,
  AfterSetupPassword = 1,
  AfterUnlock = 2,
  AfterUnlockAndKeys = 3,
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

struct WalletResponse {
  Status status;
  const char* error;
  std::string result;

  WalletResponse(Status errorCode, const char* errorMsg) : status(errorCode), error(errorMsg) {}
  WalletResponse(Status code, std::string result) : status(code), result(result) {}
  WalletResponse(std::string result) : status(Ok), result(result) {}
  WalletResponse() : status(Ok) {}
};
