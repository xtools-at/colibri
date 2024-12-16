// SPDX-License-Identifier: AGPL-3.0-or-later

#include "config.h"
#include "src/chains/Ethereum.h"
#include "src/core/RPC.h"
#include "src/core/Storage.h"
#include "src/core/Wallet.h"
#include "src/ui/AsyncButton.h"
#include "src/ui/AsyncLed.h"
#include "src/utils/interfaces.h"
#include "src/utils/ui.h"

// core
Storage store = Storage();
Wallet wallet = Wallet();
JsonRpcHandler rpc = JsonRpcHandler();

// chains
EthereumChain ethereum = EthereumChain();

// ui
AsyncButton buttonOk = AsyncButton(BUTTON_GPIO_OK);
#ifdef BUTTON_LAYOUT_TWO
AsyncButton buttonCancel = AsyncButton(BUTTON_GPIO_CANCEL);
#endif

#ifdef LED_GPIO_NEOPIXEL
AsyncLed led = AsyncLed(LED_GPIO_NEOPIXEL, true);
#else
AsyncLed led = AsyncLed(LED_GPIO);
#endif

// states
bool isHot = false;
bool isBusy = false;
bool isConnected = false;

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