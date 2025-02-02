// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <ArduinoJson.h>

#include <functional>
#include <map>

#include "../../config.h"
#include "../ui/ui.h"
#include "Wallet.h"

extern Wallet wallet;
extern bool isHot;

void listMethods(const JsonDocument& request, JsonDocument& response);

// Structure to hold method information
struct RpcMethod {
  std::function<void(const JsonDocument&, JsonDocument&)> handle;
  const char* paramsDescription;  // Simplified parameter description
  const char* resultDescription;
  Permission allowed = Permission::AfterKeysSetupAndUnlock;
};

// JSON RPC Handler
class JsonRpcHandler {
 public:
  JsonRpcHandler() {};
  void handleRequest(const std::string& input, std::string& output);

 private:
  bool validateRequest(const JsonDocument& request, JsonDocument& response);
};
