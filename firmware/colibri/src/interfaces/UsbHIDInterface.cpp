// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "UsbHIDInterface.h"

#if defined(INTERFACE_USB_HID)

USBHID HID;

// HID report descriptor
const uint8_t reportDescriptor[] = {
    0x06, 0xD0, 0xF1,  // Usage Page (FIDO Alliance)
    0x09, 0x01,  // Usage (U2F Authenticator Device)
    0xA1, 0x01,  // Collection (Application)
    0x09, 0x20,  // Usage (Input Report Data)
    0x15, 0x00,  // Logical Minimum (0)
    0x26, 0xFF, 0x00,  // Logical Maximum (255)
    0x75, 0x08,  // Report Size (8 bits)
    0x95, 0x40,  // Report Count (64 bytes)
    0x81, 0x02,  // Input (Data, Variable, Absolute)
    0x09, 0x21,  // Usage (Output Report Data)
    0x15, 0x00,  // Logical Minimum (0)
    0x26, 0xFF, 0x00,  // Logical Maximum (255)
    0x75, 0x08,  // Report Size (8 bits)
    0x95, 0x40,  // Report Count (64 bytes)
    0x91, 0x02,  // Output (Data, Variable, Absolute)
    0xC0  // End Collection
};

class CustomHIDDevice : public USBHIDDevice {
 public:
  CustomHIDDevice(void) {
    static bool initialized = false;
    if (!initialized) {
      initialized = true;
      HID.addDevice(this, sizeof(reportDescriptor));
    }
  }

  uint16_t _onGetDescriptor(uint8_t *buffer) {
    memcpy(buffer, reportDescriptor, sizeof(reportDescriptor));
    return sizeof(reportDescriptor);
  }

  bool send(uint8_t *value) { return HID.SendReport(0, value, 8); }
};

CustomHIDDevice hidDevice;

void UsbHIDInterface::init() {
  // TODO:
  USB.VID(0x1209);
  USB.PID(0x0001);
  USB.manufacturerName(HW_MANUFACTURER_NAME);
  USB.productName(HW_DEVICE_NAME);
  USB.firmwareVersion(HW_FIRMWARE_VERSION_NUM);
  USB.serialNumber("31415926");

  USB.webUSB(true);
  USB.webUSBURL("https://docs.espressif.com/projects/arduino-esp32/en/latest/_static/webusb.html");

  HID.begin();
  USB.begin();

  initialised = true;
}

void UsbHIDInterface::update() {
  if (HID.ready() && !connected) {
    connected = true;
    setStateConnected(UsbConnected);
  }

  if (HID.available()) {
    bool doneReading = false;

    while (HID.available()) {
      char c = HID.read();
      if (c == '{') {
        // new payload begins
        inputBuffer.clear();
      }

      inputBuffer += c;

      if (c == '}') {
        // end of payload
        doneReading = true;
        break;
      }
    }

    if (doneReading) {
      // pass input to RPC
      std::string output;
      rpc.handleRequest(inputBuffer, output);
      inputBuffer.clear();

      // return RPC response in batches if longer than 512 characters
      const size_t batchSize = 512;
      size_t outputLength = output.length();
      size_t offset = 0;

      while (offset < outputLength) {
        size_t chunkSize =
            (outputLength - offset) > batchSize ? batchSize : (outputLength - offset);
        HID.write((const uint8_t *)output.c_str() + offset, chunkSize);
        HID.flush();
        offset += chunkSize;
      }
      HID.print('\n');
      HID.flush();

      output.clear();
    }
  }
}

void UsbHIDInterface::disconnect() {
  connected = false;
  inputBuffer.clear();
  setStateConnected(NotConnected);
}

void UsbHIDInterface::stop() {
  disconnect();
  HID.end();

  initialised = false;
}

void UsbHIDInterface::wipe() { stop(); }

#endif
