# DIY Hardware

If you want to build your own SVITRIX, here is the hardware reference for the Ulanzi TC001 clock.

## Microcontroller

- **Chip**: ESP32-WROOM-32D (dual-core Xtensa LX6, 240 MHz)
- **Flash**: 8 MB
- **RAM**: 520 KB SRAM
- **WiFi**: 2.4 GHz 802.11 b/g/n
- **Bluetooth**: BT 4.2 + BLE (not used by SVITRIX)
- **USB-Serial**: CH340 — enables direct USB programming without external programmer

## Pinout

| GPIO | Function | Direction | Notes |
|------|----------|-----------|-------|
| 32 | LED Matrix (WS2812B-Mini) | Output | NeoPixel data line, 256 LEDs |
| 34 | Battery voltage ADC | Input | ADC1_CH6, input-only, voltage divider to 0–3.3 V |
| 35 | LDR light sensor (GL5516) | Input | ADC1_CH7, input-only |
| 26 | Left button | Input | Active LOW, internal pull-up |
| 27 | Middle button (Select) | Input | Active LOW, internal pull-up, deep sleep wakeup pin |
| 14 | Right button | Input | Active LOW, internal pull-up |
| 13 | Reset button (hidden) | Input | 5s hold → factory reset |
| 15 | Buzzer (passive piezo) | Output | PWM via LEDC. Must use `INPUT_PULLDOWN` at init to prevent floating noise |
| 21 | I2C SDA | Bidirectional | Shared bus: temperature sensor + RTC |
| 22 | I2C SCL | Output | Shared bus: temperature sensor + RTC |

## I2C Devices

| Device | Address | Function |
|--------|---------|----------|
| SHT3x (SHT31) | `0x44` | Temperature + humidity sensor |
| DS1307 | `0x68` | Real-time clock (RTC) |

### Supported Temperature Sensors

SVITRIX auto-detects the following sensors at startup (in this order):

1. BME280 (temp + humidity + pressure)
2. BMP280 (temp + pressure)
3. HTU21DF (temp + humidity)
4. SHT31 (temp + humidity) — built into Ulanzi TC001

The Ulanzi TC001 ships with an SHT3x sensor. Temperature readings may be elevated due to internal heat from the LED matrix, ESP32, and battery inside the sealed case. Use `temp_offset` in `dev.json` to compensate.

### Real-Time Clock (DS1307)

The Ulanzi TC001 has a dedicated DS1307 RTC on the I2C bus at address `0x68`. The built-in ESP32 RTC is not accurate enough for real-world timekeeping. SVITRIX currently uses NTP for time synchronization over WiFi.

## Battery

- **Capacity**: 4400 mAh Li-ion with built-in charging circuit
- **Voltage measurement**: GPIO34 with voltage divider scaled to 0–3.3 V range
- **ADC mapping**: raw values ~510 (empty) to ~660 (full)

## LED Matrix

- **Type**: WS2812B-Mini RGB LEDs
- **Size**: 8×32 (256 LEDs total)
- **Data pin**: GPIO32 (single-wire, daisy-chained)
- **Wiring**: Serpentine pattern — even rows left-to-right, odd rows right-to-left:

| Row | Sequence |
|-----|----------|
| 1 | `[001] → [002] → ... → [032]` |
| 2 | `[064] ← [063] ← ... ← [033]` |
| 3 | `[065] → [066] → ... → [096]` |
| 4 | `[128] ← [127] ← ... ← [097]` |
| 5 | `[129] → [130] → ... → [160]` |
| 6 | `[192] ← [191] ← ... ← [161]` |
| 7 | `[193] → [194] → ... → [224]` |
| 8 | `[256] ← [255] ← ... ← [225]` |

### Matrix Layout

If the matrix displays meaningless characters, the matrix type must be changed.

Create a `dev.json` in your file manager with the following content:

```json
{
  "matrix": 2
}
```

| Value | Configuration |
|-------|--------------|
| 0 | Single 32x8 matrix, zigzag wiring (default) |
| 1 | Four tiled 8x8 panels (4x1), progressive |
| 2 | Single 32x8 matrix, column-major, zigzag |

## Datasheets

- [ESP32-WROOM-32D](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf)
- [SHT3x (temperature/humidity)](https://sensirion.com/media/documents/213E6A3B/63A5A569/Datasheet_SHT3x_DIS.pdf)
- [DS1307 (RTC)](https://www.analog.com/media/en/technical-documentation/data-sheets/ds1307.pdf)
- [WS2812B-Mini (LED)](https://www.tme.eu/Document/01c0100fee68667af99767edc3a7fee2/WS2812B-MINI.pdf)
- [GL5516 (photoresistor)](https://www.kth.se/social/files/54ef17dbf27654753f437c56/GL5537.pdf)
