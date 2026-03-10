#include "Epd565_7Color.h"

// ===== Hardware mapping (exactly as provided by user) =====
// BUSY -> GPIO2
// RST  -> GPIO3
// DC   -> GPIO4
// CS   -> GPIO5
// CLK  -> GPIO6
// DIN  -> GPIO7
constexpr uint8_t PIN_BUSY = 2;
constexpr uint8_t PIN_RST = 3;
constexpr uint8_t PIN_DC = 4;
constexpr uint8_t PIN_CS = 5;
constexpr uint8_t PIN_SCK = 6;
constexpr uint8_t PIN_MOSI = 7;

// Avoid GPIO10 on ESP32-C3-Zero (onboard RGB LED).

Epd565_7Color gDisplay(PIN_BUSY, PIN_RST, PIN_DC, PIN_CS, PIN_SCK, PIN_MOSI);

// Stubbed equivalents of ESP-IDF project peripherals.
int readBatteryMvStub() {
  // ESP-IDF firmware/main/io.c uses ADC + calibration.
  // For first Arduino port, keep behavior simple and deterministic.
  return 3700;
}

bool readButtonStub() {
  // ESP-IDF code checks a button to enter Wi-Fi config portal.
  // Stubbing this out for display bring-up.
  return false;
}

uint8_t *buildTestPattern() {
  static uint8_t frame[Epd565_7Color::kFramebufferBytes];

  // 4-bit packed pixels: two pixels per byte.
  // This creates seven vertical color bars (indices 0..6) across the panel.
  constexpr uint16_t width = Epd565_7Color::kWidth;
  constexpr uint16_t height = Epd565_7Color::kHeight;
  constexpr uint16_t bytesPerLine = width / 2;

  for (uint16_t y = 0; y < height; ++y) {
    for (uint16_t x = 0; x < width; x += 2) {
      uint8_t c0 = (x * 7) / width;
      uint8_t c1 = ((x + 1) * 7) / width;
      frame[y * bytesPerLine + (x / 2)] = (c0 << 4) | (c1 & 0x0F);
    }
  }

  return frame;
}

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("[APP] picframe_colepd Arduino port starting");
  Serial.println("[APP] Target board: ESP32C3 Dev Module (Waveshare ESP32-C3-Zero)");
  Serial.printf("[APP] Battery (stub): %d mV\n", readBatteryMvStub());
  Serial.printf("[APP] Button pressed (stub): %s\n", readButtonStub() ? "yes" : "no");

  if (!gDisplay.begin()) {
    Serial.println("[APP] Display init FAILED");
    return;
  }

  uint8_t *frame = buildTestPattern();
  Serial.println("[APP] Sending test pattern to e-Paper");

  if (!gDisplay.displayFrame(frame, true)) {
    Serial.println("[APP] Display refresh FAILED");
    return;
  }

  Serial.println("[APP] Display refresh OK");
  Serial.println("[APP] Setup complete. loop() stays idle.");
}

void loop() {
  delay(1000);
}
