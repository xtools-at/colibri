// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "Display.h"

#ifdef DISPLAY_ENABLED

static TFT_eSPI display;
// static TFT_eSprite sprite(&display);

void drawText(const char* message) {
  display.startWrite();
  display.clearDisplay();
  display.fillScreen(DISPLAY_COLOR_BG);
  display.setTextColor(DISPLAY_COLOR_TEXT);

  display.setFont(&fonts::Font0);
  float textSize = 1.0f;
  #ifndef DISPLAY_SMALL
  textSize = 2.0f;
  #endif

  display.setTextSize(textSize);
  display.setTextDatum(middle_center);
  display.drawString(message, display.width() / 2, display.height() / 2);

  display.drawRect(5, 5, display.width() - 10, display.height() - 10, DISPLAY_COLOR_TEXT);
  display.endWrite();
}

void initDisplay() {
  display.setRotation(DISPLAY_ROTATION);
  display.setBrightness(DISPLAY_BRIGHTNESS);
  display.setColorDepth(DISPLAY_COLOR_DEPTH);
  display.begin();

  drawText("COLIBRI");
  log_d("Initialized display: %s", DISPLAY_TYPE);
  delay(500);
}

void drawQrCode(esp_qrcode_handle_t qrCode) {
  #ifndef DISPLAY_SMALL

  display.startWrite();
  display.clearDisplay();
  display.fillScreen(TFT_LIGHTGREY);

  int size = esp_qrcode_get_size(qrCode);
  int32_t pixels = QR_MODULE_DRAW_SIZE;

  for (size_t y = 0; y < size; y++) {
    for (size_t x = 0; x < size; x++) {
      int color = TFT_WHITE;
      if (esp_qrcode_get_module(qrCode, x, y)) {
        color = TFT_BLACK;
      }
      display.fillRect(x * pixels, y * pixels, pixels, pixels, color);
    }
  }

  display.endWrite();

  #endif
}

#endif
