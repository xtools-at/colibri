// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../config.h"
#include "../core/RPC.h"
#include "../core/Wallet.h"
#include "../ui/ui.h"

extern JsonRpcHandler rpc;
extern Wallet wallet;

// ========== Abstract Interface Class ========== //
class Interface {
 public:
  Interface() : initialised(false) {}

  virtual void init() = 0;
  virtual void update() = 0;
  virtual void stop() = 0;
  virtual void wipe() = 0;

  bool initialised;
};
