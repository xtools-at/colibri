// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../../../config.h"

#ifdef DISPLAY_ENABLED
  #include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device {
  // Panels:
  #if defined(DISPLAY_SH110X)
  lgfx::Panel_SH110x _panel_instance;  // SH1106, SH1107

  #elif defined(DISPLAY_SSD1306)
  lgfx::Panel_SSD1306 _panel_instance;

  #elif defined(DISPLAY_ST7735)
  lgfx::Panel_ST7735 _panel_instance;

  #elif defined(DISPLAY_ST7735S)
  lgfx::Panel_ST7735S _panel_instance;

  #elif defined(DISPLAY_ST7789)
  lgfx::Panel_ST7789 _panel_instance;

  #elif defined(DISPLAY_ST7796)
  lgfx::Panel_ST7796 _panel_instance;

  #elif defined(DISPLAY_ILI9341)
  lgfx::Panel_ILI9341 _panel_instance;

  #else
    #error "No display type selected, please check `config_board.h`"
  #endif

  // Bus:
  #ifndef DISPLAY_USE_I2C
  lgfx::Bus_SPI _bus_instance;
  #else
  lgfx::Bus_I2C _bus_instance;
  #endif

  // Light:
  lgfx::Light_PWM _light_instance;

  // Touch:
  // lgfx::Touch_FT5x06 _touch_instance;

 public:
  LGFX(void) {
    {  // > Communication
      auto cfg = _bus_instance.config();

  #ifndef DISPLAY_USE_I2C
      // SPI
      cfg.spi_host =  // ESP32-S2,C3 : SPI2_HOST or SPI3_HOST / ESP32: VSPI_HOST or HSPI_HOST
    #ifdef CONFIG_IDF_TARGET_ESP32
          VSPI_HOST;
    #else
          SPI2_HOST;
    #endif

      cfg.spi_mode = 0;  // Set the SPI communication mode (0 ~ 3)
      cfg.freq_write = 40000000;
      cfg.freq_read = 16000000;

    #if DISPLAY_GPIO_SPI_MISO == -1
      cfg.spi_3wire = true;  // Set to true if receiving data via the MOSI pin.
    #else
      cfg.spi_3wire = false;
    #endif

      cfg.use_lock = true;  // Set to true if using transaction locks.
      cfg.dma_channel = SPI_DMA_CH_AUTO;  // (0 = Do not use DMA / 1 = Channel 1 / 2 = Channel 2 /
                                          // SPI_DMA_CH_AUTO = Automatic configuration)

      cfg.pin_sclk = DISPLAY_GPIO_SPI_SCLK;  // SCLK
      cfg.pin_mosi = DISPLAY_GPIO_SPI_MOSI;  // MOSI
      cfg.pin_miso = DISPLAY_GPIO_SPI_MISO;  // MISO (-1 = disable)
      cfg.pin_dc = DISPLAY_GPIO_SPI_DC;  // DC
  #else
      // I2C
      cfg.i2c_addr = 0x3C;
      cfg.i2c_port = 0;  // (0 or 1)
      cfg.freq_write = 400000;
      cfg.freq_read = 400000;
      cfg.pin_sda = DISPLAY_GPIO_I2C_SDA;  // SDA
      cfg.pin_scl = DISPLAY_GPIO_I2C_SCL;  // SCL
      cfg.prefix_len = 0;
        // cfg.prefix_len = 1;
        // cfg.prefix_cmd = 0x00;
        // cfg.prefix_data = 0x40;
  #endif

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {  // > Panel
      auto cfg = _panel_instance.config();

      cfg.pin_cs = DISPLAY_GPIO_CS;  // (-1 = disable)
      cfg.pin_rst = DISPLAY_GPIO_RST;  // (-1 = disable)
      cfg.pin_busy = -1;  // (-1 = disable)

      cfg.panel_width = DISPLAY_WIDTH;
      cfg.panel_height = DISPLAY_HEIGHT;
      cfg.offset_x = DISPLAY_OFFSET_X;
      cfg.offset_y = DISPLAY_OFFSET_Y;
      cfg.offset_rotation = DISPLAY_OFFSET_ROTATION;
      cfg.dummy_read_pixel = 8;
      cfg.dummy_read_bits = 1;
      cfg.readable = true;
      cfg.invert = DISPLAY_INVERT;
      cfg.rgb_order = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = true;

  // Set the following only if there is a display offset issue with drivers
  // that have variable pixel counts, such as ST7735 or ILI9163.
  #if defined(DISPLAY_ST7735)
      cfg.memory_width = DISPLAY_WIDTH;
      cfg.memory_height = DISPLAY_HEIGHT;
  #endif

      _panel_instance.config(cfg);
    }

    {  // -- Backlight
      auto cfg = _light_instance.config();

      cfg.pin_bl = DISPLAY_GPIO_BACKLIGHT;
      cfg.invert = false;
      cfg.freq = 44100;
      cfg.pwm_channel = 7;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    /*
    // TODO: implement touchscreen support
    {  // -- Touch
      auto cfg = _touch_instance.config();

      cfg.x_min = 0;
      cfg.x_max = 239;
      cfg.y_min = 0;
      cfg.y_max = 319;
      cfg.pin_int = 38;
      cfg.bus_shared = true;
      cfg.offset_rotation = 0;

      // SPI
      cfg.spi_host = VSPI_HOST;  // (HSPI_HOST or VSPI_HOST)
      cfg.freq = 1000000;
      cfg.pin_sclk = 18;  // SCLK
      cfg.pin_mosi = 23;  // MOSI
      cfg.pin_miso = 19;  // MISO
      cfg.pin_cs = 5;  //   CS

      // I2C
      cfg.i2c_port = 1;  // (0 or 1)
      cfg.i2c_addr = 0x38;
      cfg.pin_sda = 23;  // SDA
      cfg.pin_scl = 32;  // SCL
      cfg.freq = 400000;

      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }
    */

    setPanel(&_panel_instance);
  }
};

  // include TFT_eSPI wrapper last, after `LGFX` is defined
  #include <LGFX_TFT_eSPI.hpp>

#endif
