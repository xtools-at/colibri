// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "config.h"

const char EMPTY[] = "";

// ===== RPC ===== //
const char RPC_JSONRPC[] = "jsonrpc";
const char RPC_JSONRPC_VERSION[] = "2.0";
const char RPC_ID[] = "id";
const char RPC_METHOD[] = "method";
const char RPC_PARAMS[] = "params";
const char RPC_RESULT[] = "result";
const char RPC_ERROR[] = "error";
const char RPC_ERROR_CODE[] = "code";
const char RPC_ERROR_MESSAGE[] = "message";

// ===== RPC METHOD NAMES ===== //
const char RPC_METHOD_PING[] = "ping";
const char RPC_METHOD_LIST_METHODS[] = "listMethods";
const char RPC_METHOD_SET_PW[] = "setPassword";
const char RPC_METHOD_UNLOCK[] = "unlock";
const char RPC_METHOD_LOCK[] = "lock";
const char RPC_METHOD_WIPE[] = "wipe";
const char RPC_METHOD_GET_STATUS[] = "getStatus";
const char RPC_METHOD_GET_WALLET[] = "getSelectedWallet";
const char RPC_METHOD_GET_INFO[] = "getDeviceInfo";
const char RPC_METHOD_CREATE_MNEMONIC[] = "createMnemonic";
const char RPC_METHOD_ADD_MNEMONIC[] = "addMnemonic";
const char RPC_METHOD_SELECT_WALLET[] = "selectWallet";
const char RPC_METHOD_ETH_SIGN_MSG[] = "eth_signMessage";
const char RPC_METHOD_ETH_SIGN_TYPED_DATA_HASH[] = "eth_signTypedDataHash";

// ===== RPC PARAMS & RESULTS ===== //
const char RPC_PARAMS_PW[] = "[string: password]";
const char RPC_PARAMS_ADD_MNEMONIC[] = "[string: 12/18/24 word mnemonic phrase]";
const char RPC_PARAMS_CREATE_MNEMONIC[] = "[number: (opt.) length in words (12|18|24=default)]";
const char RPC_PARAMS_SELECT_WALLET[] =
    "[number: wallet id, string: (opt.) hd path, string: (opt.) bip32 passphrase, number: (opt.) "
    "force chain type (1=ETH|2=BTC)]";
const char RPC_PARAMS_MSG[] = "[string: plaintext message to sign]";
const char RPC_PARAMS_TYPED_DATA_HASH[] =
    "[string: hex domain separator hash, string: hex message hash]";

const char RPC_RESULT_SUCCESS[] = "bool: success";
const char RPC_RESULT_STRING[] = "string: result";
const char RPC_RESULT_SIGNATURE[] = "string: signature";
const char RPC_RESULT_LIST_METHODS[] =
    "[...object: {string: method, string: params, string: result}]";
const char RPC_RESULT_CREATE_MNEMONIC[] = "[number: wallet id, string: mnemonic phrase]";
const char RPC_RESULT_ADD_MNEMONIC[] = "number: wallet id";
const char RPC_RESULT_SELECTED_WALLET[] =
    "[number: wallet id, string: address, string: pubkey, string: hdPath, string: master xpub, "
    "string: master fingerprint]";
const char RPC_RESULT_STATUS[] = "[bool: unlocked, bool: key set, bool: password set]";
const char RPC_RESULT_INFO[] =
    "[string: name, string: firmware version, string: board type, number: display type, number: "
    "stored wallets]";
const char RPC_MSG_MNEMONIC_SCREEN[] =
    "The mnemonic phrase is being displayed on the device screen";

// ===== RPC ERROR MESSAGES ===== //
const char RPC_ERROR_METHOD_NOT_FOUND[] = "Method not found";
const char RPC_ERROR_PARSE_ERROR[] = "Parse error";
const char RPC_ERROR_INVALID_PARAMS[] = "Invalid params";
const char RPC_ERROR_INTERNAL_ERROR[] = "Internal error";
const char RPC_ERROR_UNSPECIFIED_ERROR[] = "Unspecified error";
const char RPC_ERROR_NOT_IMPLEMENTED[] = "Action not supported yet";
const char RPC_ERROR_USER_REJECTED[] = "User did not approve action";
const char RPC_ERROR_LOCKED[] = "Wallet is locked";
const char RPC_ERROR_UNLOCKED[] = "Wallet is already unlocked";
const char RPC_ERROR_BUSY[] = "A request is pending already";
const char RPC_ERROR_PW_NOT_SET[] = "Set a password first";
const char RPC_ERROR_PW_SET[] = "Password set already";
const char RPC_ERROR_PW_LENGTH[] = "Password length invalid";
const char RPC_ERROR_PW_WRONG[] = "Wrong password";
const char RPC_ERROR_MNEMONIC_NOT_SET[] = "Set or generate a mnemonic first";
const char RPC_ERROR_MNEMONIC_GENERATE[] = "Error generating mnemonic";
const char RPC_ERROR_MNEMONIC_INVALID[] = "Invalid mnemonic";
const char RPC_ERROR_MNEMONIC_STORE[] = "Keystore is full";
const char RPC_ERROR_WALLET_NOT_FOUND[] = "Wallet not found or invalid HD path";
const char RPC_ERROR_WALLET_INTERNAL[] = "Internal error setting up wallet";

// ===== Storage ===== //
const char STORAGE_NVS_PARTITION_NAME[] = NVS_PARTITION_NAME;
const char STORAGE_SYS[] = "sys";
const char STORAGE_KEYS[] = "keys";
const char STORAGE_IVS[] = "ivs";
const char STORAGE_SYS_WALLET_COUNTER[] = "count";
const char STORAGE_SYS_LOGIN_ATTEMPTS[] = "l";
const char STORAGE_SYS_CHECKSUM[] = "cks";
