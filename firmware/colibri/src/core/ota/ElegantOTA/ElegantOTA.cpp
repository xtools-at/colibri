/*
 * Adapted code from ElegantOTA (licensed under AGPL 3.0, see `./LICENSE`):
 * https://github.com/ayushsharma82/ElegantOTA
 */
#pragma once

#include "ElegantOTA.h"

#ifdef OTA_ENABLED

  #include "./otaHtml.h"

ElegantOTAClass::ElegantOTAClass() {}

void ElegantOTAClass::begin(
    ELEGANTOTA_WEBSERVER *server, const char *updatePath, const char *username, const char *password
) {
  _server = server;

  setAuth(username, password);

  #if ELEGANTOTA_USE_ASYNC_WEBSERVER == 1
  _server->on(updatePath, HTTP_GET, [&](AsyncWebServerRequest *request) {
    if (_authenticate && !request->authenticate(_username.c_str(), _password.c_str())) {
      return request->requestAuthentication();
    }
    #if defined(ASYNCWEBSERVER_VERSION) && \
        ASYNCWEBSERVER_VERSION_MAJOR > \
            2  // This means we are using recommended fork of AsyncWebServer
    AsyncWebServerResponse *response =
        request->beginResponse(200, "text/html", OTA_PAGE_HTML, sizeof(OTA_PAGE_HTML));
    #else
        AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", OTA_PAGE_HTML, sizeof(OTA_PAGE_HTML));
    #endif
    // response->addHeader("Content-Encoding", "gzip");
    request->send(response);
  });
  #else
    #ifndef OTA_USE_SPIFFS
  _server->on(updatePath, HTTP_GET, [&]() {
    if (_authenticate && !_server->authenticate(_username.c_str(), _password.c_str())) {
      return _server->requestAuthentication();
    }
    // _server->sendHeader("Content-Encoding", "gzip");
    _server->send_P(200, "text/html", OTA_PAGE_HTML);
  });
    #endif
  #endif

  #if ELEGANTOTA_USE_ASYNC_WEBSERVER == 1
  _server->on("/ota/start", HTTP_GET, [&](AsyncWebServerRequest *request) {
    if (_authenticate && !request->authenticate(_username.c_str(), _password.c_str())) {
      return request->requestAuthentication();
    }

    // Get header x-ota-mode value, if present
    OTA_Mode mode = OTA_MODE_FIRMWARE;
    // Get mode from arg
    if (request->hasParam("mode")) {
      String argValue = request->getParam("mode")->value();
      if (argValue == "fs") {
        ELEGANTOTA_DEBUG_MSG("OTA Mode: Filesystem\n");
        mode = OTA_MODE_FILESYSTEM;
      } else {
        ELEGANTOTA_DEBUG_MSG("OTA Mode: Firmware\n");
        mode = OTA_MODE_FIRMWARE;
      }
    }

    // Get file MD5 hash from arg
    if (request->hasParam("hash")) {
      String hash = request->getParam("hash")->value();
      ELEGANTOTA_DEBUG_MSG(String("MD5: " + hash + "\n").c_str());
      if (!Update.setMD5(hash.c_str())) {
        ELEGANTOTA_DEBUG_MSG("ERROR: MD5 hash not valid\n");
        return request->send(400, "text/plain", "MD5 parameter invalid");
      }
    }

    #if UPDATE_DEBUG == 1
    // Serial output must be active to see the callback serial prints
    Serial.setDebugOutput(true);
    #endif

    // Pre-OTA update callback
    if (preUpdateCallback != NULL) preUpdateCallback();

    // Start update process
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, mode == OTA_MODE_FILESYSTEM ? U_SPIFFS : U_FLASH)) {
      ELEGANTOTA_DEBUG_MSG("Failed to start update process\n");
      // Save error to string
      StreamString str;
      Update.printError(str);
      _update_error_str = str.c_str();
      _update_error_str.concat("\n");
      ELEGANTOTA_DEBUG_MSG(_update_error_str.c_str());
    }

    return request->send(
        (Update.hasError()) ? 400 : 200, "text/plain",
        (Update.hasError()) ? _update_error_str.c_str() : "OK"
    );
  });
  #else
  _server->on("/ota/start", HTTP_GET, [&]() {
    if (_authenticate && !_server->authenticate(_username.c_str(), _password.c_str())) {
      return _server->requestAuthentication();
    }

    // Get header x-ota-mode value, if present
    OTA_Mode mode = OTA_MODE_FIRMWARE;
    // Get mode from arg
    if (_server->hasArg("mode")) {
      String argValue = _server->arg("mode");
      if (argValue == "fs") {
        ELEGANTOTA_DEBUG_MSG("OTA Mode: Filesystem\n");
        mode = OTA_MODE_FILESYSTEM;
      } else {
        ELEGANTOTA_DEBUG_MSG("OTA Mode: Firmware\n");
        mode = OTA_MODE_FIRMWARE;
      }
    }

    // Get file MD5 hash from arg
    if (_server->hasArg("hash")) {
      String hash = _server->arg("hash");
      ELEGANTOTA_DEBUG_MSG(String("MD5: " + hash + "\n").c_str());
      if (!Update.setMD5(hash.c_str())) {
        ELEGANTOTA_DEBUG_MSG("ERROR: MD5 hash not valid\n");
        return _server->send(400, "text/plain", "MD5 parameter invalid");
      }
    }

    #if UPDATE_DEBUG == 1
    // Serial output must be active to see the callback serial prints
    Serial.setDebugOutput(true);
    #endif

    // Pre-OTA update callback
    if (preUpdateCallback != NULL) preUpdateCallback();

    // Start update process
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, mode == OTA_MODE_FILESYSTEM ? U_SPIFFS : U_FLASH)) {
      ELEGANTOTA_DEBUG_MSG("Failed to start update process\n");
      // Save error to string
      StreamString str;
      Update.printError(str);
      _update_error_str = str.c_str();
      _update_error_str.concat("\n");
      ELEGANTOTA_DEBUG_MSG(_update_error_str.c_str());
    }

    return _server->send(
        (Update.hasError()) ? 400 : 200, "text/plain",
        (Update.hasError()) ? _update_error_str.c_str() : "OK"
    );
  });
  #endif

  #if ELEGANTOTA_USE_ASYNC_WEBSERVER == 1
  _server->on(
      "/ota/upload", HTTP_POST,
      [&](AsyncWebServerRequest *request) {
        if (_authenticate && !request->authenticate(_username.c_str(), _password.c_str())) {
          return request->requestAuthentication();
        }
        // Post-OTA update callback
        if (postUpdateCallback != NULL) postUpdateCallback(!Update.hasError());
        AsyncWebServerResponse *response = request->beginResponse(
            (Update.hasError()) ? 400 : 200, "text/plain",
            (Update.hasError()) ? _update_error_str.c_str() : "OK"
        );
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
        // Set reboot flag
        if (!Update.hasError()) {
          if (_auto_reboot) {
            _reboot_request_millis = millis();
            _reboot = true;
          }
        }
      },
      [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len,
          bool final) {
        // Upload handler chunks in data
        if (_authenticate) {
          if (!request->authenticate(_username.c_str(), _password.c_str())) {
            return request->requestAuthentication();
          }
        }

        if (!index) {
          // Reset progress size on first frame
          _current_progress_size = 0;
        }

        // Write chunked data to the free sketch space
        if (len) {
          if (Update.write(data, len) != len) {
            return request->send(400, "text/plain", "Failed to write chunked data to free space");
          }
          _current_progress_size += len;
          // Progress update callback
          if (progressUpdateCallback != NULL)
            progressUpdateCallback(_current_progress_size, request->contentLength());
        }

        if (final) {  // if the final flag is set then this is the last frame of data
          if (!Update.end(true)) {  // true to set the size to the current progress
            // Save error to string
            StreamString str;
            Update.printError(str);
            _update_error_str = str.c_str();
            _update_error_str.concat("\n");
            ELEGANTOTA_DEBUG_MSG(_update_error_str.c_str());
          }
        } else {
          return;
        }
      }
  );
  #else
  _server->on(
      "/ota/upload", HTTP_POST,
      [&]() {
        if (_authenticate && !_server->authenticate(_username.c_str(), _password.c_str())) {
          return _server->requestAuthentication();
        }
        // Post-OTA update callback
        if (postUpdateCallback != NULL) postUpdateCallback(!Update.hasError());
        _server->sendHeader("Connection", "close");
        _server->send(
            (Update.hasError()) ? 400 : 200, "text/plain",
            (Update.hasError()) ? _update_error_str.c_str() : "OK"
        );
        // Set reboot flag
        if (!Update.hasError()) {
          if (_auto_reboot) {
            _reboot_request_millis = millis();
            _reboot = true;
          }
        }
      },
      [&]() {
        // Actual OTA Download
        HTTPUpload &upload = _server->upload();
        if (upload.status == UPLOAD_FILE_START) {
          // Check authentication
          if (_authenticate && !_server->authenticate(_username.c_str(), _password.c_str())) {
            ELEGANTOTA_DEBUG_MSG("Authentication Failed on UPLOAD_FILE_START\n");
            return;
          }
          Serial.printf("Update Received: %s\n", upload.filename.c_str());
          _current_progress_size = 0;
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
    #if UPDATE_DEBUG == 1
            Update.printError(Serial);
    #endif
          }

          _current_progress_size += upload.currentSize;
          // Progress update callback
          if (progressUpdateCallback != NULL)
            progressUpdateCallback(_current_progress_size, upload.totalSize);
        } else if (upload.status == UPLOAD_FILE_END) {
          if (Update.end(true)) {
            ELEGANTOTA_DEBUG_MSG(
                String("Update Success: " + String(upload.totalSize) + "\n").c_str()
            );
          } else {
            ELEGANTOTA_DEBUG_MSG("[!] Update Failed\n");
            // Store error to string
            StreamString str;
            Update.printError(str);
            _update_error_str = str.c_str();
            _update_error_str.concat("\n");
            ELEGANTOTA_DEBUG_MSG(_update_error_str.c_str());
          }

    #if UPDATE_DEBUG == 1
          Serial.setDebugOutput(false);
    #endif
        } else {
          ELEGANTOTA_DEBUG_MSG(
              String(
                  "Update Failed Unexpectedly (likely broken connection): status=" +
                  String(upload.status) + "\n"
              )
                  .c_str()
          );
        }
      }
  );
  #endif
}

void ElegantOTAClass::setAuth(const char *username, const char *password) {
  _username = username;
  _password = password;
  _authenticate = _username.length() && _password.length();
}

void ElegantOTAClass::clearAuth() { _authenticate = false; }

void ElegantOTAClass::onStart(std::function<void()> callable) { preUpdateCallback = callable; }

void ElegantOTAClass::onProgress(std::function<void(size_t current, size_t final)> callable) {
  progressUpdateCallback = callable;
}

void ElegantOTAClass::onEnd(std::function<void(bool success)> callable) {
  postUpdateCallback = callable;
}

ElegantOTAClass ElegantOTA;

#endif
