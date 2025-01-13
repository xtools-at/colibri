// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "interfaces.h"
#if defined(INTERFACE_BLE_NIMBLE)
NimBLEInterface nimBleInterface = NimBLEInterface();
#endif

#if defined(INTERFACE_BLE_ARDUINO)
ArduinoBLEInterface arduinoBleInterface = ArduinoBLEInterface();
#endif

#if defined(DEBUG_INTERFACE_SERIAL)
SerialDebugInterface serialDebugInterface = SerialDebugInterface();
#endif

void initInterfaces() {
#if defined(DEBUG_INTERFACE_SERIAL)
  serialDebugInterface.init();
#endif

#if defined(INTERFACE_BLE_NIMBLE)
  nimBleInterface.init();
#endif

#if defined(INTERFACE_BLE_ARDUINO)
  arduinoBleInterface.init();
#endif
};

void updateInterfaces() {
#if defined(DEBUG_INTERFACE_SERIAL)
  serialDebugInterface.update();
#endif

#if defined(INTERFACE_BLE_NIMBLE)
  nimBleInterface.update();
#endif

#if defined(INTERFACE_BLE_ARDUINO)
  arduinoBleInterface.update();
#endif
};

void stopInterfaces() {
  log_d("Stopping interfaces...\n");
#if defined(DEBUG_INTERFACE_SERIAL)
  serialDebugInterface.stop();
#endif

#if defined(INTERFACE_BLE_NIMBLE)
  nimBleInterface.stop();
#endif

#if defined(INTERFACE_BLE_ARDUINO)
  arduinoBleInterface.stop();
#endif
};

void wipeInterfaces() {
  log_d("Wiping interfaces...\n");
#if defined(DEBUG_INTERFACE_SERIAL)
  serialDebugInterface.wipe();
#endif

#if defined(INTERFACE_BLE_NIMBLE)
  nimBleInterface.wipe();
#endif

#if defined(INTERFACE_BLE_ARDUINO)
  arduinoBleInterface.wipe();
#endif
};
