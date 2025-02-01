// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "AbstractInterface.h"

#if defined(INTERFACE_BLE_NIMBLE)

  #include <NimBLEDevice.h>
  #include <NimBLEServer.h>

  #include "../utils/trng.h"

NimBLECharacteristic *pCharChunkLen;

class NimBLEInterface : public Interface {
 public:
  NimBLEInterface() : Interface() {};

  void init();
  void update();
  void stop();
  void wipe();

 private:
  NimBLEServer *pServer;
  NimBLEService *pService;
  NimBLEAdvertising *pAdvertising;

  NimBLECharacteristic *pCharInput;
  NimBLECharacteristic *pCharOutput;

  void sendResponse(std::string &data);
  void disconnect();
};
#endif
