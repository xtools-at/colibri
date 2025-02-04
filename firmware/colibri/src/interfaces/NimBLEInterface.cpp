// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "NimBLEInterface.h"

#if defined(INTERFACE_BLE_NIMBLE)

const uint32_t propRead =
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN;
const uint32_t propWrite =
    NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN;
const uint16_t maxCharValueLen = 251 - 3;

std::string requestBuffer;
bool requestReady = false;

static uint16_t getMaxChunkLen(uint16_t peerMtu) {
  uint16_t maxLen = peerMtu > 20 ? peerMtu - 3 : 20;
  return maxLen > maxCharValueLen ? maxCharValueLen : maxLen;
}

// ========== BLE Callbacks ========== //
class BLEWriteCallback : public NimBLECharacteristicCallbacks {
  NimBLEUUID inputUuid = NimBLEUUID(BLE_CHARACTERISTIC_INPUT);

  void onWrite(NimBLECharacteristic *pCharacteristic) {
    if (requestReady || (pCharacteristic->getUUID() != inputUuid)) {
      return;
    }

    std::string value = pCharacteristic->getValue();

    if (value.empty()) {
      return;
    }

    if (value[0] == '{') {
      requestBuffer.clear();
    }

    requestBuffer += value;

    if (requestBuffer[requestBuffer.size() - 1] == '}') {
      requestReady = true;
    }
  }
};

class BLEServerCallback : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer *pServer, ble_gap_conn_desc *desc) {
    // only allow one connection
    if (pServer->getConnectedCount() > 1) {
      pServer->disconnect(desc->conn_handle);
      return;
    }

    // wallet should be locked after connection
    wallet.lock();

    // set led indicator
    setStateConnected(BleConnected);

    // stop advertising
    pServer->stopAdvertising();

    // request data length update from peer
    pServer->setDataLen(desc->conn_handle, 0x00FB);
  }

  void onDisconnect(NimBLEServer *pServer) {
    if (pServer->getConnectedCount() > 0) {
      return;
    }

    wallet.lock();
    setStateConnected(NotConnected);

    // restart advertising
    pServer->startAdvertising();
  }

  uint32_t onPassKeyRequest() {
  #ifdef DISPLAY_ENABLED
      // TODO: show passkey on display
  #endif

    return NimBLEDevice::getSecurityPasskey();
  }

  bool onConfirmPIN(uint32_t pin) {
    // approve pairing request on wallet
    if (!waitForApproval(Connecting)) {
      return false;
    }

    // check pin
    return (NimBLEDevice::getSecurityPasskey() == pin);
  }
};

// ========== NimBLE Interface Class ========== //

void NimBLEInterface::init() {
  if (initialised) return;
  initialised = true;

  // init
  NimBLEDevice::init(BLE_SERVER_NAME);
  NimBLEDevice::setSecurityAuth(true, true, true);
  NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_ONLY);
  NimBLEDevice::setPower(ESP_PWR_LVL_P9);
  NimBLEDevice::setMTU(512);

  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new BLEServerCallback());
  pService = pServer->createService(BLE_SERVICE_UUID);

  // BLE service characteristics
  // - input:
  BLEWriteCallback *writeCallback = new BLEWriteCallback();
  pCharInput = pService->createCharacteristic(BLE_CHARACTERISTIC_INPUT, propRead | propWrite);
  pCharInput->setCallbacks(writeCallback);
  pCharInput->setValue(std::string(BLE_INPUT_DEFAULT_MSG));

  // - output:
  pCharOutput =
      pService->createCharacteristic(BLE_CHARACTERISTIC_OUTPUT, propRead | NIMBLE_PROPERTY::NOTIFY);
  pCharOutput->setValue(std::string(BLE_OUTPUT_DEFAULT_MSG));

  // start BLE advertising
  pService->start();
  pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->start();

  // set/generate pairing key
  #ifndef DISPLAY_ENABLED
  delay(20);
  NimBLEDevice::setSecurityPasskey(BLE_PAIRING_KEY);
  #else
  NimBLEDevice::setSecurityPasskey(randomNumber(111111, 999999));
  #endif
}

void NimBLEInterface::stop() {
  disconnect();

  if (pAdvertising != NULL) {
    pAdvertising->stop();
  }
  if (pServer != NULL) {
    pServer->stopAdvertising();
    if (pService != NULL) {
      pServer->removeService(pService, true);
    }
  }

  NimBLEDevice::deinit(true);

  pAdvertising = NULL;
  pService = NULL;
  pServer = NULL;

  initialised = false;
}

void NimBLEInterface::disconnect() {
  if (!initialised) {
    return;
  }

  size_t peersNum = pServer->getConnectedCount();
  for (int i = 0; i < peersNum; i++) {
    uint16_t connID = pServer->getPeerInfo(i).getConnHandle();
    pServer->disconnect(connID);
  }
}

void NimBLEInterface::wipe() {
  disconnect();
  requestBuffer.clear();
  requestReady = false;
  NimBLEDevice::deleteAllBonds();
}

void NimBLEInterface::update() {
  if (requestReady) {
    std::string output;
    rpc.handleRequest(requestBuffer, output);
    requestBuffer.clear();

    sendResponse(output);
    output.clear();
  }
}

void NimBLEInterface::sendResponse(std::string &data) {
  if (pServer->getConnectedCount() == 0) {
    requestReady = false;
    return;
  }

  bool hasSubscribers = pCharOutput->getSubscribedCount() > 0;

  // prevent messages from getting truncated, figure out "sweet spot" for chunk size
  uint16_t peerMtu = pServer->getPeerMTU(pServer->getPeerInfo(0).getConnHandle());
  size_t maxLen = getMaxChunkLen(peerMtu);

  // send data in chunks
  size_t dataLen = data.length();
  size_t offset = 0;

  while (offset < dataLen) {
    size_t chunkLen = std::min(maxLen, dataLen - offset);
    std::string chunk(data.substr(offset, chunkLen));

    // set characteristic value
    pCharOutput->setValue(chunk);
    delay(5);

    // quit early if there are no subscribers
    if (!hasSubscribers) break;

    // notify subscribers
    pCharOutput->notify();
    delay(150);

    offset += chunkLen;
  }

  // don't reset characteristics if there are no subscribers (for manual review)
  if (hasSubscribers) {
    delay(50);

    pCharInput->setValue(std::string(BLE_INPUT_DEFAULT_MSG));
    pCharOutput->setValue(std::string(BLE_OUTPUT_DEFAULT_MSG));
  }

  requestReady = false;
}
#endif
