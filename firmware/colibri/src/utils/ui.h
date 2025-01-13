// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"
#include "../ui/AsyncButton.h"
#include "../ui/AsyncLed.h"
#include "esp_system.h"
#ifdef DISPLAY_ENABLED
// #include "Display.h"
#endif

extern AsyncButton buttonOk;
#ifdef BUTTON_LAYOUT_MAIN_TWO
extern AsyncButton buttonCancel;
#else
  #define buttonCancel buttonOk
#endif
#ifdef LED_ENABLED
extern AsyncLed led;
#endif
extern bool isHot;
extern bool isBusy;
extern Connection connection;

void updateUi();

bool waitForApproval(RgbColor color = Hot);

void setStateConnected(Connection conn);

void setStateBusy(bool busy);
