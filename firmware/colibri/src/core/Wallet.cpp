// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "Wallet.h"

#include "../../constants.h"

extern void stopInterfaces();
extern void wipeInterfaces();

WalletResponse Wallet::wipeRemote(bool interfacesOnly) {
  if (isLocked()) return WalletResponse(Unauthorized, RPC_ERROR_LOCKED);

  // approve request on hardware
  if (!waitForApproval()) return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

  if (interfacesOnly) {
    wipe(false, true);
  } else {
    wipe();
  }
  return WalletResponse();
}

void Wallet::wipe(bool wipeStore, bool wipeIfaces) {
  if (wipeIfaces) {
    log_i("Wiping interfaces...");
    wipeInterfaces();
    stopInterfaces();
  }

  lock();

  if (wipeStore) {
    log_i("Wiping store...");
    store.wipe();
  }

  delay(2000);
  esp_restart();
}

WalletResponse Wallet::setPassword(std::string& password) {
  // approve request on hardware
  if (!waitForApproval()) return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

  // password is set and encrypted keys exist -> reset password
  bool isPasswordChange = false;
  if (isPasswordSet() && isKeySet()) {
    // wallet needs to be unlocked to change password
    if (isLocked()) return WalletResponse(Unauthorized, RPC_ERROR_LOCKED);

    // re-encrypt mnemonics after setting new password
    log_d("Password change detected, stored mnemonics will be re-encrypted");
    isPasswordChange = true;
  }

  // check the provided password
  if (password.length() < MIN_PASSPHRASE_LENGTH || password.length() > MAX_PASSPHRASE_LENGTH) {
    return WalletResponse(InvalidParams, RPC_ERROR_PW_LENGTH);
  }

  // generate random IV
  uint8_t iv[AES_IV_SIZE];
  generateEntropy(iv, AES_IV_SIZE);
  store.writeIv(0, iv);
  log_s("generated IV: %s", toHex(iv, AES_IV_SIZE).c_str());

  // hash password
  uint8_t newPwHash[HASH_LENGTH];
  doubleHash((const uint8_t*)password.c_str(), password.length(), newPwHash, iv);
  log_ss("password hash: %s", toHex(newPwHash, HASH_LENGTH).c_str());

  // generate device key and checksum
  uint8_t key[HASH_LENGTH];
  generateEntropy(key);
  uint8_t checksum[HASH_LENGTH];
  doubleHash(key, HASH_LENGTH, checksum, iv);
  log_ss("generated device key: %s", toHex(key, HASH_LENGTH).c_str());
  log_s("generated checksum: %s", toHex(checksum, HASH_LENGTH).c_str());
  store.putBytes(STORAGE_SYS, STORAGE_SYS_CHECKSUM, checksum, HASH_LENGTH);

  // encrypt key with password hash and IV
  uint8_t encryptedKey[HASH_LENGTH];
  aesEncrypt(key, HASH_LENGTH, encryptedKey, newPwHash, iv);
  log_s("encrypted key: %s", toHex(encryptedKey, HASH_LENGTH).c_str());
  store.writeMnemonic(0, encryptedKey, HASH_LENGTH);

  // re-encrypt stored mnemonics
  if (isPasswordChange) {
    // get number of stored mnemonics
    uint16_t numWallets = store.readWalletCounter();

    // de- and re-encrypt all stored mnemonics
    for (uint16_t i = 1; i <= numWallets; i++) {
      log_d("Re-encrypting wallet #%d with new password...", i);

      std::string mnemonic;
      decryptStoredMnemonic(i, mnemonic, pwHash);
      encryptAndStoreMnemonic(i, mnemonic, newPwHash);

      mnemonic.clear();
      updateUi();
    }
  }

  // clear memory
  memzero(key, sizeof(key));
  memzero(newPwHash, sizeof(newPwHash));
  memzero(iv, sizeof(iv));
  memzero(checksum, sizeof(checksum));
  memzero(encryptedKey, sizeof(encryptedKey));

  // unlock wallet (also zeroes out "password")
  return unlock(password, false);
}

bool Wallet::isLocked() { return isAllZero(pwHash, HASH_LENGTH) || locked; }

bool Wallet::isPasswordSet() { return store.hasMnemonic(0) && store.hasIv(0); }

bool Wallet::isKeySet() { return store.hasMnemonic(1) && store.hasIv(1); }

