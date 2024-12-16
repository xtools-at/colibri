// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"

class AsyncButton {
 public:
  AsyncButton(
      uint8_t pin, uint64_t debounceTime = 20, uint64_t longPressTime = 600,
      uint64_t multiClickTimeout = 2500
  );

  void update();
  bool isShortPressed();
  bool isLongPressed();
  bool isMultiPressed(uint8_t count);
  void reset();

 private:
  bool _lastButtonState;
  bool _buttonState;
  bool _clickDetected;
  bool _longPressDetected;
  uint8_t _pin;
  uint32_t _clickCount;
  uint64_t _debounceTime;
  uint64_t _longPressTime;
  uint64_t _multiClickTimeout;
  uint64_t _lastDebounceTime;
  uint64_t _lastPressTime;
  uint64_t _firstClickTime;
};
