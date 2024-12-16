#pragma once

#include "c3_luatos.h"

// Display hat

// Button setup
#undef BUTTON_GPIO_OK
#define BUTTON_GPIO_OK 4
#define BUTTON_GPIO_CANCEL 9  // down
#define BUTTON_GPIO_PREV 13
#define BUTTON_GPIO_NEXT 8
// #define BUTTON_GPIO_UP 5
// #define BUTTON_GPIO_DOWN 9

// Display config
#define DISPLAY_ENABLED
#define DISPLAY_ST7735
#define DISPLAY_ST7735_TYPE_MINI160x80

#define DISPLAY_WIDTH 80
#define DISPLAY_HEIGHT 160
// #define DISPLAY_ROTATION 1
// INVERT?

#define DISPLAY_SPI
#define DISPLAY_GPIO_SPI_SDA 3  // MOSI
#define DISPLAY_GPIO_SPI_SCK 2  // SCLK
#define DISPLAY_GPIO_SPI_CS 7
#define DISPLAY_GPIO_SPI_DC 6
#define DISPLAY_GPIO_BACKLIGHT 11
#define DISPLAY_GPIO_RST 10
