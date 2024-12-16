// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

// ========== USER CONFIG ========== //

// set your Bluetooth device name (defaults to "Colibri")
// #define BLE_SERVER_NAME "MyOwnWallet"

// set your own BLE pairing key (6 digit number, "Minimal" devices only)
#define BLE_PAIRING_KEY 314159

// change the default wallet/chain
// - Bitcoin SegWit:
// #define DEFAULT_HD_PATH "m/84'/0'/0'/0/0"
// - Bitcoin Legacy (most compatible):
// #define DEFAULT_HD_PATH "m/44'/0'/0'/0/0"
// - Ethereum & Co:
#define DEFAULT_HD_PATH "m/44'/60'/0'/0/0"

// mnemonic lengths 12|18|24 are supported
#define DEFAULT_MNEMONIC_WORDS 24

// custom disable interfaces
// #define INTERFACE_BLE_DISABLED
// #define INTERFACE_BLE_NIMBLE_DISABLED
// #define INTERFACE_USB_DISABLED
// #define INTERFACE_USB_WEBUSB_DISABLED
