// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

// ========== USER CONFIG ========== //

// set your Bluetooth device name (defaults to "Colibri")
#define BLE_SERVER_NAME "Colibri"

// set your own BLE pairing key (6 digit number).
// devices with a display will generate a random key each boot.
#define BLE_PAIRING_KEY 200913

// change the default wallet/chain
// - Bitcoin SegWit:
// #define DEFAULT_HD_PATH "m/84'/0'/0'/0/0"
// - Bitcoin Legacy (most compatible):
// #define DEFAULT_HD_PATH "m/44'/0'/0'/0/0"
// - Ethereum & Co:
#define DEFAULT_HD_PATH "m/44'/60'/0'/0/0"

// mnemonic lengths 12|18|24 are supported
#define DEFAULT_MNEMONIC_WORDS 24

// change wait time for user to approve actions (in ms)
#define TIMEOUT_WAIT_FOR_APPROVAL 12000  // minimum value: 5000

// self destruct wallet after n failed unlock attempts
#define SELF_DESTRUCT_ENABLED false
#define SELF_DESTRUCT_MAX_FAILED_ATTEMPTS 10  // minimum value: 3

// custom disable interfaces
// #define INTERFACE_BLE_DISABLED
// #define INTERFACE_BLE_NIMBLE_DISABLED
// #define INTERFACE_USB_DISABLED
// #define INTERFACE_USB_WEBUSB_DISABLED

// ========== EXPERT/DEVELOPER SETTINGS ========== //

// Use custom serial baud rate
// #define BOARD_SERIAL_BAUD_RATE 115200

// Change the available board storage (NVS partition)
// -- Arduino IDE reserves 20k for NVS storage by default, but that's shared with BLE & Co.
// -- Using the config default of ~7k here is enough to store ~30 mnemonic phrases.
// -- Increase only if you've created an additional custom NVS partition with more space.
// #define NVS_MAX_AVAILABLE_STORAGE 7280

// Use custom NVS partition
// -- If you've added a custom NVS partition, update its name here
// #define NVS_PARTITION_NAME "nvs"
