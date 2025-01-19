// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "RPC.h"

#include "../../constants.h"

static void rpcError(JsonDocument& response, const char* errorMsg, int errorCode) {
  log_v("rpcError called: %s\n", errorMsg);

  JsonObject jsonError = response[RPC_ERROR].to<JsonObject>();
  jsonError[RPC_ERROR_CODE] = errorCode;
  jsonError[RPC_ERROR_MESSAGE] = errorMsg;
}

void ping(const JsonDocument& request, JsonDocument& response) { response[RPC_RESULT] = "pong"; }

void setPassword(const JsonDocument& request, JsonDocument& response) {
  std::string password = request[RPC_PARAMS][0];

  // process and store the password
  WalletResponse r = wallet.setPassword(password);

  if (r.status < Ok) {
    rpcError(response, r.error, r.status);
  } else {
    response[RPC_RESULT] = true;
  }

  password.clear();
}

void unlock(const JsonDocument& request, JsonDocument& response) {
  if (!wallet.isLocked()) {
    return rpcError(response, RPC_ERROR_UNLOCKED, Status::InvalidRequest);
  }

  std::string password = request[RPC_PARAMS][0];

  // process and store the password
  WalletResponse r = wallet.unlock(password);
  if (r.status < Ok) {
    rpcError(response, r.error, r.status);
  } else {
    response[RPC_RESULT] = true;
  }

  password.clear();
}

void lock(const JsonDocument& request, JsonDocument& response) {
  wallet.lock();
  response[RPC_RESULT] = true;
}

void getStatus(const JsonDocument& request, JsonDocument& response) {
  JsonArray resultsArray = response[RPC_RESULT].to<JsonArray>();

  resultsArray.add(!wallet.isLocked());
  resultsArray.add(wallet.isKeySet());
  resultsArray.add(wallet.isPasswordSet());
}

void getDeviceInfo(const JsonDocument& request, JsonDocument& response) {
  JsonArray resultsArray = response[RPC_RESULT].to<JsonArray>();

  resultsArray.add(HW_DEVICE_NAME);
  resultsArray.add(HW_FIRMWARE_VERSION);
  resultsArray.add(CONFIG_IDF_TARGET);
  resultsArray.add(DISPLAY_TYPE);
  resultsArray.add(wallet.storedMnemonics);
}

void getSelectedWallet(const JsonDocument& request, JsonDocument& response) {
  JsonArray resultsArray = response[RPC_RESULT].to<JsonArray>();

  // return wallet id, current chainType, address, pubkey, hdPath
  resultsArray.add(wallet.walletId);
  resultsArray.add(wallet.chainType);
  resultsArray.add(wallet.getAddress());
  resultsArray.add(wallet.getPublicKey());
  resultsArray.add(wallet.hdPath);
}

void exportXpub(const JsonDocument& request, JsonDocument& response) {
  if (!waitForApproval()) {
    return rpcError(response, RPC_ERROR_USER_REJECTED, Status::UserRejected);
  }

  JsonArray resultsArray = response[RPC_RESULT].to<JsonArray>();

  // return xpub, fingerprint
  resultsArray.add(wallet.xPub);
  resultsArray.add(wallet.fingerprint);
}

void wipe(const JsonDocument& request, JsonDocument& response) {
  WalletResponse r = wallet.wipeRemote();

  if (r.status < Ok) {
    rpcError(response, r.error, r.status);
  } else {
    response[RPC_RESULT] = true;
  }
}

void deletePairedDevices(const JsonDocument& request, JsonDocument& response) {
  WalletResponse r = wallet.wipeRemote(true);

  if (r.status < Ok) {
    rpcError(response, r.error, r.status);
  } else {
    response[RPC_RESULT] = true;
  }
}

