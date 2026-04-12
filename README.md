<!-- markdownlint-configure-file {
  "MD013": {
    "code_blocks": false,
    "tables": false
  },
  "MD033": false,
  "MD041": false
} -->

<div align="center">

![SVITRIX](/cover.png?raw=true "SVITRIX")

**Custom firmware for the [Ulanzi Smart Pixel Clock TC001](https://www.ulanzi.com/products/ulanzi-pixel-smart-clock-2882?ref=28e02dxl) and self-built matrix clocks**

[![GitHub releases](https://img.shields.io/github/downloads/svitrix/svitrix-firmware/total?style=flat-square)](https://github.com/svitrix/svitrix-firmware/releases)
[![GitHub stars](https://img.shields.io/github/stars/svitrix/svitrix-firmware?style=flat-square)](https://github.com/svitrix/svitrix-firmware)
[![AI-First](https://img.shields.io/badge/AI--First-Claude-blueviolet?style=flat-square)](https://claude.ai)

[Quick Start](#quick-start) · [Features](#features) · [API](#api) · [Documentation](https://svitrix.github.io/svitrix-firmware/) · [FAQ](#faq)

</div>

---

SVITRIX is a smart home companion designed for HomeAssistant, IOBroker, NodeRed, and other automation systems. It works out of the box with pre-installed apps for time, date, temperature, humidity, and battery. For advanced users, the powerful MQTT and HTTP API allows creating custom apps, sending notifications, and controlling every aspect of the display.

SVITRIX is a community-driven fork of the original [AWTRIX 3](https://github.com/Blueforcer/awtrix3) project, created to enable active development and accept contributions from the community.

> **Note:** In SVITRIX, "Custom Apps" are not traditional apps you install. They are dynamic pages that rotate on the display, showing content sent from an external system via MQTT or HTTP. All logic is handled by your smart home — SVITRIX provides the display.

## Features

- **Ready out of the box** — time, date, temperature, humidity, and battery apps pre-installed
- **MQTT & HTTP API** — full control over apps, notifications, settings, and display
- **Custom Apps** — create dynamic pages from your smart home without recompiling
- **HomeAssistant discovery** — automatic integration with HA
- **Notifications** — one-time messages with icons, sounds, and effects
- **19 visual effects** — Fireworks, Matrix, Plasma, Snake, and more as app backgrounds
- **Weather overlays** — snow, rain, storm, thunder, frost effects
- **RTTTL melodies** — play monophonic sounds via the built-in buzzer
- **Animated & static icons** — download from LaMetric gallery or upload your own
- **Drawing API** — pixels, lines, rectangles, circles, text, and bitmaps
- **Bar & line charts** — display data graphs directly on the matrix
- **Colored indicators** — small notification dots in screen corners
- **Mood lighting** — turn the matrix into an ambient light
- **Onscreen menu** — change settings directly on the device with buttons
- **Web interface** — WiFi setup, MQTT config, file manager, icon downloader, OTA updates, live view
- **Artnet (DMX)** — use SVITRIX as an Artnet receiver
- **Custom color palettes** — create your own 16-color palettes for effects
- **DIY hardware support** — build your own with any ESP32-WROOM board
- **Online flasher** — flash directly from your browser via USB
- **Backup & restore** — full flash backup as a zip file
- **No cloud, no telemetry**

## Quick Start

1. **Flash** — connect the Ulanzi TC001 via USB and use the [online flasher](https://svitrix.github.io/svitrix-firmware/flasher)
2. **Connect** — join the `svitrix_XXXXX` WiFi network (password: `12345678`)
3. **Configure** — open `http://192.168.4.1` and enter your home WiFi credentials
4. **Use** — the device IP appears on the display; open it in a browser for the web interface

For detailed instructions, see the [Quick Start Guide](https://svitrix.github.io/svitrix-firmware/quickstart).

## API

SVITRIX provides a dual MQTT/HTTP API. A few examples:

**Send a notification:**

```bash
curl -X POST http://<ip>/api/notify \
  -d '{"text": "Hello!", "icon": "1234", "duration": 10, "rainbow": true}'
```

**Create a custom app:**

```bash
curl -X POST "http://<ip>/api/custom?name=myapp" \
  -d '{"text": "23°C", "icon": "2056", "lifetime": 900}'
```

**Change settings:**

```bash
curl -X POST http://<ip>/api/settings \
  -d '{"BRI": 120, "TMODE": 1, "ATIME": 5}'
```

Full API reference: [API Reference](https://svitrix.github.io/svitrix-firmware/api)

## DIY Hardware

The Ulanzi TC001 uses an **ESP32-WROOM-32D** (8 MB flash, CH340 USB-Serial) with a 32x8 WS2812B-Mini matrix (256 LEDs).

| GPIO | Function | Notes |
|------|----------|-------|
| 32 | LED Matrix (WS2812B-Mini) | 256 LEDs, serpentine wiring |
| 15 | Buzzer (passive piezo) | PWM via LEDC, needs pull-down at init |
| 21/22 | I2C SDA/SCL | SHT3x (0x44) + DS1307 RTC (0x68) |
| 26/27/14 | Left/Middle/Right buttons | Active LOW, internal pull-up |
| 13 | Reset button (hidden) | 5s hold → factory reset |
| 34 | Battery voltage ADC | 4400 mAh, voltage divider |
| 35 | LDR light sensor (GL5516) | Ambient light detection |

Supported I2C sensors: BME280, BMP280, HTU21DF, SHT31 (auto-detected at startup). Full hardware reference: [Hardware Guide](https://svitrix.github.io/svitrix-firmware/hardware).

## FAQ

**Does SVITRIX work without a smart home system?**
Yes, partially. Built-in apps (time, date, temperature, humidity, battery) work out of the box. Custom apps and notifications require an external system communicating via MQTT or HTTP.

**Can I use a different matrix size?**
No. SVITRIX is optimized for 32x8 (256 LEDs).

**The temperature reading seems too high.**
The sensor is inside the case. Set an offset in `dev.json` via the file manager:

```json
{"temp_offset": -5, "hum_offset": -1}
```

**How do I update firmware without USB?**
Use the OTA update section in the web interface, or download the latest `.bin` from [GitHub releases](https://github.com/svitrix/svitrix-firmware/releases).

More answers: [FAQ](https://svitrix.github.io/svitrix-firmware/faq)

## AI-First Development

SVITRIX is developed using an **AI-first** approach. [Claude](https://claude.ai) (Anthropic) is a core part of the development workflow — from architecture decisions and code generation to refactoring, testing, and CI pipeline management. The project uses [Claude Code](https://github.com/anthropics/claude-code) as the primary development tool, with custom hooks and skills tailored to the firmware workflow.

## Contributing

Star the repo, open issues, and submit pull requests — contributions are welcome!

## Disclaimer

This open-source software is not affiliated with or endorsed by the company Ulanzi in any way. Use of the software is at your own risk and discretion, and I assume no liability for any potential damages or issues that may arise from using the software.
