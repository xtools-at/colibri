#pragma once

#include "../../config.h"
#include "../ui/AsyncButton.h"
#include "../ui/AsyncLed.h"
#include "esp_system.h"
#ifdef DISPLAY_ENABLED
// #include "Display.h"
#endif

extern AsyncButton buttonOk;
#ifdef BUTTON_LAYOUT_TWO
extern AsyncButton buttonCancel;
#else
  #define buttonCancel buttonOk
#endif
extern AsyncLed led;
extern bool isHot;
extern bool isBusy;
extern bool isConnected;

void updateLed();

void updateUi();

bool waitForApproval(RgbColor color = Hot);

void setStateConnected(bool connected);

void setStateBusy(bool busy);

void checkForGesture();
