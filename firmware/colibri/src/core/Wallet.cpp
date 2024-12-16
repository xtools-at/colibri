#pragma once

#include "Wallet.h"

#include "../../constants.h"

extern void stopInterfaces();
extern void wipeInterfaces();
extern void initInterfaces();

WalletResponse Wallet::wipeRemote() {
  if (isLocked()) return WalletResponse(Unauthorized, RPC_ERROR_LOCKED);

  // approve request on hardware
  if (!waitForApproval()) return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

  wipe();
  return WalletResponse();
}

void Wallet::wipe() {
  log_i("Wiping device...");

  wipeInterfaces();
  stopInterfaces();

  lock();
  store.wipe();

  delay(2000);
  esp_restart();
}

WalletResponse Wallet::setPassword(std::string& password) {
  // approve request on hardware
  if (!waitForApproval()) return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

  if (isPasswordSet() && isKeySet()) {
    // if (isLocked()) return Status::Unauthorized;

    // currently the wallet needs to be wiped to change the password.
    // TODO: re-encrypt already stored keys here
    return WalletResponse(InvalidRequest, RPC_ERROR_PW_SET);
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
  uint8_t passwordHash[HASH_LENGTH];
  doubleHash((const uint8_t*)password.c_str(), password.length(), passwordHash, iv);
  log_s("password hash: %s", toHex(passwordHash, HASH_LENGTH).c_str());

  // generate device key and checksum
  uint8_t key[HASH_LENGTH];
  generateEntropy(key);
  uint8_t checksum[HASH_LENGTH];
  doubleHash(key, HASH_LENGTH, checksum, iv);
  // log_s("generated device key: %s", toHex(key, HASH_LENGTH).c_str());
  log_s("generated checksum: %s", toHex(checksum, HASH_LENGTH).c_str());

  // encrypt key with password hash and IV
  uint8_t encryptedKey[HASH_LENGTH];
  aesEncrypt(key, HASH_LENGTH, encryptedKey, passwordHash, iv);
  log_s("encrypted key: %s", toHex(encryptedKey, HASH_LENGTH).c_str());

  // store encrypted key and checksum
  store.writeMnemonic(0, encryptedKey, HASH_LENGTH);
  store.putBytes(STORAGE_SYS, STORAGE_SYS_CHECKSUM, checksum, HASH_LENGTH);

  // clear memory
  memzero(iv, sizeof(iv));
  memzero(passwordHash, sizeof(passwordHash));
  memzero(key, sizeof(key));
  memzero(checksum, sizeof(checksum));
  memzero(encryptedKey, sizeof(encryptedKey));

  // unlock wallet (also zeroes out "password")
  unlock(password, false);

  return WalletResponse();
}

bool Wallet::isLocked() { return store.isAllZero(pwHash, HASH_LENGTH) || locked; }

bool Wallet::isPasswordSet() { return store.hasMnemonic(0) && store.hasIv(0); }

bool Wallet::isKeySet() { return store.hasMnemonic(1) && store.hasIv(1); }

WalletResponse Wallet::unlock(std::string& password, bool requiresApproval) {
  log_i("Unlocking wallet...");
  if (!isPasswordSet()) return WalletResponse(InvalidRequest, RPC_ERROR_PW_NOT_SET);

  // approve request on hardware
  if (requiresApproval && !waitForApproval())
    return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

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
  // log_s("password hash: %s", toHex(pwHash, HASH_LENGTH).c_str());

  // decrypt device key and hash
  uint8_t deviceKey[HASH_LENGTH];
  uint8_t checksum[HASH_LENGTH];
  aesDecrypt(encrDeviceKey, HASH_LENGTH, deviceKey, pwHash, iv);
  // log_s("decrypted key: %s", toHex(deviceKey, HASH_LENGTH).c_str());
  store.readIv(0, iv);
  doubleHash(deviceKey, HASH_LENGTH, checksum, iv);
  log_s("decrypted checksum: %s", toHex(checksum, HASH_LENGTH).c_str());

  memzero(deviceKey, sizeof(deviceKey));
  memzero(encrDeviceKey, sizeof(encrDeviceKey));
  memzero(iv, sizeof(iv));

  // compare with stored checksum
  uint8_t storedChecksum[HASH_LENGTH];
  store.getBytes(STORAGE_SYS, STORAGE_SYS_CHECKSUM, storedChecksum, HASH_LENGTH);
  log_s("stored checksum: %s", toHex(storedChecksum, HASH_LENGTH).c_str());
  bool success = memcmp(checksum, storedChecksum, HASH_LENGTH) == 0;
  log_i("password match: %s", success == 1 ? "true" : "false");
  memzero(checksum, sizeof(checksum));
  memzero(storedChecksum, sizeof(storedChecksum));
  if (!success) {
    lock();
    return WalletResponse(InvalidParams, RPC_ERROR_PW_WRONG);
  }

  // unlock wallet
  locked = false;

  if (isKeySet()) {
    // select default wallet
    selectWallet(1);
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
  memzero(&fingerprint, sizeof(fingerprint));
  memzero(&bipId, sizeof(bipId));
  memzero(&slip44, sizeof(slip44));
  memzero(&chainType, sizeof(chainType));

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

  // restore
  uint16_t currentId = walletId || 1;
  const char* currentHdPath = hdPath.length() > 0 ? hdPath.c_str() : DEFAULT_HD_PATH;

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
  // log_s("decrypted mnemonic string: %s", mnemonicStr);

  // create seed from mnemonic and (optional) passphrase
  uint8_t seed[BIP39_SEED_SIZE];
  mnemonic_to_seed(mnemonicStr, bip32Passphrase, seed, NULL);
  // log_s("seed: %s", toHex(seed, BIP39_SEED_SIZE).c_str());

  // create hd node from seed
  deleteHdNode();
  // SECP256K1 is hardcoded here, works for BTC & ETH
  const char* curveType = SECP256K1_NAME;
  int status = hdnode_from_seed(seed, BIP39_SEED_SIZE, curveType, &hdNode);
  log_d("hdnode creation: %s", status == 1 ? "success" : "error");

  // grab master xpub and fingerprint before setting path
  if (status) {
    // fill public key without a path set
    hdnode_fill_public_key(&hdNode);
    log_i("master public key: %s", toHex((&hdNode)->public_key, PUBLICKEY_LENGTH, true).c_str());

    // derive the extended public key (xpub)
    uint32_t version = 0x0488B21E;  // version for mainnet xpub // TODO: check
                                    // if this is sufficient
    char xPubArray[XPUB_LENGTH + 1];
    hdnode_serialize_public(&hdNode, 0, version, xPubArray, sizeof(xPubArray));
    xPub = std::string(xPubArray);
    log_i("master xpub: %s", xPub.c_str());

    // derive the fingerprint
    fingerprint = hdnode_fingerprint(&hdNode);
    log_i("master fingerprint: %08x", fingerprint);

    // store id and count
    walletId = id;
    storedMnemonics = numWallets;

    // set hd path (also fills public key)
    status = setHdPath(inHdPath) ? Status::Ok : Status::InvalidParams;

    // store chain type if passed in
    if (inChainType) {
      log_d("overriding chain type: %d to %d", chainType, inChainType);
      chainType = inChainType;
    }

    log_i("used chain type: %s", chainType == ETH ? "ETH" : "BTC");
    log_i("wallet pubkey: %s", getPublicKey().c_str());
    log_i("wallet address: %s", getAddress().c_str());
  } else {
    status = Status::InternalError;
  }

  // clear memory
  memzero(mnemonicBytes, sizeof(mnemonicBytes));
  memzero(mnemonicStr, sizeof(mnemonicStr));
  memzero(seed, sizeof(seed));
  memzero(encrMnemonic, sizeof(encrMnemonic));
  memzero(iv, sizeof(iv));

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

  return WalletResponse(getAddress());
}

WalletResponse Wallet::addMnemonic(std::string& mnemonic) {
  if (isLocked()) return WalletResponse(Unauthorized, RPC_ERROR_LOCKED);
  // check input
  uint16_t counter = store.readWalletCounter();
  log_d("Read wallet counter: %d", counter);

  uint16_t walletId = counter + 1;
  if (store.isOutOfBounds(walletId))
    return WalletResponse(InvalidRequest, RPC_ERROR_MNEMONIC_STORE);

  // log_s("input mnemonic: %s", mnemonic.c_str());
  if (!mnemonic_check(mnemonic.c_str())) {
    return WalletResponse(InvalidParams, RPC_ERROR_MNEMONIC_INVALID);
  }

  // approve request on hardware
  if (!waitForApproval()) return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

  // create and store iv
  uint8_t iv[AES_IV_SIZE];
  generateEntropy(iv, AES_IV_SIZE);
  store.writeIv(walletId, iv);
  log_s("generated IV: %s", toHex(iv, AES_IV_SIZE).c_str());

  // encrypt and store mnemonic
  size_t mnemonicLen = mnemonic.length() + 1;
  uint8_t encryptedMnemonic[mnemonicLen];
  aesEncrypt((uint8_t*)mnemonic.c_str(), mnemonicLen, encryptedMnemonic, pwHash, iv);
  log_s("encrypted mnemonic bytes: %s", toHex(encryptedMnemonic, mnemonicLen).c_str());
  store.writeMnemonic(walletId, encryptedMnemonic, mnemonicLen);

  // update counters
  store.writeWalletCounter(walletId);
  storedMnemonics = walletId;

  // clear memory
  memzero(encryptedMnemonic, sizeof(encryptedMnemonic));
  memzero(iv, sizeof(iv));

  // select wallet if it's the first one
  if (walletId == 1) {
    selectWallet(walletId);
  }

  // return mnemonic
  return WalletResponse((Status)walletId, mnemonic);
}

WalletResponse Wallet::createMnemonic(uint8_t words) {
  std::string mnemonic = generateMnemonic(words);
  if (mnemonic.length() == 0) {
    return WalletResponse(InvalidParams, RPC_ERROR_INVALID_PARAMS);
  }

  return addMnemonic(mnemonic);
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
      bipId = index;
    } else if (i == 1) {
      slip44 = index;
      chainType = getChainType(slip44);
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

  bool use0xPrefix = chainType == ETH;

  // TODO: do not hexlify other chains
  return toHex((&hdNode)->public_key, PUBLICKEY_LENGTH, use0xPrefix);
}

std::string Wallet::getAddress() {
  if (isLocked() || !isKeySet()) {
    return "";
  }

  // Ethereum
  if (chainType == ETH) {
    return ethereum.getAddress(&hdNode);
  } else {
    /*
    // Bitcoin
  char address[MAX_ADDRESS_LENGTH];
    if (bipId == 86) {
      // Taproot address (P2TR/BIP86)
      uint8_t tweakedPubkey[HASH_LENGTH];
      zkp_bip340_tweak_public_key(hdNode->public_key + 1, NULL, tweakedPubkey);
      if (!segwit_addr_encode(address, "bc", 1, tweakedPubkey, HASH_LENGTH)) {
        return "";
      }
    } else if (bipId == 84) {
      // Segwit P2WPKH address
      uint8_t rawAddress[HASH_LENGTH];
      hdnode_get_address_raw(hdNode, ADDRESS_TYPE_SEGWIT, rawAddress);

      if (!segwit_addr_encode(address, "bc", 0, rawAddress, ADDRESS_LENGTH)) {
        return "";
      }
    } else if (bipId == 49) {
      // BIP49 P2WPKH-nested-in-P2SH address
      ecdsa_get_address_segwit_p2sh(
          hdNode->public_key, ADDRESS_TYPE_P2SH, hdNode->curve->hasher_pubkey,
          hdNode->curve->hasher_base58, address, MAX_ADDRESS_LENGTH
      );
    } else {
      // legacy P2PKH address
      ecdsa_get_address(
          hdNode->public_key, ADDRESS_TYPE_LEGACY, hdNode->curve->hasher_pubkey,
          hdNode->curve->hasher_base58, address, MAX_ADDRESS_LENGTH
      );
    }
    return std::string(address);
    */
  }
  return "";
}

WalletResponse Wallet::signMessage(std::string& message) {
  log_i("Signing message: %s", message.c_str());
  std::string signature;

  if (isLocked()) return WalletResponse(Unauthorized, RPC_ERROR_LOCKED);
  if (!waitForApproval()) return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

  if (chainType == ETH) {
    signature = ethereum.signMessage(&hdNode, message);
    return !signature.empty() ? WalletResponse(signature)
                              : WalletResponse(InternalError, RPC_ERROR_INTERNAL_ERROR);
  }

  return WalletResponse(NotImplemented, RPC_ERROR_NOT_IMPLEMENTED);
}

WalletResponse Wallet::signTypedData(std::string& domainSeparatorHash, std::string& messageHash) {
  log_i(
      "Signing typed data - domainSeparatorHash: %s; messageHash: %s", domainSeparatorHash.c_str(),
      messageHash.c_str()
  );
  std::string signature;

  if (isLocked()) return WalletResponse(Unauthorized, RPC_ERROR_LOCKED);
  if (!waitForApproval()) return WalletResponse(UserRejected, RPC_ERROR_USER_REJECTED);

  if (chainType == ETH) {
    signature = ethereum.signTypedData(&hdNode, domainSeparatorHash, messageHash);
    return !signature.empty() ? WalletResponse(signature)
                              : WalletResponse(InternalError, RPC_ERROR_INTERNAL_ERROR);
  }

  return WalletResponse(NotImplemented, RPC_ERROR_NOT_IMPLEMENTED);
}
