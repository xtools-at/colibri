#pragma once

// VSCode shims to make IntelliSense work mostly
// TODO: this may break platform.io-builds
#if (!defined(ARDUINO) || ARDUINO < 100)
  #undef ARDUINO
  #define ARDUINO 20320
  #define ARDUINO_ARCH_ESP32
  #define ARDUINO_BOARD "esp32dev"
  #define ESP32
  #define ESP_PLATFORM
  #define CONFIG_IDF_TARGET "esp32s3"
  #define CORE_DEBUG_LEVEL 4
  #define LED_BUILTIN 2
  #define __XTENSA__
  #warning "Please compile this project in a recent version of Arduino IDE"
#endif

#include <Arduino.h>

// You can find the user configuration in the following files:
#include "config_board.h"
#include "config_custom.h"

// !! DO NOT mess with the rest of this file unless you know what you're doing !!
#include "types.h"

// ========== System ========== //
#define HW_NAME "Colibri"
#define HW_FIRMWARE_VERSION "0.0.1"
#ifndef DISPLAY_TYPE_ID
  #ifndef DISPLAY_ENABLED
    #define DISPLAY_TYPE_ID 0  // No screen
  #elif (DISPLAY_WIDTH < 80 || !defined(BUTTON_GPIO_CANCEL))
    #define DISPLAY_TYPE_ID 1  // Small screen or limited input
  #else
    #define DISPLAY_TYPE_ID 2  // Full-featured screen
  #endif
#endif

// ========== Debug ========== //
// Additional flag to log sensitive information (mnemonics, password, etc.)
// #define DEBUG_LOG_SENSITIVE 1

// Arduino core serial debug log level (insecure) - should be set in Arduino IDE directly instead
#ifndef CORE_DEBUG_LEVEL
  // log level from 0 (none) to 5 (verbose)
  #define CORE_DEBUG_LEVEL 0
#endif

// general debug flag and extended logger
#if (CORE_DEBUG_LEVEL > 0)
  #warning "!!! Debugging options enabled. Please make sure this is intentional !!!"

  // extend Arduino log methods
  #define log_print(...) Serial.print(__VA_ARGS__)
  #define log_println(...) Serial.println(__VA_ARGS__)
  #define log_printf(...) Serial.printf(__VA_ARGS__)

  // if Arduino IDE core log level is >= 'debug'
  #if (CORE_DEBUG_LEVEL >= 4)
    // enable debug serial RPC interface (insecure)
    #define DEBUG_INTERFACE_SERIAL

    // optional sensitive information logging
    #if (defined(DEBUG_LOG_SENSITIVE) && DEBUG_LOG_SENSITIVE > 0)
      #warning "XXX Sensitive information logging enabled, data may leak XXX"
      #define log_s(...) log_d(__VA_ARGS__)
    #else
      #define log_s(...)
    #endif
  #endif

#else
  #define log_s(...)
  #define log_print(...)
  #define log_println(...)
  #define log_printf(...)
#endif

// ========== Board defaults ========== //
// enable interfaces based on target chip and config
#if (defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32C3) || \
     defined(CONFIG_IDF_TARGET_ESP32C2))
  #ifndef INTERFACE_USB_DISABLED
    #define INTERFACE_USB_DISABLED
  #endif
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
  #ifndef INTERFACE_BLE_DISABLED
    #define INTERFACE_BLE_DISABLED
  #endif
#endif

// - enable BLE interfaces
#if !defined(INTERFACE_BLE_DISABLED)
  #if !defined(INTERFACE_BLE_NIMBLE_DISABLED)
    #define INTERFACE_BLE_NIMBLE
  #else
    #define INTERFACE_BLE_DISABLED
  #endif
#endif

/*
// TODO: enable when interfaces are implemented
// - no USB on V1 & C2/C3
#if !defined(INTERFACE_USB_DISABLED)
  #if !defined(DEBUG_INTERFACE_SERIAL)
    #if !defined(INTERFACE_USB_WEBUSB_DISABLED)
      #define INTERFACE_USB_WEBUSB
    #else
      #define INTERFACE_USB_DISABLED
    #endif
  #endif
#endif
*/

