#pragma once

#include "AbstractInterface.h"
// Interface needs to be included first

#include <NimBLEDevice.h>
#include <NimBLEServer.h>

#include "../utils/trng.h"

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
