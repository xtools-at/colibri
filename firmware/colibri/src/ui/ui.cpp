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
  uint64_t now = millis();
  uint64_t delta = now > wallet.timeLastActivity ? now - wallet.timeLastActivity : 0;

  if (wallet.timeLastActivity > 0 && delta > TIMEOUT_INACTIVITY_LOCK) {
    log_i(
        "Auto-locking device after inactivity of %d ms (limit=%d)", delta, TIMEOUT_INACTIVITY_LOCK
    );
    log_d("- Now:   %d", now);
    log_d("- Last:  %d", wallet.timeLastActivity);

    wallet.lock();
  }
}

static void updateButtons() {
  // update buttons
  buttonOk.update();
#ifdef BUTTON_LAYOUT_MAIN_TWO
  buttonCancel.update();
#endif
}

static void updateLed() {
  if (!isHot && !led_isBlinking) {
    if (isBusy) {
      led_turnOn(Busy);
    } else if (connection > NotConnected) {
      led_turnOn(Idle);
    } else {
      led_turnOn(Connecting);
    }
  }

  led_update();
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
  if (!wallet.isLocked() && wallet.isKeySet()) {
    wallet.timeLastActivity = millis();
  }

  // set led hot
  isHot = true;
  led_blink(100, 100, color);
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
  led_indicate(isApproved);
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
