// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "ArduinoBLEInterface.h"

#if defined(INTERFACE_BLE_ARDUINO)

String requestBuffer;
bool reqReady = false;
  #ifndef DISPLAY_ENABLED
const uint32_t securityPasskey = BLE_PAIRING_KEY;
  #else
uint32_t securityPasskey = 0;
  #endif
const size_t maxCharValueLen = 251 - 3;

// ========== BLE Callbacks ========== //
class BLESecurityCallback : public BLESecurityCallbacks {
  uint32_t onPassKeyRequest() override {
  #ifdef DISPLAY_ENABLED
      // TODO: show passkey on display
  #endif

    return securityPasskey;
  }

  bool onConfirmPIN(uint32_t pin) override {
    // approve pairing request on wallet
    if (!waitForApproval(Connecting)) {
      return false;
    }

    // check pin
    return securityPasskey == pin;
  }

  bool onSecurityRequest() override { return true; }

  void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl) override {}

  void onPassKeyNotify(uint32_t pass_key) override {}
};

class BLEWriteCallback : public BLECharacteristicCallbacks {
  BLEUUID inputUuid = BLEUUID(BLE_CHARACTERISTIC_INPUT);

  void onWrite(BLECharacteristic *pCharacteristic) override {
    if (reqReady || !pCharacteristic->getUUID().equals(inputUuid)) {
      return;
    }

    String value = pCharacteristic->getValue();

    if (value.length() == 0) {
      return;
    }

    if (value[0] == '{') {
      requestBuffer.clear();
    }

    requestBuffer += value;

    if (requestBuffer[requestBuffer.length() - 1] == '}') {
      reqReady = true;
    }
  }
};

class BLEServerCallback : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) override {
    // only allow one connection
    if (pServer->getConnectedCount() > 1) {
      pServer->disconnect(pServer->getConnId());
      return;
    }

    // wallet should be locked after connection
    wallet.lock();

    // set led indicator
    setStateConnected(BleConnected);

    // stop advertising
    pServer->getAdvertising()->stop();

    // request mtu update from peer
    delay(20);
    pServer->updatePeerMTU(pServer->getConnId(), 512);
  }

  void onDisconnect(BLEServer *pServer) {
    if (pServer->getConnectedCount() > 0) {
      return;
    }

    wallet.lock();
    setStateConnected(NotConnected);

    // restart advertising
    pServer->startAdvertising();
  }
};

void ArduinoBLEInterface::init() {
  if (initialised) return;
  initialised = true;

  // init
  BLEDevice::init(BLE_SERVER_NAME);
  BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT_NO_MITM);
  BLEDevice::setSecurityCallbacks(new BLESecurityCallback());
  BLEDevice::setPower(ESP_PWR_LVL_P9);
  BLEDevice::setMTU(512);

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BLEServerCallback());
  BLEService *pService = pServer->createService(BLE_SERVICE_UUID);

  // BLE service characteristics
  // - input:
  BLEWriteCallback *writeCallback = new BLEWriteCallback();
  pCharInput =
      pService->createCharacteristic(BLE_CHARACTERISTIC_INPUT, BLECharacteristic::PROPERTY_WRITE);
  pCharInput->setCallbacks(writeCallback);
  pCharInput->setValue(String(BLE_INPUT_DEFAULT_MSG));

  // - output:
  pCharOutput =
      pService->createCharacteristic(BLE_CHARACTERISTIC_INPUT, BLECharacteristic::PROPERTY_NOTIFY);
  pCharOutput->setValue(String(BLE_OUTPUT_DEFAULT_MSG));

  // start BLE advertising
  pService->start();
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->start();

  // set/generate pairing key
  #ifdef DISPLAY_ENABLED
  delay(20);
  securityPasskey = randomNumber(111111, 999999);
  #endif
}

void ArduinoBLEInterface::stop() {
  disconnect();

  if (pAdvertising != NULL) {
    pAdvertising->stop();
  }
  if (pServer != NULL) {
    pAdvertising->stop();
    if (pService != NULL) {
      pServer->removeService(pService);
    }
  }

  BLEDevice::deinit(true);

  pAdvertising = NULL;
  pService = NULL;
  pServer = NULL;

  initialised = false;
}

void ArduinoBLEInterface::disconnect() {
  if (!initialised) {
    return;
  }

  pServer->disconnect(pServer->getConnId());
}

void ArduinoBLEInterface::wipe() {
  pServer->removePeerDevice(pServer->getConnId(), true);

  disconnect();
  requestBuffer.clear();
  reqReady = false;
}

void ArduinoBLEInterface::update() {
  if (reqReady) {
    std::string output;
    rpc.handleRequest(std::string(requestBuffer.c_str()), output);
    requestBuffer.clear();

    String outputS = String(output.c_str());

    sendResponse(outputS);
    output.clear();
  }
}

void ArduinoBLEInterface::sendResponse(String &data) {
  if (pServer->getConnectedCount() == 0) {
    reqReady = false;
    return;
  }

  // prevent messages from getting truncated, figure out "sweet spot" for chunk size
  uint16_t peerMtu = pServer->getPeerMTU(pServer->getConnId());
  size_t maxLen = peerMtu > 3 ? peerMtu - 3 : 20;
  if (maxLen > maxCharValueLen) maxLen = maxCharValueLen;

  // send data in chunks
  size_t dataLen = data.length();
  size_t offset = 0;

  while (offset < dataLen) {
    size_t chunkLen = std::min(maxLen, dataLen - offset);
    String chunk(data.substring(offset, chunkLen));

    // set characteristic value
    pCharOutput->setValue(chunk);
    delay(5);

    // notify subscribers
    pCharOutput->notify();
    delay(150);

    offset += chunkLen;
  }

  reqReady = false;
}
#endif