WalletResponse Wallet::unlock(std::string& password, bool requiresApproval) {
  log_i("Unlocking wallet...");
  if (!isPasswordSet()) return WalletResponse(InvalidRequest, RPC_ERROR_PW_NOT_SET);

  // approve request on hardware
  if (requiresApproval && !waitForApproval()) {
    return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);
  }

// self-destruct after n failed attempts
#if SELF_DESTRUCT_ENABLED
  uint16_t logins = store.readLoginAttempts();
  logins++;
  store.writeLoginAttempts(logins);
  log_i("Login attempt %d/%d", logins, SELF_DESTRUCT_MAX_FAILED_ATTEMPTS);
#endif

  // read encrypted key and IV
  uint8_t encrDeviceKey[HASH_LENGTH];
  store.readMnemonic(0, encrDeviceKey);
  uint8_t iv[AES_IV_SIZE];
  store.readIv(0, iv);
  log_s("read encr. key: %s", toHex(encrDeviceKey, HASH_LENGTH).c_str());
  log_s("read IV: %s", toHex(iv, AES_IV_SIZE).c_str());

  // hash password
  doubleHash((uint8_t*)password.c_str(), password.length(), pwHash, iv);
  password.clear();
  log_ss("password hash: %s", toHex(pwHash, HASH_LENGTH).c_str());

  // decrypt device key and hash
  uint8_t deviceKey[HASH_LENGTH];
  uint8_t checksum[HASH_LENGTH];
  aesDecrypt(encrDeviceKey, HASH_LENGTH, deviceKey, pwHash, iv);
  log_ss("decrypted key: %s", toHex(deviceKey, HASH_LENGTH).c_str());
  store.readIv(0, iv);
  doubleHash(deviceKey, HASH_LENGTH, checksum, iv);
  log_s("checksum of decrypted key: %s", toHex(checksum, HASH_LENGTH).c_str());

  memzero(deviceKey, sizeof(deviceKey));
  memzero(encrDeviceKey, sizeof(encrDeviceKey));
  memzero(iv, sizeof(iv));

  // compare with stored checksum
  uint8_t storedChecksum[HASH_LENGTH];
  store.getBytes(STORAGE_SYS, STORAGE_SYS_CHECKSUM, storedChecksum, HASH_LENGTH);
  log_s("stored checksum: %s", toHex(storedChecksum, HASH_LENGTH).c_str());
  bool success = memcmp(checksum, storedChecksum, HASH_LENGTH) == 0;
  log_i("password match: %s", success ? "true" : "false");
  memzero(checksum, sizeof(checksum));
  memzero(storedChecksum, sizeof(storedChecksum));

  // wrong password
  if (!success) {
    lock();

#if SELF_DESTRUCT_ENABLED
    // self-destruct after n failed attempts
    if (logins >= SELF_DESTRUCT_MAX_FAILED_ATTEMPTS) {
      log_e("Triggered self-destruct after %d failed login attempts", logins);
      wipe();
    }
#endif

    return WalletResponse(InvalidParams, RPC_ERROR_PW_WRONG);
  }

  // successful unlock
#if SELF_DESTRUCT_ENABLED
  // reset login attempts
  store.writeLoginAttempts(0);
#endif

  // unlock wallet
  locked = false;

  // select default wallet
  if (isKeySet()) {
    return selectWallet(1);
  }

  return WalletResponse();
}

void Wallet::lock() {
  deleteHdNode();
  memzero(pwHash, sizeof(pwHash));
  memzero(&storedMnemonics, sizeof(storedMnemonics));
  memzero(&walletId, sizeof(walletId));

  hdPath.clear();
  xPub.clear();
  fingerprint.clear();
  memzero(&bipPurpose, sizeof(bipPurpose));
  memzero(&slip44, sizeof(slip44));
  memzero(&accountId, sizeof(accountId));
  memzero(&chainType, sizeof(chainType));

  timeLastActivity = 0;

  locked = true;
}

