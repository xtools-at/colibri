// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <Arduino.h>

// ========== PRE-CONFIGURED BOARDS ========== //
// Either UNCOMMENT to select board:

// -- Actively supported boards (more coming soon!):
// #include "src/boards/c3_supermini.h";
// #include "src/boards/s3_supermini.h";

// -- TODO: Not yet tested:
// #include "src/boards/32_devkit.h";
// #include "src/boards/c3_luatos.h";
// #include "src/boards/c3_luatos_air101lcd.h";
// #include "src/boards/c3_mini_042in_oled.h";
// #include "src/boards/s2_wemos_d1mini.h";

// ...OR set up custom board below:

// ========== CUSTOM BOARD CONFIG ========== //
// All available configuration options are listed here with their default values.

// Choose LED indicator
// #define LED_GPIO 7 // default: LED_BUILTIN
// #define LED_GPIO_NEOPIXEL 8 // default: not defined

// Setup the buttons your board has
// #define BUTTON_GPIO_OK 0
// #define BUTTON_GPIO_CANCEL 1 // default: not defined
// #define BUTTON_GPIO_PREV 2 // default: not defined // UNUSED
// #define BUTTON_GPIO_NEXT 3  // default: not defined // UNUSED

// Alternative input/UI modes
// #define BUTTON_MODE INPUT_PULLUP
// #define BUTTON_PUSHED LOW
// #define LED_ON HIGH

// !! TODO: The following has not yet been implemented: !!

// ========== DISPLAY CONFIG ========== //
// #define DISPLAY_ENABLED

// Uncomment to select a display type:
// - SSD1306 OLED displays
// #define DISPLAY_SSD1306_I2C  // (I2C - 2-pin)
// #define DISPLAY_SSD1306_SPI  // (SPI - 4-pin)

// - ST77XX TFT displays (SPI - 4-pin)
// #define DISPLAY_ST7735
// #define DISPLAY_ST7789

// -- ST7735-specific config:
// #define DISPLAY_ST7735_TYPE_BLACKTAB  // 1.8" TFT
// #define DISPLAY_ST7735_TYPE_GREENTAB  // 1.8" TFT with offset
// #define DISPLAY_ST7735_TYPE_144GREENTAB  // 1.44" TFT
// #define DISPLAY_ST7735_TYPE_REDTAB
// #define DISPLAY_ST7735_TYPE_MINI160x80  // ST7735S 0.96" 160x80
// #define DISPLAY_ST7735_TYPE_MINI160x80_PLUGIN  // ST7735S 0.96" 160x80 with FPC plugin
// #define DISPLAY_ST7735_TYPE_B  // new ST7735B chip

// General display config:
// #define DISPLAY_WIDTH 80
// #define DISPLAY_HEIGHT 160
// #define DISPLAY_ROTATION 1 // 0-4, default: not defined
// TODO: DISPLAY_INVERT?

// Connection and pin config:
// - I2C
// #define DISPLAY_I2C
// -- optional if using hardware I2C pins:
// #define DISPLAY_GPIO_I2C_SDA 5 // default: not defined
// #define DISPLAY_GPIO_I2C_SCL 6 // default: not defined

// - 4-Wire SPI:
// #define DISPLAY_SPI
// #define DISPLAY_GPIO_SPI_SDA 3 // aka MOSI
// #define DISPLAY_GPIO_SPI_SCK 2 // aka SCLK
// #define DISPLAY_GPIO_SPI_CS 7
// #define DISPLAY_GPIO_SPI_DC 6

// - shared:
// #define DISPLAY_GPIO_BACKLIGHT -1
// #define DISPLAY_GPIO_RST -1  // -1 if NC or connected to EN
