// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../../config.h"

#ifdef OTA_ENABLED
  #include <ESPmDNS.h>
  #include <WebServer.h>
  #include <WiFi.h>
  #include <esp_system.h>

  #include "ElegantOTA/ElegantOTA.h"

void initOta();
void stopOta();
void updateOta();
#else
  #define initOta()
  #define stopOta()
  #define updateOta()
#endif
