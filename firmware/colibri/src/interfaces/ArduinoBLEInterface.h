// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "AbstractInterface.h"
// Interface needs to be included first

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "../utils/trng.h"

class ArduinoBLEInterface : public Interface {
 public:
  ArduinoBLEInterface() : Interface() {};

  void init();
  void update();
  void stop();
  void wipe();

 private:
  BLEServer *pServer;
  BLEService *pService;
  BLEAdvertising *pAdvertising;

  BLECharacteristic *pCharInput;
  BLECharacteristic *pCharOutput;

  void sendResponse(String &data);
  void disconnect();
};
