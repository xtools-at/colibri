// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#define LED_GPIO_NEOPIXEL 8
#define LED_ON 0  // == LOW

// Onboard 1.47" OLED display config
#define DISPLAY_ENABLED
#define DISPLAY_ST7789
#define DISPLAY_WIDTH 172
#define DISPLAY_HEIGHT 320
#define DISPLAY_ROTATION 3
#define DISPLAY_OFFSET_X 34
#define DISPLAY_INVERT true

// Display pins
#define DISPLAY_GPIO_SPI_MOSI 6
#define DISPLAY_GPIO_SPI_SCLK 7
#define DISPLAY_GPIO_SPI_DC 15
#define DISPLAY_GPIO_CS 14
#define DISPLAY_GPIO_RST 21
#define DISPLAY_GPIO_BACKLIGHT 22
