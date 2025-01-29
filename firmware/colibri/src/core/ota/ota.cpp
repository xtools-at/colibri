// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "ota.h"

#include "../../interfaces/interfaces.h"
#include "./portalHtml.h"

#ifdef OTA_ENABLED

const IPAddress ip(192, 168, 1, 1);
const IPAddress gateway = ip;
const IPAddress subnet(255, 255, 255, 0);
const char *otaUrl = "http://192.168.1.1/";

WebServer server(80);
const uint8_t DNS_PORT = 53;
DNSServer dns;

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
  log_d("Redirecting to %s from %s", url, server.uri().c_str());

  server.sendHeader("Location", url, true);
  server.send(302, "text/plain", "");
}

static void redirectToPortal() { redirectTo("/portal"); }

  #ifdef OTA_USE_LITTLEFS
static bool handleFileRead(const String &path) {
  String filePath = path;
  if (filePath.endsWith("/")) {
    filePath += "index.html";  // Default file
  }
  String contentType = "text/plain";
  if (filePath.endsWith(".html"))
    contentType = "text/html";
  else if (filePath.endsWith(".css"))
    contentType = "text/css";
  else if (filePath.endsWith(".js"))
    contentType = "application/javascript";
  else if (filePath.endsWith(".png"))
    contentType = "image/png";
  else if (filePath.endsWith(".jpg"))
    contentType = "image/jpeg";
  else if (filePath.endsWith(".svg"))
    contentType = "image/svg+xml";
  else if (filePath.endsWith(".ico"))
    contentType = "image/x-icon";
  else if (filePath.endsWith(".gz"))
    contentType = "application/x-gzip";

  log_d("LittleFS: Checking for file: %s", filePath.c_str());

  if (LittleFS.exists(filePath)) {
    File file = LittleFS.open(filePath, "r");
    server.streamFile(file, contentType);
    file.close();

    log_d("LittleFS: File found, serving: %s", filePath.c_str());
    return true;
  }

  log_d("LittleFS: File not found: %s", filePath.c_str());

  return false;
}
  #else
static bool handleFileRead(const String &path) { return false; }
  #endif

void initOta() {
  // stop other interfaces
  stopInterfaces();

  #ifdef OTA_USE_LITTLEFS
  // init LittleFS
  LittleFS.begin();
  #endif

  // configure Wifi access point
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.softAP(OTA_WIFI_SSID, OTA_WIFI_PASSWORD, OTA_WIFI_CHANNEL, 0, OTA_MAX_CLIENTS);
  log_i("OTA access point SSID: %s", OTA_WIFI_SSID);
  log_s("OTA access point password: %s", OTA_WIFI_PASSWORD);
  log_i("OTA access point IP: %s", WiFi.softAPIP().toString().c_str());

  // DNS & mDNS setup
  MDNS.begin(OTA_WIFI_HOSTNAME);
  log_i("OTA access point DNS address: http://%s.local", OTA_WIFI_HOSTNAME);
  dns.setErrorReplyCode(DNSReplyCode::NoError);
  dns.start(DNS_PORT, "*", ip);

  // configure server
  // - 404s
  const char *notFoundPaths[] = {
      "/favicon.ico",
      "/wpad.dat",
      "/chat",
  };
  for (const char *path : notFoundPaths) {
    server.on(path, HTTP_GET, []() {
      log_d("Returning 404 for %s", server.uri().c_str());
      server.send(404);
    });
  }
  server.onNotFound([]() {
    if (!handleFileRead(server.uri())) {
      redirectToPortal();
    }
  });
  // - captive portal
  const char *portalPaths[] = {
      "/generate_204",  // android
      "/redirect",  // microsoft
      "/fwlink",  // microsoft
      "/ncsi.txt",  // windows
      "/hotspot-detect.html",  // apple
      "/canonical.html",  // firefox
  };
  for (const char *path : portalPaths) {
    server.on(path, redirectToPortal);
  }
  server.on("/success.txt", []() { server.send(200); });  // firefox

  #ifdef OTA_USE_LITTLEFS
  // serve portal page from LittleFS
  server.on("/portal", []() { handleFileRead("/portal/index.html"); });
  #else
  // serve portal page from PROGMEM
  server.on("/portal", []() { server.send_P(200, "text/html", OTA_PORTAL_HTML); });
  #endif

  // configure OTA
  ElegantOTA.begin(&server, "/");
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  // start server
  server.begin();
}

void stopOta() {
  WiFi.mode(WIFI_OFF);
  server.stop();
  MDNS.end();
  dns.stop();

  #ifdef OTA_USE_LITTLEFS
  LittleFS.end();
  #endif

  initInterfaces();
}

void updateOta() { server.handleClient(); }

#endif
