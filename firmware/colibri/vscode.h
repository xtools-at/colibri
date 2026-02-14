#pragma once

// VSCode shims to make IntelliSense work mostly
#if (!defined(COLIBRI_PIO_BUILD) && (!defined(ARDUINO) || ARDUINO < 100))
  #undef ARDUINO
  #define ARDUINO 20320
  #define ARDUINO_ARCH_ESP32
  #define ARDUINO_BOARD "esp32dev"
  #define ARDUINO_USB_MODE 0
  #define ESP32
  #define ESP_PLATFORM
  #define CONFIG_IDF_TARGET "esp32s3"
  #define CONFIG_IDF_TARGET_ESP32S3
  #define CORE_DEBUG_LEVEL 4
  #define __XTENSA__
  #define INTERFACE_BLE_ARDUINO
  #define INTERFACE_BLE_NIMBLE
  #define INTERFACE_USB_HID
  #define OTA_ENABLED
  #define DISPLAY_ENABLED
  #define DISPLAY_SSD1306
  #define DISPLAY_WIDTH 135
  #define DISPLAY_HEIGHT 240
  #define CAMERA_ENABLED
  #define CAMERA_GPIO_PWDN -1
  #define CAMERA_GPIO_RESET -1
  #define CAMERA_GPIO_XCLK -1
  #define CAMERA_GPIO_SIOD -1
  #define CAMERA_GPIO_SIOC -1
  #define CAMERA_GPIO_D7 -1
  #define CAMERA_GPIO_D6 -1
  #define CAMERA_GPIO_D5 -1
  #define CAMERA_GPIO_D4 -1
  #define CAMERA_GPIO_D3 -1
  #define CAMERA_GPIO_D2 -1
  #define CAMERA_GPIO_D1 -1
  #define CAMERA_GPIO_D0 -1
  #define CAMERA_GPIO_VSYNC -1
  #define CAMERA_GPIO_HREF -1
  #define CAMERA_GPIO_PCLK -1

  #warning "Please compile this project in a recent version of Arduino IDE"
#endif
