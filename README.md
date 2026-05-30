<!-- markdownlint-configure-file {
  "MD013": {
    "code_blocks": false,
    "tables": false
  },
  "MD033": false,
  "MD041": false
} -->

<div align="center">

![SVITRIX-XE1E](/cover.png?raw=true "SVITRIX-XE1E")

**Custom firmware for the [Ulanzi Smart Pixel Clock TC001](https://www.ulanzi.com/products/ulanzi-pixel-smart-clock-2882?ref=28e02dxl)**

[![GitHub releases](https://img.shields.io/github/downloads/XE1E/svitrix-firmware-XE1E/total?style=flat-square)](https://github.com/XE1E/svitrix-firmware-XE1E/releases)
[![GitHub stars](https://img.shields.io/github/stars/XE1E/svitrix-firmware-XE1E?style=flat-square)](https://github.com/XE1E/svitrix-firmware-XE1E)
[![AI-First](https://img.shields.io/badge/AI--First-Claude-blueviolet?style=flat-square)](https://claude.ai)

[Quick Start](#quick-start) · [Features](#features) · [API](#api) · [Documentation](https://xe1e.github.io/svitrix-firmware-XE1E/) · [FAQ](#faq)

</div>

<div align="center">
  <img src="docs/public/assets/svitrix-screen.png" alt="Svitrix Display" width="600">
</div>

---

SVITRIX-XE1E is a smart home companion designed for HomeAssistant, IOBroker, NodeRed, and other automation systems. It works out of the box with pre-installed apps for time, date, temperature, humidity, battery, and **weather** (outdoor temperature, humidity, pressure, and air quality via WeatherAPI.com). For advanced users, the powerful MQTT and HTTP API allows creating custom apps, sending notifications, and controlling every aspect of the display.

SVITRIX-XE1E is a fork of [SVITRIX](https://github.com/svitrix/svitrix-firmware), which in turn is a community-driven fork of the original [AWTRIX 3](https://github.com/Blueforcer/awtrix3) project.

> **Note:** In SVITRIX-XE1E, "Custom Apps" are not traditional apps you install. They are dynamic pages that rotate on the display, showing content sent from an external system via MQTT or HTTP. All logic is handled by your smart home — SVITRIX-XE1E provides the display.

## Features

### Native Apps
- **Time** — multiple display modes including big-digit clock, binary clock, calendar box, weekday bar
- **Date** — formatted date with weekday indicator
- **Temperature & Humidity** — indoor readings from I2C sensors (SHT3x, BME280, etc.)
- **Battery** — charge percentage with animated icon
- **Weather Apps (NEW)** — outdoor temperature, humidity, pressure, and air quality via [WeatherAPI.com](https://weatherapi.com)
  - Configurable location (city name, coordinates, auto-detect by IP, or station ID)
  - Weather condition icons (sunny, cloudy, rainy)
  - Per-app color and duration settings
  - Animated LaMetric icons from `/ICONS/` folder

### Connectivity & Integration
- **MQTT & HTTP API** — full control over apps, notifications, settings, and display
- **Custom Apps** — create dynamic pages from your smart home without recompiling
- **HomeAssistant discovery** — automatic integration with HA
- **Multi-network WiFi** — configure up to 3 WiFi networks with automatic fallback
- **Artnet (DMX)** — use SVITRIX-XE1E as an Artnet receiver

### Display & Effects
- **19 visual effects** — Fireworks, Matrix, Plasma, Snake, and more as app backgrounds
- **Weather overlays** — snow, rain, storm, thunder, frost effects
- **Slide transitions** — smooth app transitions with multiple styles
- **Animated & static icons** — download from LaMetric gallery or upload your own
- **Drawing API** — pixels, lines, rectangles, circles, text, and bitmaps
- **Bar & line charts** — display data graphs directly on the matrix
- **Colored indicators** — small notification dots in screen corners
- **Mood lighting** — turn the matrix into an ambient light
- **Custom color palettes** — create your own 16-color palettes for effects
- **Auto-brightness** — configurable min/max brightness with LDR sensor

### Notifications & Sound
- **Notifications** — one-time messages with icons, sounds, and effects
- **RTTTL melodies** — play monophonic sounds via the built-in buzzer

### Configuration & Management
- **Modern web interface** — WiFi setup, MQTT config, weather settings, file manager, icon downloader, OTA updates, live view
- **Onscreen menu** — change settings directly on the device with buttons
- **Per-app duration** — configure display time for each native app
- **Time format options** — 12/24 hour, custom formats, Celsius/Fahrenheit
- **Night mode** — automatic brightness and color changes by time of day
- **Online flasher** — flash directly from your browser via USB
- **Backup & restore** — full flash backup as a zip file
- **No cloud, no telemetry**

## Quick Start

1. **Flash** — connect the Ulanzi TC001 via USB and use the [online flasher](https://xe1e.github.io/svitrix-firmware-XE1E/flasher)
2. **Connect** — join the `svitrix_XXXXX` WiFi network (password: `12345678`)
3. **Configure** — open `http://192.168.4.1` and enter your home WiFi credentials
4. **Use** — the device shows version, then scrolls "SVITRIX XE1E" with IP and mDNS hostname; open the IP in a browser for the web interface
5. **Weather (optional)** — get a free API key from [WeatherAPI.com](https://weatherapi.com) and configure it in Settings → Weather

For detailed instructions, see the [Quick Start Guide](https://xe1e.github.io/svitrix-firmware-XE1E/quickstart).

## API

SVITRIX-XE1E provides a dual MQTT/HTTP API. A few examples:

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

**Configure weather:**

```bash
curl -X POST http://<ip>/api/weather \
  -d '{"apiKey": "your_key", "locationType": 0, "city": "Mexico City", "showOutdoorTemp": true}'
```

**Get current weather data:**

```bash
curl http://<ip>/api/weather/data
```

Full API reference: [API Reference](https://xe1e.github.io/svitrix-firmware-XE1E/api)

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

Supported I2C sensors: BME280, BMP280, HTU21DF, SHT31 (auto-detected at startup). Full hardware reference: [Hardware Guide](https://xe1e.github.io/svitrix-firmware-XE1E/hardware).

## FAQ

**Does SVITRIX-XE1E work without a smart home system?**
Yes! Built-in apps (time, date, temperature, humidity, battery) work out of the box. Weather apps work standalone with just a WeatherAPI.com key. Custom apps and notifications require an external system communicating via MQTT or HTTP.

**Can I use a different matrix size?**
No. SVITRIX-XE1E is optimized for 32x8 (256 LEDs).

**The temperature reading seems too high.**
The sensor is inside the case. Set an offset in `dev.json` via the file manager:

```json
{"temp_offset": -5, "hum_offset": -1}
```

**How do I get weather data?**
1. Sign up for a free account at [WeatherAPI.com](https://weatherapi.com)
2. Copy your API key
3. Go to Settings → Weather in the web interface
4. Paste your API key and configure your location
5. Enable the weather apps you want (Outdoor Temp, Humidity, Pressure, Air Quality)

**How do I update firmware without USB?**
Use the OTA update section in the web interface, or download the latest `.bin` from [GitHub releases](https://github.com/XE1E/svitrix-firmware-XE1E/releases).

More answers: [FAQ](https://xe1e.github.io/svitrix-firmware-XE1E/faq)

## AI-First Development

SVITRIX-XE1E is developed using an **AI-first** approach. [Claude](https://claude.ai) (Anthropic) is a core part of the development workflow — from architecture decisions and code generation to refactoring, testing, and CI pipeline management. The project uses [Claude Code](https://github.com/anthropics/claude-code) as the primary development tool, with custom hooks and skills tailored to the firmware workflow.

## Contributing

Star the repo, open issues, and submit pull requests — contributions are welcome!

## Disclaimer

This open-source software is not affiliated with or endorsed by the company Ulanzi in any way. Use of the software is at your own risk and discretion, and I assume no liability for any potential damages or issues that may arise from using the software.