void createMnemonic(const JsonDocument& request, JsonDocument& response) {
  uint8_t words = request[RPC_PARAMS][0];
  uint16_t id = request[RPC_PARAMS][1];

  if (!words) words = DEFAULT_MNEMONIC_WORDS;

  WalletResponse r = wallet.createMnemonic(words, id);

  if (r.status < Ok) {
    rpcError(response, r.error, r.status);
  } else {
    JsonArray resultsArray = response[RPC_RESULT].to<JsonArray>();
    // return walletId, and mnemonic on devices without display
    resultsArray.add(r.status);
#ifdef DISPLAY_ENABLED
    resultsArray.add(RPC_MSG_MNEMONIC_SCREEN);
#else
    resultsArray.add(r.result);
#endif
  }

  memzero(&r, sizeof(r));
}

void addMnemonic(const JsonDocument& request, JsonDocument& response) {
  std::string mnemonic = request[RPC_PARAMS][0];
  uint16_t id = request[RPC_PARAMS][1];

  WalletResponse r = wallet.addMnemonic(mnemonic, id);

  if (r.status < Ok) {
    rpcError(response, r.error, r.status);
  } else {
    response[RPC_RESULT] = r.status;
  }

  mnemonic.clear();
  memzero(&r, sizeof(r));
}

void selectWallet(const JsonDocument& request, JsonDocument& response) {
  uint16_t id = request[RPC_PARAMS][0];
  const char* hdPath = request[RPC_PARAMS][1];
  const char* bip32Passphrase = request[RPC_PARAMS][2];
  uint16_t chainType = request[RPC_PARAMS][3];

  // check input
  if (id == NULL || id < 1 || (hdPath != nullptr && strlen(hdPath) == 0) ||
      (bip32Passphrase != nullptr && strlen(bip32Passphrase) == 0)) {
    return rpcError(response, RPC_ERROR_INVALID_PARAMS, Status::InvalidParams);
  }

  WalletResponse r = wallet.selectWallet(id, hdPath, bip32Passphrase, (ChainType)chainType);
  if (r.status < Ok) {
    rpcError(response, r.error, r.status);
  } else {
    getSelectedWallet(request, response);
  }
}

void signDigest(const JsonDocument& request, JsonDocument& response) {
  std::string hash = request[RPC_PARAMS][0];

  WalletResponse r = wallet.signDigest(hash);
  if (r.status < Ok) {
    rpcError(response, r.error, r.status);
  } else {
    response[RPC_RESULT] = r.result;
  }

  hash.clear();
  memzero(&r, sizeof(r));
}

void signMessage(const JsonDocument& request, JsonDocument& response) {
  std::string msg = request[RPC_PARAMS][0];
  uint16_t chainType = request[RPC_PARAMS][1];

  WalletResponse r = wallet.signMessage(msg, (ChainType)chainType);
  if (r.status < Ok) {
    rpcError(response, r.error, r.status);
  } else {
    response[RPC_RESULT] = r.result;
  }

  msg.clear();
  memzero(&r, sizeof(r));
}

void signTypedDataHash(const JsonDocument& request, JsonDocument& response) {
  std::string domainSeparatorHash = request[RPC_PARAMS][0];
  std::string msgHash = request[RPC_PARAMS][1];

  WalletResponse r = wallet.signTypedDataHash(domainSeparatorHash, msgHash);
  if (r.status < Ok) {
    rpcError(response, r.error, r.status);
  } else {
    response[RPC_RESULT] = r.result;
  }

  domainSeparatorHash.clear();
  msgHash.clear();
  memzero(&r, sizeof(r));
}

void signEthereumTransaction(const JsonDocument& request, JsonDocument& response) {
  JsonArrayConst params = request[RPC_PARAMS];

  WalletResponse r = wallet.signTransaction(params);
  if (r.status < Ok) {
    rpcError(response, r.error, r.status);
  } else {
    response[RPC_RESULT] = r.result;
  }

  memzero(&r, sizeof(r));
}

JsonRpcHandler::JsonRpcHandler() : initialised(false) {}

