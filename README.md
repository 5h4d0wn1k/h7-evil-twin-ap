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
SSID: lab-own-wifi
Password: h0n3y
Client: 00:11:22:33:44:55
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

## Live Lab Test Plan

Run ONLY on an isolated, authorized own-lab bench against devices, networks,
and spectrum **you own**. No third-party callers, bystanders, or spectrum users
may be within range of any test transmission.

1. **Isolate** - Put the DUT in a shielded/Faraday enclosure or a room with no
   third-party devices in range. Use attenuators on any transmit path.
2. **Own devices only** - Every target (AP, remote, tag, GPS module, drone FC,
   receiver) must be your own hardware.
3. **Lowest power, shortest duration** - Start at minimum TX power / duty cycle
   and use only the seconds needed.
4. **Record** - Save before/after logs to `reports/` (git-ignored). Never
   capture or store third-party traffic.
5. **Cleanup** - Restore placeholder SSIDs (`lab-*`), MACs (`00:11:22:33:44:55`),
   example.com / RFC5737 addresses, and clear any captured data from the device.

> Jammer / spoofer / replay projects are **proofs for study and simulation**
> only. They refuse live interference scenarios: a live bench trigger requires
> the `LAB_*` allowlist environment variable AND explicit `--yes` confirmation,
> and even then only against your own hardware in a shielded bench.

## Metrics

| Metric | Target | Where |
|---|---|---|
| Firmware compile | `arduino-cli compile --fqbn esp32:esp32:esp32c6 firmware/h7_evil_twin` PASS | CI/local |
| Host helper | `python3 host/h7_cli.py --demo` exits 0 (offline) | host/ |
| Unit tests | `python3 -m unittest discover -s tests` passes | tests/ |
| py_compile | every `host/*.py` compiles clean | CI/local |

## License

MIT
