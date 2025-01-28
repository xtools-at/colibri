// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "ota.h"

#include "../../interfaces/interfaces.h"

#ifdef OTA_ENABLED

const IPAddress ip(192, 168, 1, 1);
const IPAddress gateway = ip;
const IPAddress subnet(255, 255, 255, 0);
const char *otaUrl = "http://192.168.1.1/";

WebServer server(80);

static void onOTAStart() { log_i("OTA update started..."); }

static void onOTAProgress(size_t current, size_t final) {
  log_d("OTA update progress: %d/%d", current, final);
}

static void onOTAEnd(bool success) {
  if (success) {
    // reboot
    log_i("OTA update finished successfully. Rebooting...");
    delay(5000);
    esp_restart();
  } else {
    log_e("There was an error during OTA update.");
  }
}

static void redirectTo(const char *url) {
  server.sendHeader("Location", url, true);
  server.send(302, "text/plain", "");
}

void initOta() {
  // stop other interfaces
  stopInterfaces();

  // configure Wifi access point
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(OTA_WIFI_SSID, OTA_WIFI_PASSWORD, OTA_WIFI_CHANNEL, 0, OTA_MAX_CLIENTS);
  log_i("OTA access point SSID: %s", OTA_WIFI_SSID);
  log_s("OTA access point password: %s", OTA_WIFI_PASSWORD);
  log_i("OTA access point IP: %s", WiFi.softAPIP().toString().c_str());

  // mDNS
  MDNS.begin(OTA_DNS_NAME);
  log_i("OTA access point DNS address: http://%s.local", OTA_DNS_NAME);

  // configure server
  // - 404s
  const char *notFoundPaths[] = {
      "/favicon.ico",
      "/wpad.dat",
      "/chat",
  };
  for (const char *path : notFoundPaths) {
    server.on(path, HTTP_GET, []() { server.send(404); });
  }
  // - catch unknown URLs
  server.onNotFound([]() { redirectTo(otaUrl); });

  // configure OTA
  ElegantOTA.begin(&server, "/");
  // ElegantOTA.setAuth("user", "pass");
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  // start server
  server.begin();
}

void stopOta() {
  WiFi.mode(WIFI_OFF);
  server.stop();
  initInterfaces();
}

void updateOta() { server.handleClient(); }

#endif