void JsonRpcHandler::init() {
  if (initialised) {
    return;
  }

  // permissive methods
  addMethod(RPC_METHOD_PING, ping, EMPTY, RPC_RESULT_STRING, Permission::Always);
  addMethod(
      RPC_METHOD_LIST_METHODS,
      [this](const JsonDocument& request, JsonDocument& response) { this->listMethods(response); },
      EMPTY, RPC_RESULT_LIST_METHODS, Permission::Always
  );
  addMethod(RPC_METHOD_SET_PW, setPassword, RPC_PARAMS_PW, RPC_RESULT_SUCCESS, Permission::Always);
  addMethod(RPC_METHOD_GET_STATUS, getStatus, EMPTY, RPC_RESULT_STATUS, Permission::Always);

  // after password is set
  addMethod(
      RPC_METHOD_UNLOCK, unlock, RPC_PARAMS_PW, RPC_RESULT_SUCCESS, Permission::AfterPasswordSetup
  );
  addMethod(RPC_METHOD_LOCK, lock, EMPTY, RPC_RESULT_SUCCESS, Permission::AfterPasswordSetup);

  // after unlock
  addMethod(RPC_METHOD_WIPE, wipe, EMPTY, RPC_RESULT_SUCCESS, Permission::AfterUnlock);
  addMethod(
      RPC_METHOD_DELETE_PAIRED_DEVICES, deletePairedDevices, EMPTY, RPC_RESULT_SUCCESS,
      Permission::AfterUnlock
  );
  addMethod(
      RPC_METHOD_CREATE_MNEMONIC, createMnemonic, RPC_PARAMS_CREATE_MNEMONIC,
      RPC_RESULT_CREATE_MNEMONIC, Permission::AfterUnlock
  );
  addMethod(
      RPC_METHOD_ADD_MNEMONIC, addMnemonic, RPC_PARAMS_ADD_MNEMONIC, RPC_RESULT_ADD_MNEMONIC,
      Permission::AfterUnlock
  );
  addMethod(RPC_METHOD_GET_INFO, getDeviceInfo, EMPTY, RPC_RESULT_INFO, Permission::AfterUnlock);

  // after unlock + keys set
  addMethod(RPC_METHOD_GET_WALLET, getSelectedWallet, EMPTY, RPC_RESULT_SELECTED_WALLET);
  addMethod(RPC_METHOD_EXPORT_XPUB, exportXpub, EMPTY, RPC_RESULT_EXPORT_XPUB);
  addMethod(
      RPC_METHOD_SELECT_WALLET, selectWallet, RPC_PARAMS_SELECT_WALLET, RPC_RESULT_SELECTED_WALLET
  );

  // - signing
  addMethod(RPC_METHOD_SIGN_HASH, signDigest, RPC_PARAMS_DIGEST, RPC_RESULT_SIGNATURE);
  addMethod(RPC_METHOD_SIGN_MSG, signMessage, RPC_PARAMS_MSG, RPC_RESULT_SIGNATURE);
  addMethod(
      RPC_METHOD_ETH_SIGN_TYPED_DATA_HASH, signTypedDataHash, RPC_PARAMS_TYPED_DATA_HASH,
      RPC_RESULT_SIGNATURE
  );
  addMethod(
      RPC_METHOD_ETH_SIGN_TX, signEthereumTransaction, RPC_PARAMS_ETH_SIGN_TX, RPC_RESULT_SIGNATURE
  );

  initialised = true;
}

// Add or update a method with its handling function and parameter description
void JsonRpcHandler::addMethod(
    const char* name, std::function<void(const JsonDocument&, JsonDocument&)> handle,
    const char* paramsDescription, const char* resultDescription, Permission allowed
) {
  methods[name] = {handle, paramsDescription, resultDescription, allowed};
}

// adds a list of all available methods to response->result
void JsonRpcHandler::listMethods(JsonDocument& response) {
  JsonArray resultsArray = response[RPC_RESULT].to<JsonArray>();

  for (const auto& method : methods) {
    JsonObject methodObj = resultsArray.createNestedObject();
    methodObj[RPC_METHOD] = method.first;
    methodObj[RPC_PARAMS] = method.second.paramsDescription;
    methodObj[RPC_RESULT] = method.second.resultDescription;
  }
};

