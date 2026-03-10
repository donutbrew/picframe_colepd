#pragma once

#include <Arduino.h>

// Known-good wiring for Waveshare ESP32-C3-Zero + Waveshare 5.65" 7-color E-Paper.
namespace BoardPins {
constexpr uint8_t kEpdBusy = 2;
constexpr uint8_t kEpdRst = 3;
constexpr uint8_t kEpdDc = 4;
constexpr uint8_t kEpdCs = 5;
constexpr uint8_t kEpdSck = 6;
constexpr uint8_t kEpdMosi = 7;

// ESP32-C3-Zero note: GPIO10 is tied to onboard RGB LED, do not use.
constexpr uint8_t kForbiddenRgbLedPin = 10;

static_assert(kEpdBusy != kForbiddenRgbLedPin, "BUSY cannot be GPIO10");
static_assert(kEpdRst != kForbiddenRgbLedPin, "RST cannot be GPIO10");
static_assert(kEpdDc != kForbiddenRgbLedPin, "DC cannot be GPIO10");
static_assert(kEpdCs != kForbiddenRgbLedPin, "CS cannot be GPIO10");
static_assert(kEpdSck != kForbiddenRgbLedPin, "SCK cannot be GPIO10");
static_assert(kEpdMosi != kForbiddenRgbLedPin, "MOSI cannot be GPIO10");
}  // namespace BoardPins
