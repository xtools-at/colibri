#pragma once

#include "NimBLEInterface.h"

const uint32_t propRead =
    NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::READ_ENC | NIMBLE_PROPERTY::READ_AUTHEN;
const uint32_t propWrite =
    NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_ENC | NIMBLE_PROPERTY::WRITE_AUTHEN;

std::string requestBuffer;
bool requestReady = false;

#ifdef DISPLAY_ENABLED
uint32_t pairingKey = 0;
#else
uint32_t pairingKey = BLE_PAIRING_KEY;
#endif

// ========== BLE Callbacks ========== //
class BLEWriteCallback : public NimBLECharacteristicCallbacks {
  NimBLEUUID inputUuid = NimBLEUUID(BLE_CHARACTERISTIC_INPUT);

  void onWrite(NimBLECharacteristic *pCharacteristic) {
    if (pCharacteristic->getUUID() != inputUuid) {
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

    if (requestBuffer.substr(requestBuffer.size() - 1) == "}") {
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
    setStateConnected(true);

    // stop advertising
    pServer->stopAdvertising();
  }

  void onDisconnect(NimBLEServer *pServer) {
    if (pServer->getConnectedCount() > 0) {
      return;
    }

    wallet.lock();
    setStateConnected(false);

    // restart advertising
    pServer->startAdvertising();
  }

  uint32_t onPassKeyRequest() {
    if (pairingKey == 0) {
      pairingKey = randomNumber(111111, 999999);
    }

    return pairingKey;
  }

  bool onConfirmPIN(uint32_t pin) {
    // approve pairing via hardware button
    if (pairingKey == 0 || !waitForApproval(Connecting)) {
      return false;
    }

    return pin == pairingKey;
  }

  // TODO: remove after debugging if not needed
  /*
  void onAuthenticationComplete(ble_gap_conn_desc *desc) {
    // check if successfully authenticated
    if (desc->sec_state.encrypted && desc->sec_state.authenticated) {
      // wallet.unlock();
    } else {
      // wallet.lock();
    }

    // led indicator
  }
  */
};

// ========== NimBLE Interface Class ========== //

void NimBLEInterface::init() {
  if (initialised) return;
  initialised = true;

  // init
  NimBLEDevice::init(BLE_SERVER_NAME);
  NimBLEDevice::setMTU(512);
  NimBLEDevice::setSecurityAuth(true, true, true);
  NimBLEDevice::setSecurityIOCap(BLE_HS_IO_NO_INPUT_OUTPUT);
  NimBLEDevice::setPower(ESP_PWR_LVL_P6);

  pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new BLEServerCallbacks());
  pService = pServer->createService(BLE_SERVICE_UUID);

  // BLE service characteristics
  // output:
  pCharOutput =
      pService->createCharacteristic(BLE_CHARACTERISTIC_OUTPUT, propRead | NIMBLE_PROPERTY::NOTIFY);
  pCharOutput->setValue(BLE_DEFAULT_OUTPUT);

  // input:
  BLEWriteCallback *writeCallback = new BLEWriteCallback();
  pCharInput = pService->createCharacteristic(BLE_CHARACTERISTIC_INPUT, propRead | propWrite);
  pCharInput->setCallbacks(writeCallback);
  pCharInput->setValue(BLE_DEFAULT_INPUT);
  pCharInput->createDescriptor("2904");

  // start BLE advertising
  pService->start();
  pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->start();
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
    requestReady = false;
    pCharInput->setValue(BLE_DEFAULT_INPUT);

    sendResponse(output);
    output.clear();
  }
}

void NimBLEInterface::sendResponse(std::string &data) {
  if (pServer->getConnectedCount() == 0) {
    return;
  }

  size_t peerMtu = pServer->getPeerMTU(pServer->getPeerInfo(0).getConnHandle());
  size_t maxLen = peerMtu > 3 ? peerMtu - 3 : NimBLEDevice::getMTU() - 3;
  size_t dataLen = data.length();
  size_t offset = 0;

  while (offset < dataLen) {
    size_t chunkLen = std::min(maxLen, dataLen - offset);
    std::string chunk(data.substr(offset, chunkLen));

    pCharOutput->setValue(chunk);
    delay(10);
    pCharOutput->notify();
    delay(250);

    offset += chunkLen;
  }

  delay(50);
  pCharOutput->setValue(BLE_DEFAULT_OUTPUT);
}
