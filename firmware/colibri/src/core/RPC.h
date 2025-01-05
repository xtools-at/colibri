// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <ArduinoJson.h>

#include <functional>
#include <map>

#include "../../config.h"
#include "../utils/ui.h"
#include "Wallet.h"

extern Wallet wallet;
extern bool isHot;

// Structure to hold method information
struct RpcMethod {
  std::function<void(const JsonDocument&, JsonDocument&)> handle;
  const char* paramsDescription;  // Simplified parameter description
  const char* resultDescription;
  Permission allowed;
};

// JSON RPC Handler
class JsonRpcHandler {
 public:
  JsonRpcHandler();

  void init();
  void handleRequest(const std::string& input, std::string& output);

 private:
  void addMethod(
      const char* name, std::function<void(const JsonDocument&, JsonDocument&)> handle,
      const char* paramsDescription, const char* resultDescription,
      Permission allowed = Permission::AfterKeysSetupAndUnlock
  );
  void listMethods(JsonDocument& response);
  bool validateRequest(const JsonDocument& request, JsonDocument& response);

  bool initialised = false;
  // Map to store method names and their corresponding functions and
  // descriptions
  std::map<std::string, RpcMethod> methods;
};
