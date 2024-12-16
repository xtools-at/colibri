// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "SerialDebugInterface.h"

void SerialDebugInterface::init() {
  // Serial is initialised in main.cpp > setup()
  Serial.println("> Serial debug RPC interface initialized\n");
  initialised = true;
  connected = true;
  setStateConnected(connected);
}

void SerialDebugInterface::update() {
  if (!connected) return;

  if (Serial && Serial.available()) {
    bool doneReading = false;

    while (Serial.available()) {
      char c = Serial.read();
      if (c == '{') {
        // new payload begins
        inputBuffer.clear();
      }

      inputBuffer += c;

      if (c == '}') {
        // end of payload
        doneReading = true;
        break;
      }
    }

    if (doneReading) {
      // pass input to RPC
      std::string output;
      rpc.handleRequest(inputBuffer, output);
      inputBuffer.clear();

      // return RPC response in batches if longer than 512 characters
      const size_t batchSize = 512;
      size_t outputLength = output.length();
      size_t offset = 0;

      while (offset < outputLength) {
        size_t chunkSize =
            (outputLength - offset) > batchSize ? batchSize : (outputLength - offset);
        Serial.write(output.c_str() + offset, chunkSize);
        Serial.flush();
        offset += chunkSize;
      }
      Serial.print('\n');

      output.clear();
    }
  }
}

void SerialDebugInterface::stop() {
  disconnect();
  initialised = false;
}

void SerialDebugInterface::disconnect() {
  connected = false;
  setStateConnected(connected);
}

void SerialDebugInterface::wipe() {
  stop();
  inputBuffer.clear();
}
