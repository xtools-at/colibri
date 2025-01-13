// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"
// config needs to be included first

#include <Preferences.h>
#include <nvs_flash.h>
extern "C" {
#include <memzero.h>
}

#include "../../constants.h"
#include "../utils/helpers.h"

class Storage {
 public:
  void writeWalletCounter(uint16_t counter);
  uint16_t readWalletCounter();
  void writeLoginAttempts(uint16_t attempts);
  uint16_t readLoginAttempts();

  bool writeMnemonic(uint16_t index, const uint8_t* mnemonic, size_t len);
  size_t readMnemonic(uint16_t index, uint8_t* mnemonic);
  bool hasMnemonic(uint16_t index);
  bool writeIv(uint16_t index, const uint8_t* iv);
  size_t readIv(uint16_t index, uint8_t* iv);
  bool hasIv(uint16_t index);

  void wipe();
  bool putBytes(const char* category, const char* key, const uint8_t* value, size_t len);
  size_t getBytes(const char* category, const char* key, uint8_t* value, size_t maxLen);
  bool isOutOfBounds(uint16_t index);

 private:
  bool putBytes(const char* category, uint16_t index, const uint8_t* value, size_t len);
  size_t getBytes(const char* category, uint16_t index, uint8_t* value, size_t maxLen);
  bool hasBytes(const char* category, uint16_t index, size_t len);
  void putUShort(const char* category, uint16_t number);
  uint16_t getUShort(const char* category);

  Preferences preferences;
};
