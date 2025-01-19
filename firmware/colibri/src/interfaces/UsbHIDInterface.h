// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "AbstractInterface.h"

#if defined(INTERFACE_USB_HID)

  #ifndef ARDUINO_USB_MODE
    #error "No native USB interface detected for current ESP32 board"
  #elif ARDUINO_USB_MODE == 1  // HW Serial output via USB
    #error "USB must be in OTG mode for HID to work"
  #endif

  #include <USB.h>
  #include <USBHID.h>

/*
 * Experimental USB HID interface
 */
class UsbHIDInterface : public Interface {
 public:
  UsbHIDInterface() : Interface(), inputBuffer(""), connected(false) {};

  void init();
  void update();
  void stop();
  void disconnect();
  void wipe();

 private:
  std::string inputBuffer;
  bool connected;
};

#endif