bool JsonRpcHandler::validateRequest(const JsonDocument& request, JsonDocument& response) {
  std::string methodName = request[RPC_METHOD];
  log_d("validating method call for %s", methodName.c_str());

  // method not found
  if (methods.find(methodName) == methods.end()) {
    log_e("Error: method not found: %s", methodName.c_str());
    rpcError(response, RPC_ERROR_METHOD_NOT_FOUND, Status::MethodNotFound);
    return false;
  }

  log_d(
      "method config: Permission: %d; Params: %s; Result: %s", methods[methodName].allowed,
      methods[methodName].paramsDescription, methods[methodName].resultDescription
  );

  // check if password is set
  if (methods[methodName].allowed >= Permission::AfterPasswordSetup && !wallet.isPasswordSet()) {
    log_e("Error: password not set");
    rpcError(response, RPC_ERROR_PW_NOT_SET, Status::InvalidRequest);
    return false;
  }

  // check if wallet is unlocked
  if (wallet.isLocked() && methods[methodName].allowed >= Permission::AfterUnlock) {
    log_e("Error: wallet locked");
    rpcError(response, RPC_ERROR_LOCKED, Status::Unauthorized);
    return false;
  }

  // check if keys are set
  if ((wallet.isLocked() || !wallet.isKeySet()) &&
      methods[methodName].allowed >= Permission::AfterKeysSetupAndUnlock) {
    log_e("Error: keys not set");
    rpcError(response, RPC_ERROR_MNEMONIC_NOT_SET, Status::InvalidRequest);
    return false;
  }

  // Check if the request contains parameters.
  // Methods with only one optional parameter are not supported, hardcode affected ones here.
  bool methodRequiresParams = strlen(methods[methodName].paramsDescription) != 0 &&
      methodName.compare(RPC_METHOD_LIST_METHODS) != 0 &&
      methodName.compare(RPC_METHOD_CREATE_MNEMONIC) != 0;

  if (methodRequiresParams &&
      (request[RPC_PARAMS].isNull() || request[RPC_PARAMS].size() == 0 ||
       request[RPC_PARAMS][0].isNull())) {
    log_e("Error: bad params");
    rpcError(response, RPC_ERROR_INVALID_PARAMS, Status::InvalidParams);
    return false;
  }

  log_v("validation ok\n");
  return true;
}

// Handle an incoming RPC request
void JsonRpcHandler::handleRequest(const std::string& input, std::string& output) {
  setStateBusy(true);

  JsonDocument request;
  JsonDocument response;

  response[RPC_JSONRPC] = RPC_JSONRPC_VERSION;

  // parse request JSON
  log_d("parsing input: %s", input.c_str());

  auto parseError = deserializeJson(request, input);
  if (parseError) {
    log_e("parse error: %s", parseError.c_str());
    rpcError(response, RPC_ERROR_PARSE_ERROR, Status::ParseError);
    response[RPC_ID] = 0;
    serializeJson(response, output);
    return;
  }

  response[RPC_ID] = request[RPC_ID] ? request[RPC_ID] : 0;

  if (isHot) {
    // hot wallet, only allow ping
    if (request[RPC_METHOD] != RPC_METHOD_PING) {
      log_e("Error: request sent while device is busy");
      rpcError(response, RPC_ERROR_BUSY, Status::InvalidRequest);
      serializeJson(response, output);
      return;
    }
  }

  // validate payload
  bool valid = validateRequest(request, response);
  if (valid) {
    // call the RPC method if payload passed validation
    methods[request[RPC_METHOD]].handle(request, response);
  }

  // serialize response
  serializeJson(response, output);

  log_d("output: %s\n", output.c_str());

  request.clear();
  response.clear();

  setStateBusy(false);
}
