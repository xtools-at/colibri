// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"

#if defined(INTERFACE_BLE_NIMBLE)
  #include "NimBLEInterface.h"
#endif

#if defined(INTERFACE_BLE_ARDUINO)
  #include "ArduinoBLEInterface.h"
#endif

#if defined(DEBUG_INTERFACE_SERIAL)
  #include "SerialDebugInterface.h"
#endif

void initInterfaces();
void updateInterfaces();
void stopInterfaces();
void wipeInterfaces();
