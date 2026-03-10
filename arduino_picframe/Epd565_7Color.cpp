#include "Epd565_7Color.h"

namespace {
// Preserved from original firmware/main/epd.c init table.
const Epd565_7Color::InitCmd kInitCmds[] = {
    {0x00, {0xef, 0x08}, 2},
    {0x01, {0x37, 0x00, 0x23, 0x23}, 4},
    {0x03, {0x00}, 1},
    {0x06, {0xC7, 0xC7, 0x1D}, 3},
    {0x30, {0x39}, 1},
    {0x41, {0x00}, 1},
    {0x50, {0x37}, 1},
    {0x60, {0x22}, 1},
    {0x61, {0x02, 0x58, 0x01, 0xC0}, 4},
    {0xE3, {0xAA}, static_cast<uint8_t>(1 | Epd565_7Color::kInitDataWait)},
    {0x50, {0x37}, 1},
    {0x00, {0x00}, 0xFF},
};
} // namespace

Epd565_7Color::Epd565_7Color(uint8_t busyPin, uint8_t rstPin, uint8_t dcPin,
                             uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
                             SPIClass &spi)
    : busyPin_(busyPin),
      rstPin_(rstPin),
      dcPin_(dcPin),
      csPin_(csPin),
      sckPin_(sckPin),
      mosiPin_(mosiPin),
      spi_(spi) {}

bool Epd565_7Color::begin() {
  Serial.println("[EPD] begin(): configuring pins and SPI");

  pinMode(dcPin_, OUTPUT);
  pinMode(rstPin_, OUTPUT);
  pinMode(csPin_, OUTPUT);
  pinMode(busyPin_, INPUT_PULLUP);

  digitalWrite(csPin_, HIGH);
  digitalWrite(dcPin_, HIGH);

  spi_.begin(sckPin_, -1, mosiPin_, csPin_);

  hardwareReset();
  if (!waitBusyLevel(HIGH, 1000)) {
    Serial.println("[EPD] begin(): busy did not go HIGH after reset");
    return false;
  }

  for (size_t i = 0; kInitCmds[i].dataBytes != 0xFF; ++i) {
    sendCommand(kInitCmds[i].cmd);
    sendData(kInitCmds[i].data, kInitCmds[i].dataBytes & 0x1F);
    if (kInitCmds[i].dataBytes & kInitDataWait) {
      delay(100);
    }
  }

  initialized_ = true;
  Serial.println("[EPD] begin(): init sequence done");
  return true;
}

bool Epd565_7Color::displayFrame(const uint8_t *framebuffer, bool sleepAfter) {
  if (!initialized_) {
    Serial.println("[EPD] displayFrame(): not initialized");
    return false;
  }

  Serial.println("[EPD] displayFrame(): pushing 600x448x4bpp framebuffer");

  sendCommand(0x61);
  const uint8_t geometry[] = {0x02, 0x58, 0x01, 0xC0};
  sendData(geometry, sizeof(geometry));

  sendCommand(0x10);

  // 600 pixels * 4 bits = 300 bytes per line.
  constexpr size_t bytesPerLine = kWidth / 2;
  for (uint16_t y = 0; y < kHeight; ++y) {
    sendData(&framebuffer[y * bytesPerLine], bytesPerLine);
    if ((y % 32) == 0) {
      Serial.printf("[EPD] sent line %u/%u\n", y, kHeight);
    }
  }

  Serial.println("[EPD] displayFrame(): trigger panel refresh");
  sendCommand(0x04);
  if (!waitBusyLevel(HIGH, 30000)) return false;

  sendCommand(0x12);
  if (!waitBusyLevel(HIGH, 30000)) return false;

  sendCommand(0x02);
  if (!waitBusyLevel(HIGH, 30000)) return false;

  Serial.println("[EPD] displayFrame(): refresh complete");
  if (sleepAfter) {
    sleep();
  }
  return true;
}

void Epd565_7Color::sleep() {
  if (!initialized_) return;

  Serial.println("[EPD] sleep(): sending deep-sleep command");
  sendCommand(0x07);
  const uint8_t data = 0xA5;
  sendData(&data, 1);
}

void Epd565_7Color::sendCommand(uint8_t cmd) {
  digitalWrite(dcPin_, LOW);
  digitalWrite(csPin_, LOW);
  spi_.beginTransaction(SPISettings(1'000'000, MSBFIRST, SPI_MODE0));
  spi_.transfer(cmd);
  spi_.endTransaction();
  digitalWrite(csPin_, HIGH);
}

void Epd565_7Color::sendData(const uint8_t *data, size_t len) {
  if (len == 0) return;

  digitalWrite(dcPin_, HIGH);
  digitalWrite(csPin_, LOW);
  spi_.beginTransaction(SPISettings(1'000'000, MSBFIRST, SPI_MODE0));
  while (len--) {
    spi_.transfer(*data++);
  }
  spi_.endTransaction();
  digitalWrite(csPin_, HIGH);
}

void Epd565_7Color::hardwareReset() {
  digitalWrite(rstPin_, LOW);
  delay(10);
  digitalWrite(rstPin_, HIGH);
  delay(100);
}

bool Epd565_7Color::waitBusyLevel(int expectedLevel, uint32_t timeoutMs) {
  const uint32_t start = millis();
  while (digitalRead(busyPin_) != expectedLevel) {
    delay(2);
    if ((millis() - start) > timeoutMs) {
      Serial.printf("[EPD] waitBusyLevel(): timeout waiting for %s\n",
                    expectedLevel ? "HIGH" : "LOW");
      return false;
    }
  }
  return true;
}
