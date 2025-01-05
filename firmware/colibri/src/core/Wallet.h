// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"
#include "../signers/Bitcoin.h"
#include "../signers/Ethereum.h"
#include "../utils/chains.h"
#include "../utils/crypto.h"
#include "../utils/ui.h"
#include "Storage.h"
#include "esp_system.h"

extern Storage store;

class Wallet {
 public:
  Wallet() : locked(true) {}

  std::string hdPath;
  std::string xPub;
  std::string fingerprint;
  uint16_t walletId;
  uint16_t storedMnemonics;
  ChainType chainType;
  uint32_t bipPurpose;
  uint32_t slip44;
  uint32_t accountId;

  bool isLocked();
  bool isPasswordSet();
  bool isKeySet();

  void wipe(bool wipeStore = true, bool wipeIfaces = true);
  void lock();

  WalletResponse setPassword(std::string& password);
  WalletResponse addMnemonic(std::string& mnemonic, uint16_t overwriteId = 0);
  WalletResponse createMnemonic(uint8_t words = DEFAULT_MNEMONIC_WORDS, uint16_t overwriteId = 0);
  WalletResponse unlock(std::string& password, bool requiresApproval = true);
  WalletResponse wipeRemote(bool interfacesOnly = false);
  WalletResponse selectWallet(
      uint16_t id, const char* hdPath = DEFAULT_HD_PATH, const char* bip32Passphrase = "",
      ChainType inChainType = UNKNOWN
  );

  std::string getPublicKey();
  std::string getAddress();

  WalletResponse signDigest(std::string& hexDigest);
  WalletResponse signMessage(std::string& message, ChainType chainTypeOverride = UNKNOWN);
  WalletResponse signTypedDataHash(std::string& domainSeparatorHash, std::string& messageHash);

 private:
  void deleteHdNode();
  bool setHdPath(const char* hdPath);

 protected:
  bool locked;
  uint8_t pwHash[HASH_LENGTH];
  HDNode hdNode;

  void decryptStoredMnemonic(uint16_t id, std::string& output, uint8_t encKey[HASH_LENGTH]);
  void encryptAndStoreMnemonic(uint16_t id, std::string& mnemonic, uint8_t encKey[HASH_LENGTH]);
};
