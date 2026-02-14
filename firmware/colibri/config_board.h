// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <Arduino.h>

// ========== PRE-CONFIGURED BOARDS ========== //
// >>>> Either UNCOMMENT to select a prebuilt board configuration:

// - Simple boards without displays:
// #include "src/boards/32_devkit.h";
// #include "src/boards/c3_supermini.h";
// #include "src/boards/c6_supermini.h";
// #include "src/boards/h2_supermini.h";
// #include "src/boards/s3_supermini.h";

// - Boards with displays:
// #include "src/boards/32_t-display_1_14in_lcd.h";
// #include "src/boards/c3_mini_0_42in_oled.h";
// #include "src/boards/c6_waveshare_1_47in_lcd.h";
// #include "src/boards/s3_adafruit_feather_1_14in_tft.h";

// - Experimental/developers only:
// #include "src/boards/s2_wemos_d1mini.h";
// #include "src/boards/s3_wroom_camera.h";

//*/
// >>>> ...OR set up custom board below:

// ========== CUSTOM BOARD CONFIG ========== //
// All available configuration options are listed here with their default values.

// Choose LED indicator
// #define LED_GPIO 7 // default: LED_BUILTIN
// #define LED_GPIO_NEOPIXEL 8 // default: not defined
// #define LED_NEOPIXEL_BRIGHTNESS_PERCENT 25
// #define LED_NEOPIXEL_COLOR_ORDER LED_COLOR_ORDER_GRB

// Setup the buttons your board has
// #define BUTTON_GPIO_OK 0 // default: BOOT button (0 or 9 depending on chip)
// #define BUTTON_GPIO_CANCEL 1 // default: not defined

// Alternative input/UI modes
// #define BUTTON_MODE INPUT_PULLUP
// #define BUTTON_PUSHED LOW
// #define LED_ON HIGH

// ========== DISPLAY CONFIG ========== //
// #define DISPLAY_ENABLED

//*/
// > Display config - uncomment to select a type:
// - SSD1306 OLED displays
// #define DISPLAY_SSD1306

// - SH110x OLED displays (SH1106, SH1107)
// #define DISPLAY_SH110X

// - ST77xx TFT displays (SPI)
// #define DISPLAY_ST7735
// #define DISPLAY_ST7735S
// #define DISPLAY_ST7789
// #define DISPLAY_ST7796

// - ILIxxxx TFT displays (SPI)
// #define DISPLAY_ILI9341

//*/
// > Panel config:
// #define DISPLAY_HEIGHT 160 // default: not defined
// #define DISPLAY_WIDTH 80 // default: not defined
// #define DISPLAY_BRIGHTNESS 200 // 0-255
// #define DISPLAY_COLOR_DEPTH 16 // 1/2/4/8/16/24
// #define DISPLAY_ROTATION 0 // 0-4
// #define DISPLAY_INVERT false
// #define DISPLAY_OFFSET_X 0
// #define DISPLAY_OFFSET_Y 0
// #define DISPLAY_OFFSET_ROTATION 0

// > Connection and pin config:
// - 2-wire I2C
// #define DISPLAY_GPIO_I2C_SDA 5 // default: not defined
// #define DISPLAY_GPIO_I2C_SCL 6 // default: not defined

// - 3/4-wire SPI:
// #define DISPLAY_GPIO_SPI_MOSI 3 // aka SDA - default: not defined
// #define DISPLAY_GPIO_SPI_MISO -1 // -1 if using 3-wire SPI display
// #define DISPLAY_GPIO_SPI_SCLK 2 // aka SCK - default: not defined
// #define DISPLAY_GPIO_SPI_DC 6 // default: not defined

//*/
// > Panel pin config:
// #define DISPLAY_GPIO_CS -1 // -1 to disable
// #define DISPLAY_GPIO_BACKLIGHT -1 // -1 to disable
// #define DISPLAY_GPIO_RST -1  // -1 if NC or connected to EN

// - QR code config
// #define QR_MAX_VERSION 8 // 1-40, influences QR code capacity, size on screen and required performance
// #define QR_MODULE_DRAW_SIZE 4 // size of each QR code *module* (square) in pixels, influences QR code size on screen

// ========== CAMERA CONFIG ========== //
// [ESP32, S2, S3] only!
// #define CAMERA_ENABLED

// - Pin config (no defaults, all required)
// #define CAMERA_GPIO_PWDN -1
// #define CAMERA_GPIO_RESET -1
// #define CAMERA_GPIO_XCLK 21
// #define CAMERA_GPIO_SIOD 26
// #define CAMERA_GPIO_SIOC 27
// #define CAMERA_GPIO_D7 35
// #define CAMERA_GPIO_D6 34
// #define CAMERA_GPIO_D5 39
// #define CAMERA_GPIO_D4 36
// #define CAMERA_GPIO_D3 19
// #define CAMERA_GPIO_D2 18
// #define CAMERA_GPIO_D1 5
// #define CAMERA_GPIO_D0 4
// #define CAMERA_GPIO_VSYNC 25
// #define CAMERA_GPIO_HREF 23
// #define CAMERA_GPIO_PCLK 22

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
