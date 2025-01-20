// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "ui.h"

#include "../core/Wallet.h"
extern Wallet wallet;

static void checkForGesture() {
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

static void checkForTimeout() {
  // TODO: fix
  /*
  if (wallet.timeLastActivity > 0 &&
      (millis() - wallet.timeLastActivity) > TIMEOUT_INACTIVITY_LOCK) {
    log_i("Auto-locking device after inactivity");
    wallet.lock();
  }
  */
}

static void updateButtons() {
  // update buttons
  buttonOk.update();
#ifdef BUTTON_LAYOUT_MAIN_TWO
  buttonCancel.update();
#endif
}

static void updateLed() {
  if (!isHot && !led.isBlinking) {
    if (isBusy) {
      led.turnOn(Busy);
    } else if (connection > NotConnected) {
      led.turnOn(Idle);
    } else {
      led.turnOn(Connecting);
    }
  }

  led.update();
}

void updateUi() {
  updateButtons();
  updateLed();
  checkForGesture();
  checkForTimeout();
}

bool waitForApproval(RgbColor color) {
  log_i("Waiting for user approval...");
  // reset auto-lock timeout
  if (!wallet.isLocked()) wallet.timeLastActivity = millis();

  // set led hot
  isHot = true;
  led.blink(100, 100, color);
  updateUi();

  // reset buttons
  buttonOk.reset();
#ifdef BUTTON_LAYOUT_MAIN_TWO
  buttonCancel.reset();
#endif

  // loop for n sec, check for presses
  bool isApproved = false;
  uint64_t startTime = millis();
  while ((millis() - startTime) < TIMEOUT_WAIT_FOR_APPROVAL) {
    updateUi();

#ifdef BUTTON_LAYOUT_MAIN_TWO
    if (buttonCancel.isShortPressed()) {
      isApproved = false;
      break;
    }
    if (buttonOk.isShortPressed()) {
      isApproved = true;
      break;
    }
#else  // BUTTON_LAYOUT_MAIN_ONE
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

void setStateConnected(Connection conn) {
  log_d("Interface connected: %s (%d)", conn > 0 ? "true" : "false", conn);
  connection = conn;
  updateUi();
}

void setStateBusy(bool busy) {
  isBusy = busy;
  updateUi();
}
