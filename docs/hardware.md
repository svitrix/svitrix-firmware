# DIY Hardware

If you want to build your own SVITRIX, here is the pinout of the Ulanzi TC001 clock.

| GPIO | Function | Direction | Notes |
|------|----------|-----------|-------|
| 32 | LED Matrix (WS2812B) | Output | NeoPixel data line |
| 34 | Battery voltage ADC | Input | ADC1_CH6, input-only |
| 35 | LDR light sensor (GL5516) | Input | ADC1_CH7, input-only |
| 26 | Left button | Input | Active LOW, optional |
| 27 | Middle button (Select) | Input | Active LOW, optional |
| 14 | Right button | Input | Active LOW, optional |
| 13 | Reset button (hidden) | Input | 5s hold → factory reset |
| 15 | Buzzer (passive piezo) | Output | PWM via LEDC |
| 21 | I2C SDA | Bidirectional | Temperature/humidity sensors |
| 22 | I2C SCL | Output | Temperature/humidity sensors |

### Supported Sensors (I2C)

- BME280 (temp + humidity)
- BMP280 (temp only)
- HTU21DF (temp + humidity)
- SHT31 (temp + humidity)

Sensors are auto-detected at startup in the order listed above.

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
