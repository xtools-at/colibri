// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "CameraInterface.h"

#if defined(CAMERA_ENABLED)

static camera_config_t camera_config = {
    .pin_pwdn = CAMERA_GPIO_PWDN,
    .pin_reset = CAMERA_GPIO_RESET,
    .pin_xclk = CAMERA_GPIO_XCLK,
    .pin_sccb_sda = CAMERA_GPIO_SIOD,
    .pin_sccb_scl = CAMERA_GPIO_SIOC,
    .pin_d7 = CAMERA_GPIO_D7,
    .pin_d6 = CAMERA_GPIO_D6,
    .pin_d5 = CAMERA_GPIO_D5,
    .pin_d4 = CAMERA_GPIO_D4,
    .pin_d3 = CAMERA_GPIO_D3,
    .pin_d2 = CAMERA_GPIO_D2,
    .pin_d1 = CAMERA_GPIO_D1,
    .pin_d0 = CAMERA_GPIO_D0,
    .pin_vsync = CAMERA_GPIO_VSYNC,
    .pin_href = CAMERA_GPIO_HREF,
    .pin_pclk = CAMERA_GPIO_PCLK,

    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size = FRAMESIZE_SVGA,
    .jpeg_quality = 10,
    .fb_count = 2,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY
};

esp_err_t camera_init() { return esp_camera_init(&camera_config); }

esp_err_t camera_capture() {
  // capture a frame
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    log_e("Frame buffer could not be acquired");
    return ESP_FAIL;
  }

  // replace this with your own function
  // display_image(fb->width, fb->height, fb->format, fb->buf, fb->len);

  // return the frame buffer back to be reused
  esp_camera_fb_return(fb);

  return ESP_OK;
}

void CameraInterface::init() {
  log_i("Initializing camera interface...");
  esp_err_t err = camera_init();
  if (err != ESP_OK) {
    log_e("Camera initialization failed with error 0x%x", err);
    return;
  }

  log_d("Camera initialized successfully");
}

void CameraInterface::update() {}

void CameraInterface::disconnect() {}

void CameraInterface::stop() {
  disconnect();
  esp_camera_deinit();
}

void CameraInterface::wipe() { stop(); }

#endif
