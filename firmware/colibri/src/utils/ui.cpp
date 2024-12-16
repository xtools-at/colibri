// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "ui.h"

#include "../core/Wallet.h"
extern Wallet wallet;

void updateLed() {
  if (!isHot && !led.isBlinking) {
    if (isBusy) {
      led.turnOn(Busy);
    } else if (isConnected) {
      led.turnOn(Idle);
    } else {
      led.turnOn(Connecting);
    }
  }

  led.update();
}

void updateUi() {
  // update buttons
  buttonOk.update();
#ifdef BUTTON_LAYOUT_TWO
  buttonCancel.update();
#endif

  updateLed();
  checkForGesture();
}

bool waitForApproval(RgbColor color) {
  log_i("Waiting for user approval...");
  // set led hot
  isHot = true;
  led.blink(100, 100, color);
  updateUi();

  // reset buttons
  buttonOk.reset();
#ifdef BUTTON_LAYOUT_TWO
  buttonCancel.reset();
#endif

  // loop for n sec, check for presses
  bool isApproved = false;
  uint64_t startTime = millis();
  while ((millis() - startTime) < TIMEOUT_WAIT_FOR_APPROVAL) {
    updateUi();

#ifdef BUTTON_LAYOUT_TWO
    if (buttonCancel.isShortPressed()) {
      isApproved = false;
      break;
    }
    if (buttonOk.isShortPressed()) {
      isApproved = true;
      break;
    }
#else  // BUTTON_LAYOUT_ONE
    if (buttonOk.isLongPressed()) {
      isApproved = false;
      break;
    }
    if (buttonOk.isMultiPressed(2)) {
      isApproved = true;
      break;
    }
#endif

    delay(5);
  }

  // blink led, reset hot state
  led.indicate(isApproved);
  isHot = false;

  buttonOk.reset();
  buttonCancel.reset();
  updateUi();

  return isApproved;
}

void setStateConnected(bool connected) {
  log_d("Interface connected: %s", connected ? "true" : "false");
  isConnected = connected;
  updateUi();
}

void setStateBusy(bool busy) {
  isBusy = busy;
  updateUi();
}

void checkForGesture() {
  if (isHot) return;

  // check for lock/reboot request
  if (buttonCancel.isLongPressed()) {
    bool isLocked = wallet.isLocked();

    if (isLocked) {
      log_i("Rebooting device\n");
      esp_restart();
    } else {
      log_i("Locking device\n");
      wallet.lock();
    }
  }

  // check for wipe request
  if (buttonCancel.isMultiPressed(10)) {
    log_i("Device wipe initiated\n");
    if (!waitForApproval(RgbColor::Warning)) {
      return;
    }

    wallet.wipe();
  }
}