WalletResponse Wallet::selectWallet(
    uint16_t id, const char* inHdPath, const char* bip32Passphrase, ChainType inChainType
) {
  if (!isPasswordSet()) return WalletResponse(InvalidRequest, RPC_ERROR_PW_NOT_SET);
  if (isLocked()) return WalletResponse(Unauthorized, RPC_ERROR_LOCKED);
  log_i("Selecting wallet #%d", id);

  uint16_t numWallets = store.readWalletCounter();

  if (id > numWallets || !store.hasMnemonic(id) || !store.hasIv(id))
    return WalletResponse(InvalidParams, RPC_ERROR_WALLET_NOT_FOUND);

  // sanitize input
  if (!inHdPath || strlen(inHdPath) == 0) {
    inHdPath = DEFAULT_HD_PATH;
  }
  if (!bip32Passphrase) {
    bip32Passphrase = "";
  }

  // restore on error
  uint16_t currentId = walletId > 1 ? walletId : 1;
  const char* currentHdPath = hdPath.length() > 0 ? hdPath.c_str() : DEFAULT_HD_PATH;

  // decrypt mnemonic with password
  std::string mnemonic;
  decryptStoredMnemonic(id, mnemonic, pwHash);

  // create seed from mnemonic and (optional) passphrase
  uint8_t seed[BIP39_SEED_SIZE];
  mnemonic_to_seed(mnemonic.c_str(), bip32Passphrase, seed, NULL);
  log_ss("seed: %s", toHex(seed, BIP39_SEED_SIZE).c_str());

  deleteHdNode();

  // determine curve
  if (inChainType) {
    chainType = inChainType;
  } else {
    chainType = getChainType(getSlip44(inHdPath));
  }
  const char* curveType = getChainCurveType(chainType);

  // create hd node from seed
  int status = hdnode_from_seed(seed, BIP39_SEED_SIZE, curveType, &hdNode);
  log_d("hdnode creation: %s", status == 1 ? "success" : "error");
  memzero(seed, sizeof(seed));

  // grab master xpub and fingerprint before setting path
  if (status) {
    // fill public key without a path set
    hdnode_fill_public_key(&hdNode);
    log_s("master public key: %s", toHex((&hdNode)->public_key, PUBLICKEY_LENGTH, true).c_str());

    // derive the fingerprint before setting the hd path
    uint32_t fpNum = hdnode_fingerprint(&hdNode);

    // derive the extended public key (xpub)
    char xPubChars[XPUB_LENGTH + 1];
    uint32_t magicNum = getXpubMagicNumber(inHdPath);
    hdnode_serialize_public(&hdNode, fpNum, magicNum, xPubChars, sizeof(xPubChars));
    xPub = std::string(xPubChars);
    log_s("master xpub: %s", xPub.c_str());

    // store id and count
    walletId = id;
    storedMnemonics = numWallets;

    // set hd path (also fills public key)
    status = setHdPath(inHdPath) ? Status::Ok : Status::InvalidParams;

    // override chain type if passed in
    if (inChainType) {
      log_d("overriding chain type: %d to %d", chainType, inChainType);
      chainType = inChainType;
    }

    // store fingerprint after setting chain type
    uint8_t fpBytes[4];
    uint32ToBytes(fpNum, fpBytes);
    fingerprint = toHex(fpBytes, 4, true);
    log_s("master fingerprint: %s", fingerprint.c_str());

    // logs
    log_i("used chain type: %d", chainType);
    log_i("wallet pubkey: %s", getPublicKey().c_str());
    log_i("wallet address: %s", getAddress().c_str());
  } else {
    status = Status::InternalError;
  }

  // restore on error
  if (status < Status::Ok) {
    log_e("Error setting up wallet #%d", id);
    if (currentId != id) {
      selectWallet(currentId, currentHdPath, "", inChainType);
    } else {
      // lock to prevent error
      lock();
    }
    return WalletResponse((Status)status, RPC_ERROR_WALLET_INTERNAL);
  }

  // get unlock time
  timeLastActivity = millis();

  return WalletResponse();
}

WalletResponse Wallet::addMnemonic(std::string& mnemonic, uint16_t overwriteId) {
  if (isLocked()) return WalletResponse(Unauthorized, RPC_ERROR_LOCKED);

  // get next available wallet id
  uint16_t counter = store.readWalletCounter();
  log_d("Read wallet counter: %d", counter);
  uint16_t newCounter = counter + 1;
  uint16_t walletId = newCounter;

  // overwrite existing wallet
  if (overwriteId) {
    if (overwriteId < walletId) {
      log_d("Overwriting wallet #%d", overwriteId);

      walletId = overwriteId;
      newCounter = counter;
    } else {
      log_e(
          "Error: non-existing wallet id #%d passed, using next available slot #%d instead",
          overwriteId, walletId
      );
    }
  }

  if (store.isOutOfBounds(walletId))
    return WalletResponse(InvalidRequest, RPC_ERROR_MNEMONIC_STORE);

  log_ss("input mnemonic: %s", mnemonic.c_str());
  if (!mnemonic_check(mnemonic.c_str())) {
    return WalletResponse(InvalidParams, RPC_ERROR_MNEMONIC_INVALID);
  }

  // approve request on hardware
  if (!waitForApproval()) return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

  // encrypt mnemonic with password
  encryptAndStoreMnemonic(walletId, mnemonic, pwHash);

  // update counters
  store.writeWalletCounter(newCounter);
  storedMnemonics = newCounter;

  // select wallet if it's the first one
  if (walletId == 1) {
    selectWallet(walletId);
  }

  // return walletId (compatible with Status since always >=1) and mnemonic
  return WalletResponse((Status)walletId, mnemonic);
}

