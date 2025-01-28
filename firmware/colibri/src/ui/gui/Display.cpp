// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "Display.h"

#ifdef DISPLAY_ENABLED
static TFT_eSPI lcd;
// static TFT_eSprite sprite(&lcd);

static void drawDemo() {
  lcd.startWrite();

  lcd.setTextColor(TFT_WHITE);
  lcd.drawNumber(lcd.getRotation(), 16, 0);

  lcd.setFont(&fonts::FreeMono9pt7b);
  lcd.setTextColor(0xFF0000U);
  lcd.drawString("R", 30, 16);

  lcd.setFont(&fonts::Font2);
  lcd.setTextDatum(bottom_right);
  lcd.drawString("bottom_right", lcd.width() / 2, lcd.height() / 2);

  lcd.setFont(&fonts::FreeMono12pt7b);
  lcd.setTextColor(0x00FF00U);
  lcd.drawString("G", 40, 16);

  lcd.setFont(&fonts::Font0);
  lcd.setTextColor(0x0000FFU);
  lcd.drawString("B", 50, 16);

  lcd.drawFastVLine(lcd.width() / 2, 0, lcd.height(), 0xFFFFFFU);
  lcd.drawFastHLine(0, lcd.height() / 2, lcd.width(), 0xFFFFFFU);

  lcd.drawRect(30, 30, lcd.width() - 60, lcd.height() - 60, 2 * 7);

  lcd.endWrite();
}

void initDisplay() {
  lcd.init();
  lcd.setRotation(DISPLAY_ROTATION);
  lcd.setBrightness(DISPLAY_BRIGHTNESS);
  lcd.setColorDepth(DISPLAY_COLOR_DEPTH);

  lcd.setTextSize((std::max(lcd.width(), lcd.height()) + 255) >> 8);

  lcd.fillScreen(TFT_BLACK);

  // TODO: show splash screen instead
  drawDemo();
}

#endif
