// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../../config.h"
#include "../../../constants.h"

#ifdef DISPLAY_ENABLED
  #include <qrcode.h>

  #include "PolymorphicLGFX.h"

void initDisplay();
void drawText(const char* message);
void drawQrCode(esp_qrcode_handle_t qrcode);

#else

  #define initDisplay()
  #define drawText(...)
  #define drawQrCode(...)

#endif
