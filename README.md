# 🛜 ESP32 OTA Lab

![Platform](https://img.shields.io/badge/platform-ESP32-00979D)
![Framework](https://img.shields.io/badge/framework-Arduino-00878F)
![Tooling](https://img.shields.io/badge/tooling-PlatformIO-orange)
![Status](https://img.shields.io/badge/status-experimental-yellow)

A hands-on playground for **two ESP-WROOM-32 development boards**, wireless firmware updates, and small electronics experiments — built with help from **GPT and Codex**.

> 🧪 This is a personal test project, not a production firmware. OTA and HTTP were successfully tested on both boards before this repository cleanup. The reorganized firmware must be verified on hardware after its first upload.

## ✨ What is here?

- 🛜 Password-protected ArduinoOTA updates, separated from experiment code.
- 🧩 Two PlatformIO environments in one codebase: `esp32-1` and `esp32-2`.
- 🌐 A local web page for DHT11, RGB lighting, and short 28BYJ-48 stepper movements.
- 🔐 Ignored local credentials; no private network addresses in shared configuration.
- 🛠️ VS Code tasks for building and uploading each board.
- 🤖 Polish instructions for working with GPT/Codex and planning wiring safely.

**Stepper addition:** ULN2003 IN1–IN4 use GPIO19/21/22/23, with short left/right movements and Stop. Separate 5 V motor power and common ground are required. Wiring, upload, and motor behavior still need hardware verification; see [the stepper guide](docs/STEPPER_WEB.md).

**In progress:** DHT11 + KY-016 climate lighting: manual RGB and brightness, plus automatic colors over 17–22°C with humidity-based saturation. Both firmware targets compile; the web UI was tested with simulated readings. Wiring, upload, sensor readings, physical colors, and subsequent OTA still need hardware verification.

SG90 addition: GPIO16, a 45–135° control slider, center/release buttons, and automatic signal release. Servo and stepper commands are interlocked. Separate motor power is required; servo wiring and operation still need hardware verification. See [the servo guide](docs/SERVO_WEB.md).

Second RGB LED: sync with the main lamp or choose an independent color and brightness. GPIO17/32/33 drive R/G/B through individual 1 kΩ resistors; the common cathode goes to ground. Hardware verification after upload is pending. See [the second RGB guide](docs/RGB_BARE.md).

## 🚀 Quick start

Install VS Code with the PlatformIO IDE extension, then open this project folder.

```sh
cp secrets.example.json secrets.local.json
chmod 600 secrets.local.json
```

Fill in Wi-Fi credentials, each board's OTA password, and its reachable address in `secrets.local.json`. This file stays on your computer. **If it already exists, keep it — do not overwrite it with the example.** The example uses mDNS names; replace them with local IP addresses if mDNS is unavailable.

The upload password must match the firmware **currently installed** on the target. Both boards normally use the same Wi-Fi network; per-device `wifi_ssid` and `wifi_password` overrides are supported.

### 🔨 Build

```sh
pio run -e esp32-1
pio run -e esp32-2
```

### 📡 Upload over Wi-Fi

Power the target board, connect the computer to its LAN, and select exactly one environment:

```sh
# Board 1
pio run -e esp32-1 -t upload

# Board 2
pio run -e esp32-2 -t upload
```

**macOS system Terminal — known working launcher on this workstation:**

```sh
# Board 1
~/.platformio/penv/bin/pio run -e esp32-1 -t upload

# Board 2
~/.platformio/penv/bin/pio run -e esp32-2 -t upload

# Previous command remains an alias for Board 1
~/.platformio/penv/bin/pio run -e nodemcu-32s -t upload
```

Run commands from the project root. These home-relative commands contain no personal host path. Avoid `pio run -t upload` without `-e`: the default configuration selects both boards.

### 🌐 Verify HTTP

With mDNS available:

```sh
curl --fail http://esp32-1.local/
curl --fail http://esp32-2.local/
```

For a local IP target, use the no-secret helper instead:

```sh
python3 scripts/check_http.py esp32-1
python3 scripts/check_http.py esp32-2
```

Confirm the response names the intended device. An HTTP response alone does not prove the OTA handler works; verify a subsequent OTA update separately.

## 🗂️ Project map

| Path | Purpose |
|---|---|
| `src/main.cpp` | Wi-Fi, DHT11 on GPIO18, RGB PWM on GPIO25/26/27, and HTTP API (hardware verification pending) |
| `include/web_page.h` | Local climate and RGB control page |
| `include/climate_color.h` | Temperature and humidity color mapping |
| `src/ota_service.cpp` | Dedicated OTA infrastructure |
| `include/ota_service.h` | OTA interface |
| `platformio.ini` | Shared board settings and two targets |
| `scripts/` | Private configuration, uploader, HTTP check, and publication audit |
| `secrets.example.json` | Shareable configuration template |
| `secrets.local.json` | **Ignored:** local Wi-Fi and OTA credentials |
| `.local/` | **Ignored:** workstation notes, addresses, private paths; no passwords |
| `.private-backup/` | **Ignored:** sensitive pre-migration recovery copy |
| `docs/` | Hardware, wiring, setup, and AI workflow in Polish |
| `AGENTS.md` | Polish project instructions for GPT/Codex |

## 🔌 Hardware bench

Two ESP-WROOM-32 boards; the existing `nodemcu-32s` board profile is retained. A **5 V / 2 A USB supply** powers a board through an **intact Micro-B USB cable**. There is also a breadboard with labelled rows, jumper wires, LEDs, servos, a temperature sensor, and assorted Arduino modules. Exact module models and pinouts still need identification.

See [hardware and wiring](docs/HARDWARE.md) before connecting modules. ESP32 GPIO uses **3.3 V logic**. Do not connect 5 V signals directly or power a servo from GPIO/3V3.

## 🧭 Guides

- [28BYJ-48 + ULN2003 — stepper wiring and controls in Polish](docs/STEPPER_WEB.md)
- [DHT11 + KY-016 — current experiment, wiring plan and behavior in Polish](docs/CLIMATE_RGB.md)
- [Previous blue LED experiment — historical wiring and usage in Polish](docs/LED_WEB.md)
- [Setup, migration, commands, and macOS troubleshooting](docs/SETUP.md)
- [Keeping OTA available](docs/OTA.md)
- [Hardware inventory and wiring workflow](docs/HARDWARE.md)
- [38-pin P/L map and GPIO reference — in Polish](docs/PINOUT.md)
- [Working with GPT and Codex — in Polish](docs/AI_WORKFLOW.md)
- [Agent instructions — in Polish](AGENTS.md)

## 🔐 Before sharing

Credentials are read locally and generated into an ignored build header. The uploader passes OTA authentication inside its Python process, avoiding passwords in command arguments and espota debug output. It does not encrypt the firmware or replace network security. Use OTA on a trusted LAN; do not expose OTA/HTTP to the Internet.

**Build artifacts contain credentials.** Never publish `.pio`, firmware binaries, local configuration, private notes, or backups. `.gitignore` prevents normal accidental additions; review staged files and never force-add private files.

```sh
python3 scripts/audit_public.py
git status --short
git diff --cached --stat
```

The audit checks the shareable tree before Git initialization and staged contents after initialization. It is an extra check, not proof that every possible secret has been found. No remote publication is performed by setup or build.

## 📚 Technical references

[PlatformIO ESP32 OTA](https://docs.platformio.org/en/latest/platforms/espressif32.html#over-the-air-ota-update) · [ArduinoOTA example](https://github.com/espressif/arduino-esp32/blob/master/libraries/ArduinoOTA/examples/BasicOTA/BasicOTA.ino) · [ESP32 partition tables](https://docs.espressif.com/projects/arduino-esp32/en/latest/tutorials/partition_table.html) · [Apple local-network permissions](https://support.apple.com/en-gb/guide/mac-help/mchla4f49138/mac) · [ESP32 datasheet](https://documentation.espressif.com/esp32_datasheet_en.html)
