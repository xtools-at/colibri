// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#define LED_GPIO 8
#define LED_ON 0  // == LOW

// Onboard 0.42" OLED display config
#define DISPLAY_ENABLED
#define DISPLAY_SH110X

#define DISPLAY_HEIGHT 50
#define DISPLAY_WIDTH 72
#define DISPLAY_OFFSET_X 30
#define DISPLAY_OFFSET_Y 14

#define DISPLAY_GPIO_I2C_SDA 5
#define DISPLAY_GPIO_I2C_SCL 6
