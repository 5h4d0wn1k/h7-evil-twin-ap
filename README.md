> **⚠️ EDUCATIONAL USE ONLY — AUTHORIZED TESTING ONLY.**
> This project exists for education, research, and **defense of systems you own
> or hold explicit written authorization to assess**. Unauthorized use is
> prohibited and may be illegal. Read [ETHICS.md](ETHICS.md) and
> [SCOPE.md](SCOPE.md) before use. Use at your own risk; **AS IS**, no warranty.

# H7 — Evil Twin AP: Rogue-AP Emulation & Credential-Log Audit Lab

[![License](https://img.shields.io/github/license/5h4d0wn1k/h7-evil-twin-ap)](LICENSE)
[![Stars](https://img.shields.io/github/stars/5h4d0wn1k/h7-evil-twin-ap)](https://github.com/5h4d0wn1k/h7-evil-twin-ap/stargazers)
[![Last Commit](https://img.shields.io/github/last-commit/5h4d0wn1k/h7-evil-twin-ap)](https://github.com/5h4d0wn1k/h7-evil-twin-ap/commits/master)
[![Issues](https://img.shields.io/github/issues/5h4d0wn1k/h7-evil-twin-ap)](https://github.com/5h4d0wn1k/h7-evil-twin-ap/issues)

**H7** is a wireless security lab for studying rogue access points: an
ESP32-C6 firmware that clones an SSID, hosts a captive portal, and builds a
deauth engine — paired with an offline Python host helper that redacts and
audits credential logs produced during authorized own-network tests.

## Why H7?

Rogue access points and captive-portal credential capture are central concepts
in Wi-Fi security education — and equally central to defending against them.
H7 provides the attacker side in a strictly lab-scoped way: the ESP32-C6 sketch
scans networks, clones SSID/channel, spoofs BSSID, serves a captive portal,
and drives a deauth engine, while the `host/` helper parses the resulting
credential logs with redaction-by-default and requires an explicit `--reveal`
flag in authorized labs. Everything above stays on networks you own.

## Features

- **Wi-Fi network scanner** — discovers nearby networks with signal strength.
- **SSID + channel cloning** — exact replica of the target network's name and
  channel (`firmware/h7_evil_twin/h7_evil_twin.ino`).
- **BSSID spoofing** — custom MAC address for the rogue AP.
- **Captive portal** — web-based credential capture page.
- **Deauth engine** — forces clients to reconnect (`broadcast`/`targeted`).
- **Serial control** — interactive command interface (`scan`, `start`, `stop`,
  `deauth`, `creds`, `select N`).
- **Offline credential-log audit** — `host/h7_cli.py` parses captured logs,
  redacts passwords by default, and reports SSID/password pairs
  (`--demo`, `--file`, `--reveal`).
- **Unit tests** — `python3 -m unittest discover -s tests`.

## Quickstart

### Prerequisites

- ESP32-C6 board + Arduino CLI with `esp32:esp32:esp32c6` core
- Python 3.8+ for the host helper

### Flash the firmware

```bash
arduino-cli compile --fqbn esp32:esp32:esp32c6 firmware/h7_evil_twin
arduino-cli upload --fqbn esp32:esp32:esp32c6 --port /dev/ttyACM0 firmware/h7_evil_twin
```

### Host-side log audit (offline demo, redacted)

```bash
python3 host/h7_cli.py --demo
python3 host/h7_cli.py --file fixtures/creds.log                 # redacted report
python3 host/h7_cli.py --file fixtures/creds.log --reveal        # authorized lab only
```

### Tests

```bash
python3 -m unittest discover -s tests
```

## Project Structure

- `firmware/h7_evil_twin/h7_evil_twin.ino` — ESP32-C6 evil-twin sketch
  (scanner, clone, portal, deauth).
- `host/h7_cli.py` — offline credential-log parser with redaction.
- `host/hw_common.py` — shared host helpers and demo tag.
- `fixtures/creds.log` — sample own-lab credential log.
- `docs/` — `ARCHITECTURE.md` and `TEST_LOG.md`.
- `tests/` — host helper unit tests.

## Research Context

H7 is the attacker counterpart to deauth-detection projects: combine it with a
beacon flooder for mass fake-AP studies, or with an IDS (WIDS) to validate
rogue-AP detection.

## Documentation

- [Architecture](docs/ARCHITECTURE.md)
- [Test log](docs/TEST_LOG.md)
- [Firmware notes](firmware/README.md)
- [ETHICS.md](ETHICS.md), [SCOPE.md](SCOPE.md), [SECURITY.md](SECURITY.md)

## Contributing

Contributions for educational and authorized wireless-security research are
welcome. See [CONTRIBUTING.md](CONTRIBUTING.md) and
[CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).

## License

MIT License — see [LICENSE](LICENSE) for details.

> **⚠️ EDUCATIONAL USE ONLY — AUTHORIZED TESTING ONLY.**