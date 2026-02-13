
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "AbstractInterface.h"

#if defined(CAMERA_ENABLED)
  #include "esp_camera.h"
/*
 * Camera interface for BC-UR QR code scanning.
 */
class CameraInterface : public Interface {
 public:
  CameraInterface() : Interface() {};

  void init();
  void update();
  void stop();
  void disconnect();
  void wipe();
};

#endif
