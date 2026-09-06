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

## IMPORTANT: Read before use.

This project is provided for **educational and authorized security testing purposes only**.

### Authorization Requirements
- You MUST have explicit written permission from the network/device owner before testing
- Use only on networks you own or have explicit authorization to test
- This tool is designed for research in your own lab only

### Legal Framework
- **Computer Fraud and Abuse Act (CFAA)**: Unauthorized access to computer systems is a federal crime
- **Title 18 U.S.C. § 2510 et seq. (Wiretap Act)**: Unauthorized interception of communications is illegal
- **State Laws**: Many states have additional computer crime and surveillance statutes

### Acceptable Use
- Research on networks you own (authorized lab bench)
- Security education and training
- Academic rogue-AP detection research in controlled environments

### Prohibited Use
- Cloning or impersonating access points you do not own
- Credential harvesting without authorization
- Any activity that violates applicable laws or regulations

### No Warranty
This software is provided "AS IS" without warranty of any kind. The author is not responsible for any misuse or damage caused by this software.

### Responsible Disclosure
If you discover vulnerabilities using this tool, follow responsible disclosure practices:
1. Report to the vendor/owner privately
2. Allow reasonable time for remediation
3. Do not exploit beyond proof of concept

## License

MIT
