// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "AsyncButton.h"

AsyncButton::AsyncButton(
    uint8_t pin, uint64_t debounceTime, uint64_t longPressTime, uint64_t multiClickTimeout
)
    : _pin(pin),
      _debounceTime(debounceTime),
      _longPressTime(longPressTime),
      _multiClickTimeout(multiClickTimeout),
      _lastDebounceTime(0),
      _lastPressTime(0),
      _firstClickTime(0),
      _lastButtonState(!BUTTON_PUSHED),
      _buttonState(!BUTTON_PUSHED),
      _clickDetected(false),
      _longPressDetected(false),
      _clickCount(0) {
  pinMode(_pin, BUTTON_MODE);
}

void AsyncButton::update() {
  bool reading = digitalRead(_pin);
  if (reading != _lastButtonState) {
    _lastDebounceTime = millis();
  }

  if ((millis() - _lastDebounceTime) > _debounceTime) {
    if (reading != _buttonState) {
      _buttonState = reading;
      if (_buttonState == BUTTON_PUSHED) {
        _lastPressTime = millis();
        _clickDetected = true;
        _clickCount++;
        if (_clickCount == 1) {
          _firstClickTime = millis();
        }
      }
    }

    if (reading == BUTTON_PUSHED && _lastPressTime &&
        (millis() - _lastPressTime) > _longPressTime) {
      _longPressDetected = true;
    }
  }

  if (_firstClickTime && (_clickCount > 0 && (millis() - _firstClickTime) > _multiClickTimeout)) {
    _clickCount = 0;
    _firstClickTime = 0;
  }

  _lastButtonState = reading;
}

bool AsyncButton::isShortPressed() {
  if (_clickDetected) {
    _clickDetected = false;
    return true;
  }
  return false;
}

bool AsyncButton::isLongPressed() {
  if (_longPressDetected) {
    _longPressDetected = false;
    _lastPressTime = 0;
    return true;
  }
  return false;
}

bool AsyncButton::isMultiPressed(uint8_t times) {
  if (_clickCount >= times) {
    _clickCount = 0;
    _firstClickTime = 0;
    return true;
  }
  return false;
}

void AsyncButton::reset() {
  _clickCount = 0;
  _lastPressTime = 0;
  _firstClickTime = 0;
  _longPressDetected = false;
  _clickDetected = false;
}
