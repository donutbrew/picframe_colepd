#pragma once

#include <Arduino.h>
#include <SPI.h>

// Arduino port of firmware/main/epd.c from the original ESP-IDF project.
// Changed for Arduino IDE + ESP32-C3-Zero wiring.
class Epd565_7Color {
public:
  static constexpr uint16_t kWidth = 600;
  static constexpr uint16_t kHeight = 448;
  static constexpr size_t kFramebufferBytes = (kWidth * kHeight) / 2; // 4bpp

  Epd565_7Color(uint8_t busyPin, uint8_t rstPin, uint8_t dcPin,
                uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
                SPIClass &spi = SPI);

  bool begin();
  bool displayFrame(const uint8_t *framebuffer, bool sleepAfter = true);
  void sleep();

private:
  void sendCommand(uint8_t cmd);
  void sendData(const uint8_t *data, size_t len);
  void hardwareReset();
  bool waitBusyLevel(int expectedLevel, uint32_t timeoutMs);

  uint8_t busyPin_;
  uint8_t rstPin_;
  uint8_t dcPin_;
  uint8_t csPin_;
  uint8_t sckPin_;
  uint8_t mosiPin_;
  SPIClass &spi_;
  bool initialized_ = false;
};
