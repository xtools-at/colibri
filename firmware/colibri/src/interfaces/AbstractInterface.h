#pragma once

#include "../../config.h"
#include "../core/RPC.h"
#include "../core/Wallet.h"
#include "../utils/ui.h"

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
