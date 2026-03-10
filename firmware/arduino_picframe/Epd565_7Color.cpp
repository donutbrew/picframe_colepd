#include "Epd565_7Color.h"

namespace {
struct EpdInitCmd {
  uint8_t cmd;
  uint8_t data[16];
  uint8_t dataBytes;  // bit7=delay, 0xFF=end
};

constexpr uint8_t kInitDataWait = 0x80;

// Preserved from firmware/main/epd.c
const EpdInitCmd kInitCmds[] = {
    {0x00, {0xef, 0x08}, 2},
    {0x01, {0x37, 0x00, 0x23, 0x23}, 4},
    {0x03, {0x00}, 1},
    {0x06, {0xC7, 0xC7, 0x1D}, 3},
    {0x30, {0x39}, 1},
    {0x41, {0x00}, 1},
    {0x50, {0x37}, 1},
    {0x60, {0x22}, 1},
    {0x61, {0x02, 0x58, 0x01, 0xC0}, 4},
    {0xE3, {0xAA}, static_cast<uint8_t>(1 | kInitDataWait)},
    {0x50, {0x37}, 1},
    {0x00, {0x00}, 0xFF},
};

constexpr uint32_t kSpiHz = 1'000'000;
}  // namespace

Epd565_7Color::Epd565_7Color(uint8_t busyPin, uint8_t rstPin, uint8_t dcPin,
                             uint8_t csPin, uint8_t sckPin, uint8_t mosiPin,
                             SPIClass& spi)
    : busyPin_(busyPin),
      rstPin_(rstPin),
      dcPin_(dcPin),
      csPin_(csPin),
      sckPin_(sckPin),
      mosiPin_(mosiPin),
      spi_(spi) {}

bool Epd565_7Color::begin() {
  Serial.println("[EPD] begin(): pin setup + SPI begin");
  pinMode(dcPin_, OUTPUT);
  pinMode(rstPin_, OUTPUT);
  pinMode(csPin_, OUTPUT);
  pinMode(busyPin_, INPUT_PULLUP);

  digitalWrite(csPin_, HIGH);
  digitalWrite(dcPin_, HIGH);

  spi_.begin(sckPin_, -1, mosiPin_, csPin_);

  hardwareReset();
  if (!waitBusyLevel(HIGH, 1000)) {
    Serial.println("[EPD] begin(): BUSY timeout after reset");
    return false;
  }

  for (size_t i = 0; kInitCmds[i].dataBytes != 0xFF; ++i) {
    sendCommand(kInitCmds[i].cmd);
    sendData(kInitCmds[i].data, kInitCmds[i].dataBytes & 0x1F);
    if (kInitCmds[i].dataBytes & kInitDataWait) delay(100);
  }

  initialized_ = true;
  Serial.println("[EPD] begin(): init done");
  return true;
}

bool Epd565_7Color::displayFrame(const uint8_t* framebuffer, bool sleepAfter) {
  if (!initialized_) return false;

  constexpr size_t bytesPerLine = kWidth / 2;

  sendCommand(0x61);
  const uint8_t geometry[] = {0x02, 0x58, 0x01, 0xC0};
  sendData(geometry, sizeof(geometry));

  sendCommand(0x10);
  for (uint16_t y = 0; y < kHeight; ++y) {
    sendData(&framebuffer[y * bytesPerLine], bytesPerLine);
    if ((y % 32) == 0) Serial.printf("[EPD] transfer line %u/%u\n", y, kHeight);
  }

  Serial.println("[EPD] triggering refresh");
  sendCommand(0x04);
  if (!waitBusyLevel(HIGH, 30000)) return false;

  sendCommand(0x12);
  if (!waitBusyLevel(HIGH, 30000)) return false;

  sendCommand(0x02);
  if (!waitBusyLevel(HIGH, 30000)) return false;

  if (sleepAfter) sleep();
  Serial.println("[EPD] refresh done");
  return true;
}

void Epd565_7Color::sleep() {
  if (!initialized_) return;
  sendCommand(0x07);
  const uint8_t x = 0xA5;
  sendData(&x, 1);
  Serial.println("[EPD] entered panel deep sleep command mode");
}

void Epd565_7Color::sendCommand(uint8_t cmd) {
  digitalWrite(dcPin_, LOW);
  digitalWrite(csPin_, LOW);
  spi_.beginTransaction(SPISettings(kSpiHz, MSBFIRST, SPI_MODE0));
  spi_.transfer(cmd);
  spi_.endTransaction();
  digitalWrite(csPin_, HIGH);
}

void Epd565_7Color::sendData(const uint8_t* data, size_t len) {
  if (len == 0) return;
  digitalWrite(dcPin_, HIGH);
  digitalWrite(csPin_, LOW);
  spi_.beginTransaction(SPISettings(kSpiHz, MSBFIRST, SPI_MODE0));
  while (len--) spi_.transfer(*data++);
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
  uint32_t start = millis();
  while (digitalRead(busyPin_) != expectedLevel) {
    if ((millis() - start) > timeoutMs) {
      Serial.printf("[EPD] BUSY timeout waiting for %s\n", expectedLevel ? "HIGH" : "LOW");
      return false;
    }
    delay(2);
  }
  return true;
}
