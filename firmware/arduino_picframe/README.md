# Arduino IDE firmware folder port

This folder is the Arduino-installable entry point for the original `firmware/` project.

## Goal of this first Arduino firmware pass

- Keep the display-driving behavior from `firmware/main/epd.c`.
- Keep your known-good Waveshare ESP32-C3-Zero pin mapping exact.
- Make it easy to compile/upload from Arduino IDE without ESP-IDF build steps.

## How to use in Arduino IDE

1. Open `firmware/arduino_picframe/firmware_arduino.ino`.
2. Install/select **ESP32 by Espressif Systems** board package.
3. Select board: **ESP32C3 Dev Module**.
4. Select the USB port for your Waveshare ESP32-C3-Zero.
5. Upload.
6. Open Serial Monitor at **115200**.

## Pin mapping (fixed)

- BUSY -> GPIO2
- RST  -> GPIO3
- DC   -> GPIO4
- CS   -> GPIO5
- CLK/SCK -> GPIO6
- DIN/MOSI -> GPIO7

These are defined centrally in `BoardPins_WaveshareC3Zero.h`, with compile-time checks to prevent accidental GPIO10 use.

## What from original firmware is currently deferred

The following ESP-IDF-specific subsystems are intentionally stubbed/deferred here so display bring-up is robust:

- `wifi_manager` and network sync (`sync.c`)
- OTA update flow
- NVS image-slot state tracking
- Deep sleep scheduling logic
- ADC-calibrated battery measurement and button-driven Wi-Fi portal mode

## Dependencies

- No third-party Arduino libs required.
- Uses only Arduino core and `<SPI.h>`.
