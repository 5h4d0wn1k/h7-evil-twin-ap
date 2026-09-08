# Evil Twin AP Firmware

## Purpose

Clone your OWN SSID in the lab and study captive-portal flows. Demo is analysis-only for credential logs produced in authorized lab tests.

## Board

- **Board**: ESP32-C6
- **FQBN**: `esp32:esp32:esp32c6`
- **Sketch**: `h7_evil_twin/h7_evil_twin.ino`

## Wiring

```
Standalone ESP32-C6. USB-C for serial/power.
```

## Build

```bash
arduino-cli compile --fqbn esp32:esp32:esp32c6 firmware/h7_evil_twin
# upload (example, ESP32-C6):
# arduino-cli upload --fqbn esp32:esp32:esp32c6 --port /dev/ttyACM0 firmware/h7_evil_twin
```

## Runtime

See the root README "IMPORTANT" section before powering on. This firmware is
for authorized own-lab study. Serial console exposes the interactive command
set described in the root README. All identifiers in the sketch are
placeholders (`lab-*` SSIDs, `00:11:22:33:44:55`, RFC 5737 / example.com).
