// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../../config.h"

#ifdef DISPLAY_ENABLED
  #include "PolymorphicLGFX.h"

void initDisplay();

#else

  #define initDisplay()

#endif
