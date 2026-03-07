# Frequently Asked Questions (FAQ)

If you don't find what you're looking for, please open an issue on [GitHub](https://github.com/svitrix/svitrix-firmware/issues).

## General

#### Q: Does SVITRIX work without a smart home system?

A: Yes, partially. SVITRIX comes with built-in apps for time, date, temperature, humidity, and battery level — they work right out of the box without any external system.

However, SVITRIX is primarily designed as a smart home companion. To unlock its full potential — custom apps, notifications, remote control — you need an external system (HomeAssistant, IOBroker, NodeRed, etc.) that communicates via MQTT or HTTP API.

#### Q: Can I use different matrix sizes other than 32x8?

A: No. SVITRIX is optimized for the Ulanzi TC001, which uses a 32x8 pixel matrix (256 LEDs). Other sizes are not supported.

## Flashing & Installation

#### Q: I'm getting an error while flashing with the SVITRIX Web Flasher

A: Try a different USB cable and USB port. Some cables (including the one bundled with the device) support charging only, not data transfer. A good data cable is the most common fix.

#### Q: I only have 192KB of free flash memory. What's wrong?

A: The Ulanzi stock firmware uses a different partition table with a smaller filesystem partition.

To get the full ~1350KB of available storage, flash using the [SVITRIX online flasher](./flasher) — it writes to all partitions including the filesystem. The Ulanzi OTA updater only writes to the app partition and leaves the old partition layout in place.

## Display & Icons

#### Q: I'm experiencing drawing glitches with my icons

A: The SVITRIX GIF renderer supports 8-bit GIFs. If you notice graphic glitches, your GIF likely has transparency. Replace transparent pixels with solid black using an online tool like [this one](https://onlinegiftools.com/add-gif-background).

#### Q: I have a blinking indicator in the corner. What does it mean?

A: SVITRIX uses small indicator LEDs in the corners of the matrix to signal connection issues:

| Indicator | Position | Meaning |
|-----------|----------|---------|
| 1 | Top-right corner | WiFi connection problem |
| 2 | Bottom-right corner | MQTT broker connection problem |

Once the connection is restored, the indicator disappears automatically.

## Configuration

#### Q: What is `DoNotTouch.json` and what if I accidentally modified it?

A: `DoNotTouch.json` stores critical device settings:

- Static IP configuration
- MQTT connection settings
- HomeAssistant Discovery prefix
- NTP server / timezone
- HTTP Auth password

If you accidentally modified this file, delete it via the [file manager](./webinterface) and reboot the device. The file will be recreated automatically, but you will need to reconfigure the settings listed above.

#### Q: The temperature sensor shows a higher value than expected

A: The sensor on Ulanzi TC001 is located inside the sealed case. Internal heat from the LED matrix (depending on brightness) and the battery raises the reading above the actual room temperature.

You can compensate by setting offsets in `dev.json` via the [file manager](./webinterface):

```json
{
  "temp_offset": -5,
  "hum_offset": -1
}
```

Reboot the device after saving. Adjust the values to match your environment — the exact offset depends on brightness settings and battery usage.

See all available developer settings in the [Hidden Features](./dev) section.

## Custom Builds

#### Q: I want to build my own SVITRIX. Which firmware should I use?

A: Use the [SVITRIX flasher](./flasher) — the firmware is compatible with both Ulanzi TC001 and custom builds using any ESP32-WROOM board (including ESP32 D1 Mini).

Make sure to follow the correct pinout from the [Hardware Guide](./hardware).

#### Q: My self-built device shows garbled characters on the matrix

A: You need to change the matrix layout type. The default is `0`, which may not match your wiring.

Create a `dev.json` file via the [file manager](./webinterface) and try values `1` or `2`:

```json
{
  "matrix": "1"
}
```

| Value | Layout |
|-------|--------|
| 0 | Single 32x8, row-major, zigzag |
| 1 | Four tiled 8x8 panels, row-major, progressive |
| 2 | Single 32x8, column-major, zigzag |

Reboot the device after saving.

## OTA Updates

#### Q: How do I update the firmware without USB?

A: Open the [web interface](./webinterface) in your browser, go to the update section, and upload the new `firmware.bin` file. The device will reboot automatically after the update.

You can download the latest firmware from the [GitHub releases](https://github.com/svitrix/svitrix-firmware/releases) page.