// Defaults
// - system
#ifndef BOARD_SERIAL_BAUD_RATE
  #define BOARD_SERIAL_BAUD_RATE 115200
#endif

// - storage
#ifndef NVS_MAX_AVAILABLE_STORAGE
  #define NVS_MAX_AVAILABLE_STORAGE 7000
#endif

// - ESP32 hardware RNG depends on antenna activity
#if defined(INTERFACE_BLE_DISABLED)
  #define RNG_ANTENNA_DISABLED
#endif

// - LED defaults
#if (!defined(LED_GPIO) && defined(LED_BUILTIN))
  #define LED_GPIO LED_BUILTIN
#endif
#if (!defined(DISPLAY_ENABLED) && !defined(LED_GPIO) && !defined(LED_GPIO_NEOPIXEL))
  #error "LED pin is required"
#endif

#ifndef LED_ON
  #define LED_ON HIGH
  #define LED_OFF LOW
#else
  #if (LED_ON == HIGH)
    #define LED_OFF LOW
  #else
    #define LED_OFF HIGH
  #endif
#endif

// - button defaults
#ifndef BUTTON_GPIO_OK
  #define BUTTON_GPIO_OK 0
#endif

#ifndef BUTTON_MODE
  #define BUTTON_MODE INPUT_PULLUP
#endif

#ifndef BUTTON_PUSHED
  #define BUTTON_PUSHED LOW
#endif

// - button layouts
#ifdef BUTTON_GPIO_CANCEL
  #define BUTTON_LAYOUT_TWO
#else
  #define BUTTON_LAYOUT_ONE
#endif

// ========== Crypto ========== //
// Trezor lib config  - MOVED TO LIB
// #define USE_BIP39_CACHE 0
// #define USE_BIP32_CACHE 0
// #define USE_ETHEREUM 1
// #define USE_KECCAK 1
// #define USE_PRECOMPUTED_CP 0

// Constants
#define PRIVATEKEY_LENGTH 32
#define PUBLICKEY_LENGTH 33
#define HASH_LENGTH 32
#define SIGNATURE_LENGTH 64
#define RECOVERABLE_SIGNATURE_LENGTH (SIGNATURE_LENGTH + 1)
#define AES_IV_SIZE 16
#define BIP39_SEED_SIZE 64
#define ADDRESS_LENGTH 20
#define XPUB_LENGTH 111
#define MAX_HDPATH_LENGTH 44  // (5 * 7) + 9
#define MAX_ADDRESS_LENGTH 100

// Keystore
#define MIN_PASSPHRASE_LENGTH 12
#define MAX_PASSPHRASE_LENGTH 256
#define MAX_MNEMONIC_LENGTH 196
#define SYSTEM_STORAGE (2 * HASH_LENGTH + AES_IV_SIZE + 32)
#define MAX_STORED_KEYS \
  ((NVS_MAX_AVAILABLE_STORAGE - SYSTEM_STORAGE) / (MAX_MNEMONIC_LENGTH + AES_IV_SIZE + 16))

// TODO: move into network config, these values are for Bitcoin only
// #define ADDRESS_TYPE_LEGACY 0  // P2PKH
// #define ADDRESS_TYPE_P2SH 5  // P2WPKH-P2SH
// #define ADDRESS_TYPE_SEGWIT 6  // P2WPKH

// ========== Interfaces config ========== //
// General
#define TIMEOUT_WAIT_FOR_APPROVAL 12000

// NimBLE
// - lib config - MOVED TO LIB
// #define CONFIG_BT_NIMBLE_MAX_CONNECTIONS 1
// #define CONFIG_BT_NIMBLE_MAX_BONDS 5
// #define CONFIG_BT_NIMBLE_ROLE_CENTRAL_DISABLED
// #define CONFIG_BT_NIMBLE_ROLE_OBSERVER_DISABLED

// - BLE service config
#ifndef BLE_SERVER_NAME
  #define BLE_SERVER_NAME HW_NAME
#endif
#define BLE_SERVICE_UUID "183D"
#define BLE_DEFAULT_INPUT "RPC Input"
#define BLE_DEFAULT_OUTPUT "RPC Output"
#define BLE_CHARACTERISTIC_INPUT "A001"
#define BLE_CHARACTERISTIC_OUTPUT "A002"