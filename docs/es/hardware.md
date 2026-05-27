# Hardware DIY

Si quieres construir tu propio SVITRIX, aquí está la referencia de hardware para el reloj Ulanzi TC001.

## Microcontrolador

- **Chip**: ESP32-WROOM-32D (dual-core Xtensa LX6, 240 MHz)
- **Flash**: 8 MB
- **RAM**: 520 KB SRAM
- **WiFi**: 2.4 GHz 802.11 b/g/n
- **Bluetooth**: BT 4.2 + BLE (no usado por SVITRIX)
- **USB-Serial**: CH340 — permite programación USB directa sin programador externo

## Pinout

| GPIO | Función | Dirección | Notas |
|------|---------|-----------|-------|
| 32 | Matriz LED (WS2812B-Mini) | Salida | Línea de datos NeoPixel, 256 LEDs |
| 34 | ADC voltaje batería | Entrada | ADC1_CH6, solo entrada, divisor de voltaje a 0–3.3 V |
| 35 | Sensor de luz LDR (GL5516) | Entrada | ADC1_CH7, solo entrada |
| 26 | Botón izquierdo | Entrada | Activo BAJO, pull-up interno |
| 27 | Botón medio (Seleccionar) | Entrada | Activo BAJO, pull-up interno, pin de despertar deep sleep |
| 14 | Botón derecho | Entrada | Activo BAJO, pull-up interno |
| 13 | Botón reset (oculto) | Entrada | Mantener 5s → reset de fábrica |
| 15 | Buzzer (piezo pasivo) | Salida | PWM vía LEDC. Debe usar `INPUT_PULLDOWN` al iniciar para prevenir ruido flotante |
| 21 | I2C SDA | Bidireccional | Bus compartido: sensor de temperatura + RTC |
| 22 | I2C SCL | Salida | Bus compartido: sensor de temperatura + RTC |

## Dispositivos I2C

| Dispositivo | Dirección | Función |
|-------------|-----------|---------|
| SHT3x (SHT31) | `0x44` | Sensor de temperatura + humedad |
| DS1307 | `0x68` | Reloj en tiempo real (RTC) |

### Sensores de Temperatura Soportados

SVITRIX auto-detecta los siguientes sensores al iniciar (en este orden):

1. BME280 (temp + humedad + presión)
2. BMP280 (temp + presión)
3. HTU21DF (temp + humedad)
4. SHT31 (temp + humedad) — incluido en Ulanzi TC001

El Ulanzi TC001 viene con un sensor SHT3x. Las lecturas de temperatura pueden estar elevadas debido al calor interno de la matriz LED, ESP32 y batería dentro de la carcasa sellada. Usa `temp_offset` en `dev.json` para compensar.

### Reloj en Tiempo Real (DS1307)

El Ulanzi TC001 tiene un RTC DS1307 dedicado en el bus I2C en la dirección `0x68`. El RTC interno del ESP32 no es suficientemente preciso para mantener la hora real. SVITRIX actualmente usa NTP para sincronización de tiempo vía WiFi.

## Batería

- **Capacidad**: 4400 mAh Li-ion con circuito de carga integrado
- **Medición de voltaje**: GPIO34 con divisor de voltaje escalado a rango 0–3.3 V
- **Mapeo ADC**: valores raw ~510 (vacía) a ~660 (llena)

## Matriz LED

- **Tipo**: LEDs RGB WS2812B-Mini
- **Tamaño**: 8×32 (256 LEDs en total)
- **Pin de datos**: GPIO32 (un solo cable, encadenados)
- **Cableado**: Patrón serpentina — filas pares de izquierda a derecha, filas impares de derecha a izquierda:

| Fila | Secuencia |
|------|-----------|
| 1 | `[001] → [002] → ... → [032]` |
| 2 | `[064] ← [063] ← ... ← [033]` |
| 3 | `[065] → [066] → ... → [096]` |
| 4 | `[128] ← [127] ← ... ← [097]` |
| 5 | `[129] → [130] → ... → [160]` |
| 6 | `[192] ← [191] ← ... ← [161]` |
| 7 | `[193] → [194] → ... → [224]` |
| 8 | `[256] ← [255] ← ... ← [225]` |

### Configuración de Matriz

Si la matriz muestra caracteres sin sentido, el tipo de matriz debe cambiarse.

Crea un `dev.json` en tu administrador de archivos con el siguiente contenido:

```json
{
  "matrix": 2
}
```

| Valor | Configuración |
|-------|---------------|
| 0 | Matriz única 32x8, cableado zigzag (por defecto) |
| 1 | Cuatro paneles 8x8 en mosaico (4x1), progresivo |
| 2 | Matriz única 32x8, columna mayor, zigzag |

## Hojas de Datos

- [ESP32-WROOM-32D](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf)
- [SHT3x (temperatura/humedad)](https://sensirion.com/media/documents/213E6A3B/63A5A569/Datasheet_SHT3x_DIS.pdf)
- [DS1307 (RTC)](https://www.analog.com/media/en/technical-documentation/data-sheets/ds1307.pdf)
- [WS2812B-Mini (LED)](https://www.tme.eu/Document/01c0100fee68667af99767edc3a7fee2/WS2812B-MINI.pdf)
- [GL5516 (fotoresistor)](https://www.kth.se/social/files/54ef17dbf27654753f437c56/GL5537.pdf)
