#pragma once

#include "Storage.h"

void Storage::wipe() {
  // delete storage keys
  preferences.begin(STORAGE_KEYS);
  preferences.clear();
  preferences.end();
  delay(10);

  preferences.begin(STORAGE_IVS);
  preferences.clear();
  preferences.end();
  delay(10);

  preferences.begin(STORAGE_SYS);
  preferences.clear();
  preferences.end();
  delay(10);

  // format file system
  nvs_flash_erase();
  delay(50);
  nvs_flash_init();
  delay(50);
}

uint16_t Storage::readWalletCounter() {
  preferences.begin(STORAGE_SYS);
  uint16_t counter = preferences.getUShort(STORAGE_SYS_COUNTER);
  preferences.end();

  return counter;
}

void Storage::writeWalletCounter(uint16_t counter) {
  preferences.begin(STORAGE_SYS);
  preferences.putUShort(STORAGE_SYS_COUNTER, counter);
  preferences.end();
}

bool Storage::writeMnemonic(uint16_t index, const uint8_t* mnemonic, size_t len) {
  return putBytes(STORAGE_KEYS, index, mnemonic, len);
}

size_t Storage::readMnemonic(uint16_t index, uint8_t* mnemonic) {
  return getBytes(STORAGE_KEYS, index, mnemonic, MAX_MNEMONIC_LENGTH);
}

bool Storage::hasMnemonic(uint16_t index) {
  return hasBytes(STORAGE_KEYS, index, MAX_MNEMONIC_LENGTH);
}

bool Storage::writeIv(uint16_t index, const uint8_t* iv) {
  return putBytes(STORAGE_IVS, index, iv, AES_IV_SIZE);
}

size_t Storage::readIv(uint16_t index, uint8_t* iv) {
  return getBytes(STORAGE_IVS, index, iv, AES_IV_SIZE);
}

bool Storage::hasIv(uint16_t index) { return hasBytes(STORAGE_IVS, index, AES_IV_SIZE); }

bool Storage::putBytes(const char* category, const char* key, const uint8_t* value, size_t len) {
  preferences.begin(category);
  size_t savedLen = preferences.putBytes(key, value, len);
  preferences.end();

  return savedLen == len;
}

bool Storage::putBytes(const char* category, uint16_t index, const uint8_t* value, size_t len) {
  if (isOutOfBounds(index)) {
    return false;
  }

  char key[4];
  utoa(index, key, 10);

  return putBytes(category, key, value, len);
}

size_t Storage::getBytes(const char* category, const char* key, uint8_t* value, size_t maxLen) {
  preferences.begin(category);
  size_t len = preferences.getBytes(key, value, maxLen);
  preferences.end();

  return len;
}

size_t Storage::getBytes(const char* category, uint16_t index, uint8_t* value, size_t maxLen) {
  if (isOutOfBounds(index)) {
    return 0;
  }

  char key[4];
  utoa(index, key, 10);

  return getBytes(category, key, value, maxLen);
}

bool Storage::isAllZero(const uint8_t* value, size_t len) {
  if (len == 0) return true;

  for (size_t i = 0; i < len; i++) {
    if (value[i] != 0) {
      return false;
    }
  }

  return true;
}

bool Storage::isOutOfBounds(uint16_t index) { return index > uint16_t(MAX_STORED_KEYS); }

bool Storage::hasBytes(const char* category, uint16_t index, size_t len) {
  if (isOutOfBounds(index)) {
    return false;
  }

  uint8_t value[len];
  size_t byteslen = getBytes(category, index, value, len);
  if (byteslen == 0) {
    return false;
  }

  bool hasValue = !isAllZero(value, len);
  memzero(value, len);

  return hasValue;
}