WalletResponse Wallet::createMnemonic(uint8_t words, uint16_t overwriteId) {
  std::string mnemonic = generateMnemonic(words);
  if (mnemonic.length() == 0) {
    return WalletResponse(InvalidParams, RPC_ERROR_INVALID_PARAMS);
  }

  return addMnemonic(mnemonic, overwriteId);
}

void Wallet::deleteHdNode() {
  memzero(&hdNode, sizeof(HDNode));
  hdNode = HDNode();
  log_v("cleared hd node");
}

bool Wallet::setHdPath(const char* inPath) {
  if (inPath == nullptr) {
    return false;
  }
  log_i("setting hd path: %s", inPath);

  // Copy the hdPath to a modifiable string
  char path[MAX_HDPATH_LENGTH];
  strncpy(path, inPath, sizeof(path));
  path[sizeof(path) - 1] = '\0';

  // Check if the path starts with "m/"
  if (strncmp(path, "m/", 2) != 0) {
    return false;
  }

  // Tokenize the path
  uint8_t i = 0;
  char* token = strtok(path + 2, "/");

  while (token != nullptr) {
    uint32_t index = strtoul(token, nullptr, 10);

    // fetch info about chain and coin from path
    if (i == 0) {
      bipPurpose = index;
    } else if (i == 1) {
      slip44 = index;
      chainType = getChainType(slip44);
    } else if (i == 4) {
      accountId = index;
    }

    // set the path
    if (strchr(token, '\'') != nullptr) {
      index |= 0x80000000;  // Hardened index
      if (!hdnode_private_ckd_prime(&hdNode, index)) {
        return false;
      }
    } else {
      if (!hdnode_private_ckd(&hdNode, index)) {
        return false;
      }
    }
    token = strtok(nullptr, "/");

    i++;
  }

  // save path & public key
  hdnode_fill_public_key(&hdNode);
  hdPath = std::string(inPath);

  return true;
}

std::string Wallet::getPublicKey() {
  if (isLocked() || !isKeySet()) {
    return "";
  }

  return toHex((&hdNode)->public_key, PUBLICKEY_LENGTH, chainType == ETH);
}

std::string Wallet::getAddress() {
  if (isLocked() || !isKeySet()) {
    return "";
  }

  // determine address based on chain type
  if (chainType == ETH) {
    // Ethereum
    return ethGetAddress(&hdNode);
  } else if (chainType == BTC) {
    // Bitcoin
    return btcGetAddress(&hdNode, bipPurpose, slip44);
  } else if (chainType == SOL) {
    // Solana
    return solGetAddress(&hdNode);
  }
  return "";
}

WalletResponse Wallet::signDigest(std::string& hexDigest) {
  if (isLocked()) return WalletResponse(Unauthorized, RPC_ERROR_LOCKED);
  if (!waitForApproval()) return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

  std::string signature;

  // Sign hash generically
  uint8_t sigBytes[RECOVERABLE_SIGNATURE_LENGTH];
  uint8_t digest[HASH_LENGTH];
  fromHex(hexDigest.c_str(), digest, HASH_LENGTH);

  bool success = signHash(&hdNode, digest, sigBytes);
  if (success) signature = toHex(sigBytes, RECOVERABLE_SIGNATURE_LENGTH, true);

  memzero(sigBytes, sizeof(sigBytes));
  memzero(&digest, sizeof(digest));

  if (signature.empty()) {
    return WalletResponse(InternalError, RPC_ERROR_SIGNATURE_FAILED);
  }

  return WalletResponse(signature);
}

WalletResponse Wallet::signTransaction(JsonArrayConst input, ChainType chainTypeOverride) {
  if (isLocked()) return WalletResponse(Unauthorized, RPC_ERROR_LOCKED);
  if (!waitForApproval()) return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

  ChainType useChainType = chainTypeOverride ? chainTypeOverride : chainType;
  log_i("Signing tx (chain type %d)", useChainType);

  if (useChainType == ETH) {
    // Ethereum
    return ethSignTransaction(&hdNode, input);
  }

  return WalletResponse(NotImplemented, RPC_ERROR_NOT_IMPLEMENTED);
}

