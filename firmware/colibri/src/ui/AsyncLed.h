// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"

#ifdef LED_GPIO_NEOPIXEL
  #include "esp32-hal-rgb-led.h"
#endif

class AsyncLed {
 public:
  bool isBlinking;
  bool isNeopixel;

  AsyncLed(uint8_t ledPin, bool neopixel = false);
  ~AsyncLed();

  void update();
  void turnOn(RgbColor color = Default);
  void turnOff();
  void blink(uint16_t timeOn, uint16_t timeOff, RgbColor color = Default, uint16_t count = 0);
  void indicate(bool success);

 private:
  uint8_t pin;
  uint8_t ledState;
  uint16_t blinkTimeOn;
  uint16_t blinkTimeOff;
  uint16_t targetBlinkCount;
  uint16_t blinkCount;
  RgbColor ledRgbColor;
  uint64_t lastBlinkTime;

#ifdef LED_GPIO_NEOPIXEL
  uint8_t getColorComponent(RgbColor color, uint8_t component);
  void setPixelRgbColor(RgbColor color);
#endif
};
