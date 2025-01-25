// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

// config first
#include "../../config.h"

// then WiFi...
#include <WiFi.h>
// ...then WebServer...
#include <WebServer.h>
// ...then OTA
#include <ElegantOTA.h>

class OTA {
 public:
  OTA() {};

  void init();
  void stop();
  void update();
};
