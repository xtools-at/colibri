/*
 * Adapted code from ElegantOTA (licensed under AGPL 3.0, see `./LICENSE`):
 * https://github.com/ayushsharma82/ElegantOTA
 */
#pragma once

#include "../../../../config.h"

#ifdef OTA_ENABLED
  #include "stdlib_noniso.h"

  #ifndef ELEGANTOTA_USE_ASYNC_WEBSERVER
    #define ELEGANTOTA_USE_ASYNC_WEBSERVER 0
  #endif

  #if CORE_DEBUG_LEVEL >= 4
    #define ELEGANTOTA_DEBUG_MSG(x) Serial.printf("%s %s", "[ElegantOTA] ", x)

    #ifndef UPDATE_DEBUG
      #define UPDATE_DEBUG 1
    #endif
  #else
    #define ELEGANTOTA_DEBUG_MSG(x)
  #endif

  // ESP32 only
  #include <functional>

  #include "FS.h"
  #include "StreamString.h"
  #include "Update.h"
  #if ELEGANTOTA_USE_ASYNC_WEBSERVER == 1
    #include "AsyncTCP.h"
    #include "ESPAsyncWebServer.h"
    #define ELEGANTOTA_WEBSERVER AsyncWebServer
  #else
    #include "WebServer.h"
    #include "WiFi.h"
    // #include "WiFiClient.h"
    #define ELEGANTOTA_WEBSERVER WebServer
  #endif
  #define HARDWARE "ESP32"

enum OTA_Mode { OTA_MODE_FIRMWARE = 0, OTA_MODE_FILESYSTEM = 1 };

class ElegantOTAClass {
 public:
  ElegantOTAClass();

  void begin(
      ELEGANTOTA_WEBSERVER *server, const char *updatePath = "/update", const char *username = "",
      const char *password = ""
  );

  void setAuth(const char *username, const char *password);
  void clearAuth();
  void onStart(std::function<void()> callable);
  void onProgress(std::function<void(size_t current, size_t final)> callable);
  void onEnd(std::function<void(bool success)> callable);

 private:
  ELEGANTOTA_WEBSERVER *_server;

  bool _authenticate;
  String _username;
  String _password;

  bool _auto_reboot = true;
  bool _reboot = false;
  unsigned long _reboot_request_millis = 0;

  String _update_error_str = "";
  unsigned long _current_progress_size;

  std::function<void()> preUpdateCallback = NULL;
  std::function<void(size_t current, size_t final)> progressUpdateCallback = NULL;
  std::function<void(bool success)> postUpdateCallback = NULL;
};

extern ElegantOTAClass ElegantOTA;
#endif
