// SPDX-License-Identifier: AGPL-3.0-or-later

#pragma once

#include "OTA.h"

// WiFi credentials
const char* wifiSSID = "Colibri OTA";
const char* wifiPassword = "12345678";

// WebServer instance
WebServer server(80);

static void onOTAStart() { log_i("OTA update started..."); }

static void onOTAProgress(size_t current, size_t final) {
  log_d("OTA update progress: %d/%d", current, final);
}

static void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    // reboot
    log_i("OTA update finished successfully. Rebooting...");
  } else {
    log_e("There was an error during OTA update.");
  }
}

void OTA::init() {
  // configure Wifi access point
  IPAddress ip(192, 168, 1, 1);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.softAP(wifiSSID, wifiPassword);
  WiFi.softAPConfig(ip, gateway, subnet);
  log_i("OTA access point IP: %s", WiFi.softAPIP().toString().c_str());

  // configure WebServer
  server.on("/", []() { server.send(200, "text/plain", "Hi! This is ESP32."); });

  // configure OTA
  ElegantOTA.setAuth("1234", "1234");
  ElegantOTA.begin(&server);
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  // start server
  server.begin();
}

void OTA::stop() { WiFi.mode(WIFI_OFF); }

void OTA::update() {
  server.handleClient();
  ElegantOTA.loop();
}