WalletResponse Wallet::signMessage(std::string& message, ChainType chainTypeOverride) {
  if (isLocked()) return WalletResponse(Unauthorized, RPC_ERROR_LOCKED);
  if (!waitForApproval()) return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

  std::string signature;
  ChainType useChainType = chainTypeOverride ? chainTypeOverride : chainType;
  log_i("Signing message (type %d): %s", useChainType, message.c_str());

  if (useChainType == ETH) {
    // Ethereum
    signature = ethSignMessage(&hdNode, message);
  } else if (useChainType == BTC) {
    // Bitcoin
    signature = btcSignMessage(&hdNode, message, slip44, bipPurpose);
  } else {
    return WalletResponse(NotImplemented, RPC_ERROR_NOT_IMPLEMENTED);
  }

  if (signature.empty()) {
    return WalletResponse(InternalError, RPC_ERROR_SIGNATURE_FAILED);
  }

  return WalletResponse(signature);
}

WalletResponse Wallet::signTypedDataHash(
    std::string& domainSeparatorHash, std::string& messageHash
) {
  log_i(
      "Signing typed data - domainSeparatorHash: %s; messageHash: %s", domainSeparatorHash.c_str(),
      messageHash.c_str()
  );
  std::string signature;

  if (isLocked()) return WalletResponse(Unauthorized, RPC_ERROR_LOCKED);
  if (!waitForApproval()) return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

  if (chainType == ETH) {
    signature = ethSignTypedDataHash(&hdNode, domainSeparatorHash, messageHash);
  } else {
    return WalletResponse(NotImplemented, RPC_ERROR_NOT_IMPLEMENTED);
  }

  if (signature.empty()) {
    return WalletResponse(InternalError, RPC_ERROR_SIGNATURE_FAILED);
  }

  return WalletResponse(signature);
}

void Wallet::decryptStoredMnemonic(uint16_t id, std::string& output, uint8_t encKey[HASH_LENGTH]) {
  // read encrypted mnemonic and IV
  uint8_t encrMnemonic[MAX_MNEMONIC_LENGTH];
  size_t mnemonicLen = store.readMnemonic(id, encrMnemonic);
  log_s("read encr. mnemonic bytes: %s", toHex(encrMnemonic, mnemonicLen).c_str());
  uint8_t iv[AES_IV_SIZE];
  store.readIv(id, iv);
  log_s("read IV: %s", toHex(iv, AES_IV_SIZE).c_str());

  // decrypt mnemonic
  uint8_t mnemonicBytes[mnemonicLen];
  aesDecrypt(encrMnemonic, mnemonicLen, mnemonicBytes, pwHash, iv);
  char mnemonicStr[mnemonicLen + 1];
  strncpy(mnemonicStr, (const char*)mnemonicBytes, mnemonicLen);
  mnemonicStr[mnemonicLen] = '\0';
  log_ss("decrypted mnemonic string: %s", mnemonicStr);
  output = std::string(mnemonicStr);

  // clear memory
  memzero(encrMnemonic, sizeof(encrMnemonic));
  memzero(iv, sizeof(iv));
  memzero(mnemonicBytes, sizeof(mnemonicBytes));
  memzero(mnemonicStr, sizeof(mnemonicStr));
}

void Wallet::encryptAndStoreMnemonic(
    uint16_t id, std::string& mnemonic, uint8_t encKey[HASH_LENGTH]
) {
  // create and store iv
  uint8_t iv[AES_IV_SIZE];
  generateEntropy(iv, AES_IV_SIZE);
  store.writeIv(id, iv);
  log_s("generated IV: %s", toHex(iv, AES_IV_SIZE).c_str());

  // encrypt and store mnemonic
  size_t mnemonicLen = mnemonic.length() + 1;
  uint8_t encryptedMnemonic[mnemonicLen];
  aesEncrypt((uint8_t*)mnemonic.c_str(), mnemonicLen, encryptedMnemonic, encKey, iv);
  log_s("encrypted mnemonic bytes: %s", toHex(encryptedMnemonic, mnemonicLen).c_str());
  store.writeMnemonic(id, encryptedMnemonic, mnemonicLen);

  // clear memory
  memzero(encryptedMnemonic, sizeof(encryptedMnemonic));
  memzero(iv, sizeof(iv));
}
