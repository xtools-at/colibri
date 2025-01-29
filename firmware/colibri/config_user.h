// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

// ========== USER CONFIG ========== //

// > Bluetooth settings

// - set your Bluetooth device name (defaults to "Colibri")
// #define BLE_SERVER_NAME "Colibri"

// - set your own BLE pairing key (6 digit number). devices with a display will generate a random
// key each boot.
// #define BLE_PAIRING_KEY 200913

// > Wallet settings

// - change the default wallet/chain (pick one)
// #define DEFAULT_HD_PATH "m/44'/60'/0'/0/0" // Ethereum & Co
// #define DEFAULT_HD_PATH "m/84'/0'/0'/0/0" // Bitcoin SegWit
// #define DEFAULT_HD_PATH "m/49'/0'/0'/0/0" // Bitcoin SegWit P2SH
// #define DEFAULT_HD_PATH "m/44'/0'/0'/0/0" Bitcoin Legacy (most compatible)

// - seed phrase lengths 12|18|24 are supported
// #define DEFAULT_MNEMONIC_WORDS 24

// - change wait time for user to approve actions (in ms)
// #define TIMEOUT_WAIT_FOR_APPROVAL (12 * 1000)  // == 12sec; minimum value: 5000 (5sec)

// - change timeout for auto-locking the device after inactivity (in ms)
// #define TIMEOUT_INACTIVITY_LOCK (10 * 60 * 1000)  // == 10min; minimum value: 20000 (20sec)

// - self destruct wallet after n failed unlock attempts
// #define SELF_DESTRUCT_ENABLED false
// #define SELF_DESTRUCT_MAX_FAILED_ATTEMPTS 10  // minimum value: 3

// > Interface settings

// - disable interfaces
// -- generally:
// #define INTERFACE_BLE_DISABLED
// #define INTERFACE_USB_DISABLED
// -- specific ones:
// #define INTERFACE_BLE_NIMBLE_DISABLED
// #define INTERFACE_BLE_ARDUINO_DISABLED
// #define INTERFACE_USB_HID_DISABLED

// - OTA settings
// #define OTA_WIFI_SSID "Colibri OTA Update"  // WiFi access point SSID for OTA updates
// #define OTA_WIFI_PASSWORD "ColibriOTAUpdate" // min. 8 characters

// > Developer settings

// #define OTA_ENABLED  // test OTA updates
// #define OTA_USE_SPIFFS // alternatively serving data from SPIFFS
// #define INTERFACE_QR  // test BC-UR QR code interface
