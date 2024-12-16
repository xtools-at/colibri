#pragma once

#include "AsyncLed.h"

AsyncLed::AsyncLed(uint8_t ledPin, bool neopixel)
    : pin(ledPin),
      isBlinking(false),
      blinkTimeOn(0),
      blinkTimeOff(0),
      lastBlinkTime(0),
      ledState(LED_OFF),
      blinkCount(0),
      targetBlinkCount(0),
      isNeopixel(neopixel) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LED_OFF);
}

AsyncLed::~AsyncLed() { turnOff(); }

void AsyncLed::update() {
  if (isBlinking) {
    uint64_t currentTime = millis();
    uint16_t interval = ledState == LED_ON ? blinkTimeOn : blinkTimeOff;

    if (currentTime - lastBlinkTime >= interval) {
      lastBlinkTime = currentTime;
      ledState = !ledState;  // Toggle LED state
#ifdef LED_GPIO_NEOPIXEL
      if (isNeopixel)
        setPixelRgbColor(
            ledState == LED_ON ? ledRgbColor : RgbColor::Off
        );  // Set color or turn off
#else
      if (!isNeopixel) digitalWrite(pin, ledState);
#endif
      if (targetBlinkCount > 0) {
        if (ledState == LED_OFF) {  // Count only complete on-off cycles
          blinkCount++;
          if (blinkCount > targetBlinkCount) {
            isBlinking = false;
          }
        }
      }
    }
  }
}

void AsyncLed::turnOn(RgbColor color) {
  isBlinking = false;
#ifdef LED_GPIO_NEOPIXEL
  if (isNeopixel) setPixelRgbColor(color);
#else
  if (!isNeopixel) digitalWrite(pin, LED_ON);
#endif
  ledState = LED_ON;
}

void AsyncLed::turnOff() {
  isBlinking = false;
#ifdef LED_GPIO_NEOPIXEL
  if (isNeopixel) setPixelRgbColor(RgbColor::Off);  // Turn off
#else
  if (!isNeopixel) digitalWrite(pin, LED_OFF);
#endif
  ledState = LED_OFF;
}

void AsyncLed::blink(uint16_t timeOn, uint16_t timeOff, RgbColor color, uint16_t count) {
  turnOn(color);

  isBlinking = true;
  blinkTimeOn = timeOn;
  blinkTimeOff = timeOff;
  lastBlinkTime = millis();
  targetBlinkCount = count;
  blinkCount = 0;
  ledRgbColor = color;

  update();
}

void AsyncLed::indicate(bool success) {
  if (success) {
    blink(200, 200, RgbColor::Success, 4);
  } else {
    blink(50, 50, RgbColor::Warning, 16);
  }
}

#ifdef LED_GPIO_NEOPIXEL
uint8_t AsyncLed::getColorComponent(RgbColor color, uint8_t component) {
  switch (component) {
    case 0:  // Red
      return (color >> 16) & 0xFF;
    case 1:  // Green
      return (color >> 8) & 0xFF;
    case 2:  // Blue
      return color & 0xFF;
    default:
      return 0;  // Invalid component
  }
}

void AsyncLed::setPixelRgbColor(RgbColor color) {
  rgbLedWrite(
      LED_GPIO_NEOPIXEL, getColorComponent(color, 0), getColorComponent(color, 1),
      getColorComponent(color, 2)
  );
}
#endif
