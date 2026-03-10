# Arduino IDE Port (ESP32-C3-Zero + Waveshare 5.65" 7-color E-Paper)

This folder is a practical Arduino IDE port of the original ESP-IDF firmware, focused on **reliable display bring-up first**.

## What was kept from ESP-IDF

- The panel init command table from `firmware/main/epd.c` was preserved.
- The refresh sequence (`0x10` write buffer, then `0x04`, `0x12`, `0x02`) was preserved.
- The panel size and packed 4bpp format (`600x448`, `600*448/2` bytes) was preserved.

## What was intentionally simplified/stubbed

To keep this first version easy to compile and verify in Arduino IDE:

- Wi-Fi manager / HTTP sync / OTA update logic: **deferred**.
- NVS image slot management and scheduling: **deferred**.
- Button handling: **stubbed** to "not pressed".
- Battery ADC reading: **stubbed** to 3700 mV.
- Deep sleep scheduling: **deferred**.

## Arduino IDE setup

1. Install **Arduino IDE 2.x**.
2. Install ESP32 core by Espressif in Boards Manager.
3. Select board: **ESP32C3 Dev Module**.
4. Open `arduino_picframe.ino`.
5. Select the USB serial port of your Waveshare ESP32-C3-Zero.
6. Upload.

## Pin mapping used (fixed to your known-good wiring)

- BUSY -> GPIO2
- RST  -> GPIO3
- DC   -> GPIO4
- CS   -> GPIO5
- CLK/SCK -> GPIO6
- DIN/MOSI -> GPIO7

## Required libraries

No third-party Arduino libraries are required in this first port.
It only uses built-in Arduino core APIs and `<SPI.h>`.

## Expected behavior

- Serial monitor at 115200 prints step-by-step init/refresh logs.
- Display should update with a 7-bar test pattern.
- After refresh, panel is put into deep sleep command mode (`0x07`, `0xA5`).
