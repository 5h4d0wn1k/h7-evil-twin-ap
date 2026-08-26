# H7 — Evil Twin AP

Clone any WiFi SSID, create a fake access point, and capture credentials via captive portal.

## Overview

This project implements an Evil Twin attack tool that:
- Scans for nearby WiFi networks
- Clones the SSID and channel of a target network
- Creates a fake AP with the same name
- Deauthenticates the original AP to force clients to reconnect
- Captures credentials through a captive portal

**WARNING: Educational use only. Test on your own networks.**

## Hardware

| Component | Connection | Role |
|-----------|------------|------|
| ESP32-C6 | Main board | Evil twin AP + deauth engine |

## Features

- **Network Scanner**: Discovers nearby networks with signal strength
- **SSID Cloning**: Exact replica of target network name
- **Channel Matching**: Operates on same channel as target
- **BSSID Spoofing**: Custom MAC address for AP
- **Captive Portal**: Web-based credential capture
- **Deauth Engine**: Forces clients to disconnect
- **Serial Control**: Interactive command interface

## Serial Commands

```
scan     - Scan for nearby networks
select N - Select network N as target
start    - Start evil twin AP
stop     - Stop evil twin AP
deauth   - Send deauth packets to target
creds    - Show captured credentials
```

## Captured Output

```
*** CREDENTIAL CAPTURED ***
SSID: MyHomeWiFi
Password: mysecretpassword
Client: AA:BB:CC:DD:EE:FF
**************************
```

## Build & Flash

```bash
# ESP32-C6 specific
arduino-cli compile --fqbn esp32:esp32:esp32c6 h7_evil_twin
arduino-cli upload --fqbn esp32:esp32:esp32c6 --port /dev/ttyACM0 h7_evil_twin
```

## Research Value

This project is the **attacker counterpart** to H1 (Deauth Detector):
- **W1 — Deauth Engine + IDS**: Complete attack/defense pair
- **W6 — Beacon Flood**: Extend to mass fake AP generation
- **X5 — MITM Suite**: Combine with network interception

## Legal Disclaimer

This tool is for authorized security testing only. Unauthorized use is illegal. Always obtain written permission before testing on networks you don't own.

## License

MIT
