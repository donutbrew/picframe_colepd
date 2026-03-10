#include "BoardPins_WaveshareC3Zero.h"
#include "Epd565_7Color.h"

// Arduino adaptation of firmware/main entrypoint for quick bring-up.
// Platform-specific ESP-IDF systems (Wi-Fi manager, OTA, NVS image slots,
// scheduled deep sleep, ADC calibration) are intentionally stubbed in this
// first Arduino-ready firmware folder.

Epd565_7Color gEpd(BoardPins::kEpdBusy, BoardPins::kEpdRst, BoardPins::kEpdDc,
                   BoardPins::kEpdCs, BoardPins::kEpdSck, BoardPins::kEpdMosi);

int readBatteryMv() {
  // Stub: original firmware/main/io.c sampled ADC and tracked min voltage.
  return 3700;
}

bool readButtonPressed() {
  // Stub: original firmware used this to decide Wi-Fi config/AP mode.
  return false;
}

uint8_t* makeTestPattern() {
  static uint8_t frame[Epd565_7Color::kFramebufferBytes];
  constexpr uint16_t width = Epd565_7Color::kWidth;
  constexpr uint16_t height = Epd565_7Color::kHeight;
  constexpr uint16_t bytesPerLine = width / 2;

  // 7 vertical color bars (palette indexes 0..6) in 4-bit packed format.
  for (uint16_t y = 0; y < height; ++y) {
    for (uint16_t x = 0; x < width; x += 2) {
      uint8_t p0 = (x * 7) / width;
      uint8_t p1 = ((x + 1) * 7) / width;
      frame[y * bytesPerLine + (x / 2)] = static_cast<uint8_t>((p0 << 4) | (p1 & 0x0F));
    }
  }

  return frame;
}

void printPinMap() {
  Serial.println("[APP] EPD pin map (Waveshare ESP32-C3-Zero)");
  Serial.printf("[APP] BUSY=%u RST=%u DC=%u CS=%u SCK=%u MOSI=%u\n", BoardPins::kEpdBusy,
                BoardPins::kEpdRst, BoardPins::kEpdDc, BoardPins::kEpdCs,
                BoardPins::kEpdSck, BoardPins::kEpdMosi);
}

void setup() {
  Serial.begin(115200);
  delay(400);

  Serial.println("\n[APP] firmware/ Arduino build starting");
  Serial.println("[APP] Arduino IDE board target: ESP32C3 Dev Module");

  printPinMap();
  Serial.printf("[APP] battery(mV) stub=%d\n", readBatteryMv());
  Serial.printf("[APP] button stub=%s\n", readButtonPressed() ? "pressed" : "not pressed");

  if (!gEpd.begin()) {
    Serial.println("[APP] FAIL: display init");
    return;
  }

  uint8_t* frame = makeTestPattern();
  Serial.println("[APP] sending test frame");
  if (!gEpd.displayFrame(frame, true)) {
    Serial.println("[APP] FAIL: display refresh");
    return;
  }

  Serial.println("[APP] SUCCESS: refresh complete");
}

void loop() {
  delay(1000);
}
