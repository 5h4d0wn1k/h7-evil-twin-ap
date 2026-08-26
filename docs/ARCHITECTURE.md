# H7 — Evil Twin AP Architecture

## System Overview

The H7 Evil Twin AP is an offensive WiFi tool that creates a malicious access point identical to a target network, then captures credentials from clients that connect to it.

## Attack Flow

```
┌─────────────────┐
│  WiFi Scanner   │
│  (Target Disc.) │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  SSID Clone     │
│  (Evil Twin)    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐     ┌─────────────────┐
│  Deauth Engine  │────▶│  Target AP      │
│  (Force Reconn.)│     │  (Victim)       │
└────────┬────────┘     └─────────────────┘
         │
         ▼
┌─────────────────┐
│  Captive Portal │
│  (Cred Capture) │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Credential Log │
│  (Serial/SD)    │
└─────────────────┘
```

## Components

### 1. Network Scanner

Scans nearby WiFi networks using `WiFi.scanNetworks()`:
- SSID name
- Signal strength (RSSI)
- Channel number
- BSSID (MAC address)
- Encryption status

### 2. SSID Cloner

Creates an evil twin AP with:
- Same SSID as target
- Same channel as target
- Custom BSSID (incremented last byte)
- Open network (no password required)

### 3. Deauth Engine

Sends 802.11 deauthentication frames:
- **Broadcast deauth**: Forces all clients to disconnect
- **Targeted deauth**: Attacks specific client MAC
- **Reason code 7**: "Class 3 frame from non-associated"

### 4. Captive Portal

Web server that:
- Redirects all HTTP requests to login page
- Presents legitimate-looking WiFi login form
- Captures SSID and password on submission
- Logs credentials to serial output

## WiFi Frame Structure

### Deauth Frame

```
Bytes 0-1:   0xC0 0x00 (Deauthentication)
Bytes 2-3:   Duration
Bytes 4-9:   Destination (broadcast or client)
Bytes 10-15: Source (attacker AP)
Bytes 16-21: BSSID (target AP)
Bytes 22-23: Sequence number
Bytes 24-25: Reason code (0x0007)
```

## ESP32-C6 WiFi Capabilities

The C6 supports:
- WiFi 6 (802.11ax) — 2.4 GHz
- Monitor mode for frame injection
- AP+STA concurrent mode
- Custom BSSID setting

## Attack Scenarios

### Scenario 1: Coffee Shop Evil Twin

1. Scan for "CoffeeShop_WiFi"
2. Create evil twin with same name
3. Deauth legitimate AP
4. Capture credentials when users reconnect
5. Users think they're connecting to real network

### Scenario 2: Corporate Network

1. Scan for "CorpSecure"
2. Create evil twin (employees trust the name)
3. Deauth during lunch break (high traffic)
4. Capture domain credentials
5. Lateral movement with captured creds

### Scenario 3: Home Network (Lab)

1. Scan for your own network
2. Create evil twin for testing
3. Verify H1 (Deauth Detector) alerts
4. Test WPA handshake capture

## Defensive Countermeasures

This tool demonstrates why:
1. **WPA3-SAE**: Resists offline dictionary attacks
2. **802.1X/EAP**: Enterprise authentication
3. **Certificate pinning**: Prevents MITM
4. **Network monitoring**: Detects rogue APs (H1)

## Integration with Other Projects

```
H7 (Evil Twin) ──attacks──▶ H1 (Deauth Detector)
     │                            │
     │                            ▼
     │                    W1 (Deauth IDS)
     │
     ▼
X5 (MITM Suite) ──captures──▶ D6 (Email Forensics)
```

## References

- IEEE 802.11 Management Frames
- WiFi Evil Twin Attacks
- Captive Portal Implementation
- ESP32-C6 WiFi Documentation
