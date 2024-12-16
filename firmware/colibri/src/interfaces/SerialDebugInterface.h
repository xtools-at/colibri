// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "AbstractInterface.h"

/*
 * Insecure serial interface without any TLS,
 * for debugging with test keys only.
 *
 * Do NOT use in production!
 */
class SerialDebugInterface : public Interface {
 public:
  SerialDebugInterface() : Interface(), inputBuffer(""), connected(false) {};

  void init();
  void update();
  void stop();
  void disconnect();
  void wipe();

 private:
  std::string inputBuffer;
  bool connected;
};
