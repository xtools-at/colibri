// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

// extern "C" {
//  #include <segwit_addr.h>
//  #include <zkp_bip340.h>
// }

#include "../../config.h"
#include "../chains/Ethereum.h"
#include "../utils/chains.h"
#include "../utils/crypto.h"
#include "../utils/ui.h"
#include "Storage.h"
#include "esp_system.h"

extern Storage store;
extern EthereumChain ethereum;

class Wallet {
 public:
  Wallet() : locked(true) {}

  std::string hdPath;
  std::string xPub;
  uint16_t walletId;
  uint16_t storedMnemonics;
  uint32_t fingerprint;
  uint32_t bipId;
  uint32_t slip44;
  int chainType;

  bool isLocked();
  bool isPasswordSet();
  bool isKeySet();

  void wipe();
  void lock();

  WalletResponse setPassword(std::string& password);
  WalletResponse addMnemonic(std::string& mnemonic);
  WalletResponse createMnemonic(uint8_t words = DEFAULT_MNEMONIC_WORDS);
  WalletResponse unlock(std::string& password, bool requiresApproval = true);
  WalletResponse wipeRemote();
  WalletResponse selectWallet(
      uint16_t id, const char* hdPath = DEFAULT_HD_PATH, const char* bip32Passphrase = "",
      ChainType inChainType = UNKNOWN
  );

  std::string getPublicKey();
  std::string getAddress();

  WalletResponse signMessage(std::string& message);
  WalletResponse signTypedData(std::string& domainSeparatorHash, std::string& messageHash);

 private:
  bool locked;
  uint8_t pwHash[HASH_LENGTH];
  HDNode hdNode;

  void deleteHdNode();
  bool setHdPath(const char* hdPath);
};
