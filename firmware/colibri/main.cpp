// SPDX-License-Identifier: AGPL-3.0-or-later
/*
 * Colibri Wallet - Libre DIY Hardware Wallet <https://colibri.diy>
 * Copyright (C) 2024-2025  xtools-at <https://github.com/xtools-at>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"
#include "src/core/RPC.h"
#include "src/core/Storage.h"
#include "src/core/Wallet.h"
#include "src/interfaces/interfaces.h"
#include "src/ui/AsyncButton.h"
#include "src/ui/AsyncLed.h"
#include "src/ui/ui.h"

// core
Storage store = Storage();
Wallet wallet = Wallet();
JsonRpcHandler rpc = JsonRpcHandler();

// ui
AsyncButton buttonOk = AsyncButton(BUTTON_GPIO_OK);
#ifdef BUTTON_LAYOUT_MAIN_TWO
AsyncButton buttonCancel = AsyncButton(BUTTON_GPIO_CANCEL);
#endif

#ifdef LED_ENABLED
  #ifdef LED_GPIO_NEOPIXEL
AsyncLed led = AsyncLed(LED_GPIO_NEOPIXEL, true);
  #else
AsyncLed led = AsyncLed(LED_GPIO);
  #endif
#endif

// state tracking
bool isHot = false;
bool isBusy = false;
Connection connection = NotConnected;

void setup() {
  led.turnOn(RgbColor::Busy);

  // enable Serial when debugging
#if (CORE_DEBUG_LEVEL > 0)
  Serial.begin(BOARD_SERIAL_BAUD_RATE);
  while (!Serial) {
    delay(20);
  }
  Serial.setDebugOutput(true);
  log_println("\n=== WARNING: SERIAL DEBUG ENABLED ===\n");
  log_printf("> Serial debug log level: %d\n", CORE_DEBUG_LEVEL);
#endif

  // display setup and boot screen
  // TODO

  rpc.init();
  delay(50);

  initInterfaces();
  delay(100);

  // show version screen
  // TODO
}

void loop() {
  updateUi();
  updateInterfaces();

  // debounce
  delay(5);
}
