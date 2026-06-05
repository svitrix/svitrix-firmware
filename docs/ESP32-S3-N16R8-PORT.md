# Svitrix DIY — ESP32-S3 N16R8 Build Guide

> **Estado:** Documento de diseño completo  
> **Fecha:** 2026-06-05  
> **Hardware:** ESP32-S3-WROOM-1 N16R8 (16MB Flash, 8MB PSRAM)  
> **Target:** Build DIY completo (sin Ulanzi TC001)

---

## Tabla de Contenidos

1. [Resumen del Proyecto](#1-resumen-del-proyecto)
2. [Especificaciones del Sistema](#2-especificaciones-del-sistema)
3. [Bill of Materials (BOM)](#3-bill-of-materials-bom)
4. [Pinout Completo ESP32-S3](#4-pinout-completo-esp32-s3)
5. [Esquemáticos por Subsistema](#5-esquemáticos-por-subsistema)
6. [Cálculos de Potencia](#6-cálculos-de-potencia)
7. [Diseño de PCB](#7-diseño-de-pcb)
8. [Ensamblaje y Cableado](#8-ensamblaje-y-cableado)
9. [Cambios en el Firmware](#9-cambios-en-el-firmware)
10. [Testing y Validación](#10-testing-y-validación)
11. [Troubleshooting](#11-troubleshooting)
12. [Plan de Trabajo](#12-plan-de-trabajo)

---

## 1. Resumen del Proyecto

### 1.1 Objetivo

Construir un reloj/display LED inteligente basado en ESP32-S3 N16R8 con matriz WS2812B 32×8, compatible con el firmware Svitrix. Diseño DIY completo, sin dependencia del hardware Ulanzi TC001.

### 1.2 Características

- Display LED 32×8 (256 LEDs) full color
- WiFi 2.4GHz + Bluetooth 5.0
- Sensores de temperatura/humedad (I2C)
- Sensor de luz ambiente (auto-brillo)
- 4 botones de navegación
- Buzzer para alarmas/notificaciones
- Batería opcional (LiPo 3.7V)
- Integración Home Assistant (MQTT)
- OTA updates
- Web UI completa

### 1.3 Ventajas sobre TC001

| Aspecto | Ulanzi TC001 | DIY ESP32-S3 |
|---------|--------------|--------------|
| Flash | 8 MB (4 MB usados) | 16 MB |
| PSRAM | — | 8 MB |
| LittleFS | 256 KB | ~9.5 MB |
| USB | CH340 externo | Nativo USB-C |
| Personalización | Limitada | Total |
| Costo | ~$50 | ~$25-35 |
| Disponibilidad | Variable | Componentes estándar |

---

## 2. Especificaciones del Sistema

### 2.1 MCU: ESP32-S3-WROOM-1 N16R8

| Parámetro | Valor |
|-----------|-------|
| CPU | Dual Xtensa LX7 @ 240 MHz |
| Flash | 16 MB (Quad SPI) |
| PSRAM | 8 MB (Octal SPI) |
| SRAM | 512 KB |
| WiFi | 802.11 b/g/n, 2.4 GHz |
| Bluetooth | 5.0 LE |
| GPIO | 45 (usamos 12) |
| ADC | 2× 12-bit SAR |
| USB | OTG 1.1 + Serial/JTAG |
| Temp. operación | -40°C a +85°C |
| Voltaje | 3.0V - 3.6V (típico 3.3V) |

### 2.2 Display: Matriz WS2812B 32×8

| Parámetro | Valor |
|-----------|-------|
| LEDs totales | 256 (32 columnas × 8 filas) |
| Tipo LED | WS2812B-Mini o WS2812B |
| Voltaje | 5V DC |
| Corriente máx | 60mA/LED × 256 = 15.36A teórico |
| Corriente típica | 2-3A @ 50% brillo promedio |
| Protocolo | Single-wire, 800 kHz |
| Refresh rate | 60 FPS |

### 2.3 Diagrama de Bloques

```
┌─────────────────────────────────────────────────────────────────┐
│                        POWER SUPPLY                              │
│  ┌─────────┐    ┌─────────┐    ┌─────────┐                      │
│  │ USB-C   │───►│ 5V Rail │───►│ 3.3V    │                      │
│  │ or PSU  │    │         │    │ LDO     │                      │
│  └─────────┘    └────┬────┘    └────┬────┘                      │
│                      │              │                            │
└──────────────────────┼──────────────┼────────────────────────────┘
                       │              │
        ┌──────────────┼──────────────┼──────────────┐
        │              ▼              ▼              │
        │         ┌─────────────────────┐            │
        │         │   ESP32-S3-WROOM-1  │            │
        │         │      N16R8          │            │
        │         │                     │            │
        │         │  GPIO48 ──────────────────►  LED Matrix 32×8
        │         │                     │            │  (WS2812B)
        │         │  GPIO9  ◄── BTN_L   │            │
        │         │  GPIO10 ◄── BTN_SEL │            │
        │         │  GPIO11 ◄── BTN_R   │            │
        │         │  GPIO12 ◄── BTN_RST │            │
        │         │                     │            │
        │         │  GPIO17 ──► BUZZER  │            │
        │         │                     │            │
        │         │  GPIO41 ◄──► I2C SDA ◄──► SHT31/BME280
        │         │  GPIO42 ◄──► I2C SCL │    DS1307 RTC
        │         │                     │            │
        │         │  GPIO4  ◄── LDR     │            │
        │         │  GPIO5  ◄── VBAT    │  (opcional)
        │         │                     │            │
        │         │  USB D+/D- ◄──► USB-C            │
        │         └─────────────────────┘            │
        │                                            │
        └────────────────────────────────────────────┘
```

---

## 3. Bill of Materials (BOM)

### 3.1 Componentes Principales

| # | Componente | Especificación | Cantidad | Precio Est. | Notas |
|---|------------|----------------|----------|-------------|-------|
| 1 | ESP32-S3-WROOM-1 N16R8 | 16MB Flash, 8MB PSRAM | 1 | $4-6 | AliExpress/LCSC |
| 2 | Matriz LED WS2812B 32×8 | Panel flexible o rígido | 1 | $8-15 | 256 LEDs |
| 3 | SHT31 Module | I2C temp/humidity | 1 | $2-4 | Alternativa: BME280 |
| 4 | DS1307 RTC Module | I2C con batería CR2032 | 1 | $1-2 | Opcional pero recomendado |
| 5 | LDR GL5516 | Fotoresistor 5-10kΩ @10lux | 1 | $0.10 | Para auto-brillo |
| 6 | Buzzer pasivo | 5V, 2kHz, piezo | 1 | $0.30 | Para alarmas/RTTTL |
| 7 | Tactile buttons | 6×6mm, 4-pin | 4 | $0.40 | Momentary, NO |
| 8 | USB-C Connector | 16-pin SMD o breakout | 1 | $0.50-2 | Para alimentación y datos |

### 3.2 Componentes Pasivos

| # | Componente | Valor | Cantidad | Package | Notas |
|---|------------|-------|----------|---------|-------|
| 9 | Resistor | 330Ω | 1 | 0805/THT | Serie con DIN LED |
| 10 | Resistor | 10kΩ | 1 | 0805/THT | Pull-down LDR |
| 11 | Resistor | 4.7kΩ | 2 | 0805/THT | Pull-up I2C (opcional) |
| 12 | Resistor | 100kΩ | 2 | 0805/THT | Divisor batería (si aplica) |
| 13 | Capacitor | 100nF | 3 | 0805 | Bypass MCU |
| 14 | Capacitor | 10µF | 2 | 0805/1206 | Bypass poder |
| 15 | Capacitor | 1000µF 10V | 1 | Electrolítico | Bulk para LEDs |
| 16 | Capacitor | 100µF 10V | 1 | Electrolítico | Cerca de matriz LED |

### 3.3 Reguladores de Voltaje

| # | Componente | Especificación | Cantidad | Notas |
|---|------------|----------------|----------|-------|
| 17 | AMS1117-3.3 | 3.3V 1A LDO | 1 | Para ESP32-S3 |
| 18 | Capacitor | 22µF 10V | 2 | Input/output AMS1117 |

### 3.4 Opcionales

| # | Componente | Especificación | Cantidad | Notas |
|---|------------|----------------|----------|-------|
| 19 | LiPo Battery | 3.7V, 2000-4400mAh | 1 | Para portabilidad |
| 20 | TP4056 Module | Cargador LiPo USB-C | 1 | Con protección |
| 21 | MT3608 Module | Step-up 5V 2A | 1 | Boost para LEDs desde LiPo |
| 22 | Switch SPDT | On/Off | 1 | Corte de batería |

### 3.5 BOM Resumen de Costos

| Categoría | Costo Estimado |
|-----------|----------------|
| MCU + regulador | $5-7 |
| Matriz LED | $8-15 |
| Sensores | $3-6 |
| Pasivos + buttons | $2-3 |
| PCB (si aplica) | $2-5 (JLCPCB 5pcs) |
| **Total mínimo** | **$20-26** |
| **Con batería** | **$30-40** |

---

## 4. Pinout Completo ESP32-S3

### 4.1 Asignación de Pines del Proyecto

```
ESP32-S3-WROOM-1 N16R8 — Vista superior (antena arriba)
Numeración de pines del módulo (no del DevKit)

                          ┌──────────────────┐
                          │    [ANTENNA]     │
                          │                  │
                     GND ─┤1              44├─ GND
                      3V3 ─┤2              43├─ GPIO21
                      EN ─┤3              42├─ GPIO20 (USB D+) ⚠️
              GPIO4 (LDR) ─┤4              41├─ GPIO19 (USB D-) ⚠️
         GPIO5 (VBAT ADC) ─┤5              40├─ GPIO18
                   GPIO6 ─┤6              39├─ GPIO17 (BUZZER)
                   GPIO7 ─┤7              38├─ GPIO16
                  GPIO15 ─┤8              37├─ GPIO15
         GPIO9 (BTN_LEFT) ─┤9              36├─ GPIO14
       GPIO10 (BTN_SELECT) ─┤10             35├─ GPIO13
        GPIO11 (BTN_RIGHT) ─┤11             34├─ GPIO12 (BTN_RESET)
                   GPIO8 ─┤12             33├─ GPIO11
                  GPIO37 ─┤13             32├─ GPIO10
                  GPIO38 ─┤14             31├─ GPIO9
                  GPIO39 ─┤15             30├─ GPIO46 ⚠️ Strapping
                  GPIO40 ─┤16             29├─ GPIO45 ⚠️ Strapping
          GPIO41 (I2C_SDA) ─┤17             28├─ GPIO0  ⚠️ Strapping
          GPIO42 (I2C_SCL) ─┤18             27├─ GPIO35
                   GPIO2 ─┤19             26├─ GPIO36
                   GPIO1 ─┤20             25├─ GPIO37
                  GPIO44 ─┤21             24├─ GPIO38
                  GPIO43 ─┤22             23├─ GPIO39 (JTAG)
                      GND ─┤23             22├─ GPIO40
         GPIO48 (LED_DIN) ─┤24             21├─ GPIO41
                          │                  │
                          └──────────────────┘

⚠️ = No usar para I/O general (strapping, USB, PSRAM)
```

### 4.2 Tabla de Pines Usados

| GPIO | Función | Dirección | Tipo | Pull | Notas |
|------|---------|-----------|------|------|-------|
| **4** | LDR (luz ambiente) | Input | ADC1_CH3 | — | Divisor de voltaje |
| **5** | Batería ADC | Input | ADC1_CH4 | — | Divisor 1:2, opcional |
| **9** | Botón LEFT | Input | Digital | Pull-up | Activo LOW |
| **10** | Botón SELECT | Input | Digital | Pull-up | Activo LOW, wake |
| **11** | Botón RIGHT | Input | Digital | Pull-up | Activo LOW |
| **12** | Botón RESET | Input | Digital | Pull-up | Long-press 5s |
| **17** | Buzzer PWM | Output | LEDC | — | 2kHz típico |
| **41** | I2C SDA | I/O | I2C | Pull-up ext | 4.7kΩ a 3.3V |
| **42** | I2C SCL | I/O | I2C | Pull-up ext | 4.7kΩ a 3.3V |
| **48** | LED Matrix DIN | Output | RMT | — | 330Ω serie |

### 4.3 Pines NO Disponibles (Reservados)

| GPIO | Razón | Uso Interno |
|------|-------|-------------|
| 0 | Strapping | Boot mode select |
| 3 | Strapping | JTAG TMS |
| 19 | USB | USB_D- nativo |
| 20 | USB | USB_D+ nativo |
| 26-32 | PSRAM | Octal PSRAM (N16R8) |
| 33-34 | Flash | Quad SPI Flash |
| 43 | UART | U0TXD (console) |
| 44 | UART | U0RXD (console) |
| 45 | Strapping | VDD_SPI voltage |
| 46 | Strapping | Boot / ROM log |

### 4.4 Pines Libres para Expansión

| GPIO | ADC | Touch | RTC | Notas |
|------|-----|-------|-----|-------|
| 1 | ADC1_CH0 | T1 | ✓ | Disponible |
| 2 | ADC1_CH1 | T2 | ✓ | Disponible |
| 6 | ADC1_CH5 | T6 | ✓ | Disponible |
| 7 | ADC1_CH6 | T7 | ✓ | Disponible |
| 8 | — | T8 | ✓ | Disponible |
| 13 | ADC2_CH2 | T13 | ✓ | Disponible |
| 14 | ADC2_CH3 | T14 | ✓ | Disponible |
| 15 | ADC2_CH4 | — | ✓ | Disponible |
| 16 | ADC2_CH5 | — | ✓ | Disponible |
| 18 | ADC2_CH7 | — | ✓ | Disponible |
| 21 | — | — | ✓ | Disponible |

---

## 5. Esquemáticos por Subsistema

### 5.1 Alimentación Principal

```
                    ┌─────────────────────────────────────────────┐
                    │              POWER SUPPLY                    │
                    │                                              │
   USB-C     ┌──────┴──────┐                                       │
   VBUS ────►│             │                                       │
   5V        │  Protección │        5V Rail                        │
             │  ESD + Fuse │───────────┬─────────────────► LED Matrix VCC
   GND ─────►│             │           │                           │
             └──────┬──────┘           │    ┌───────────┐          │
                    │                  │    │ AMS1117   │          │
                    │                  ├───►│   3.3V    │───► ESP32-S3 3V3
                    │                  │    │           │          │
                    │              C1  │    └─────┬─────┘          │
                    │            100µF ┴         │                 │
                    │                  │     C2  ┴  22µF           │
                    │                  │         │                 │
                    └──────────────────┴─────────┴────────► GND    │
                                                                   │
                    └─────────────────────────────────────────────┘

Componentes:
- C1: 100µF/10V electrolítico (input bulk)
- C2: 22µF/10V cerámico o tantalio (output AMS1117)
- Fuse: Polyfuse 3A resettable (opcional pero recomendado)
- ESD: TVS diode SMBJ5.0A o similar
```

### 5.2 Matriz LED WS2812B

```
                    ┌────────────────────────────────────────────────┐
                    │           LED MATRIX 32×8 WS2812B              │
                    │                                                │
    ESP32-S3        │     R1                                         │
    GPIO48 ─────────┼────[330Ω]────► DIN ─┬─[LED]─┬─[LED]─┬─ ... ─┬─►│
                    │                     │       │       │       │  │
                    │                     └───────┴───────┴───────┘  │
                    │                           Serpentine wiring    │
                    │                                                │
       5V Rail ─────┼────────────────► VCC ──────────────────────────┤
                    │         │                                      │
                    │    C3   ┴  1000µF/10V                          │
                    │         │  (cerca del primer LED)              │
                    │         │                                      │
          GND ──────┼─────────┴──► GND ──────────────────────────────┤
                    │                                                │
                    └────────────────────────────────────────────────┘

Notas:
- R1 (330Ω): Limita corriente de pico, protege GPIO
- C3 (1000µF): Absorbe picos de corriente de los LEDs
- Distribuir capacitores 100µF cada 64 LEDs si es matriz larga
- Cable de datos lo más corto posible
- Separar cables de alimentación de datos
```

### 5.3 Botones

```
                    ┌────────────────────────────────────────────┐
                    │              BUTTONS (4×)                   │
                    │                                            │
                    │     BTN_LEFT (GPIO9)                       │
    ESP32-S3        │         │                                  │
    GPIO9 ──────────┼─────────┴────┬────[BTN]────► GND           │
    (internal       │              │                              │
     pull-up)       │         (no external pull-up needed)        │
                    │                                            │
                    │     BTN_SELECT (GPIO10) — also WAKE        │
    GPIO10 ─────────┼─────────┴────┬────[BTN]────► GND           │
                    │              │                              │
                    │                                            │
                    │     BTN_RIGHT (GPIO11)                     │
    GPIO11 ─────────┼─────────┴────┬────[BTN]────► GND           │
                    │              │                              │
                    │                                            │
                    │     BTN_RESET (GPIO12) — factory reset     │
    GPIO12 ─────────┼─────────┴────┬────[BTN]────► GND           │
                    │              │                              │
                    └────────────────────────────────────────────┘

Comportamiento:
- LEFT/RIGHT: Navegación, cambio de app
- SELECT: Confirmar, press corto
- SELECT (1s): Menú
- SELECT (2× rápido): Toggle power
- RESET (5s): Factory reset

Notas:
- ESP32-S3 tiene pull-ups internos ~45kΩ
- Para anti-bounce hardware: agregar 100nF en paralelo con botón
- Usar EasyButton library maneja debounce por software
```

### 5.4 Sensor de Luz (LDR)

```
                    ┌────────────────────────────────────────────┐
                    │              LIGHT SENSOR                   │
                    │                                            │
       3.3V ────────┼────────────┬───────────────────────────────┤
                    │            │                                │
                    │       ┌────┴────┐                           │
                    │       │   LDR   │  GL5516 or similar        │
                    │       │ 5k-500k │  (5kΩ@10lux, 500kΩ@dark)  │
                    │       └────┬────┘                           │
                    │            │                                │
    ESP32-S3        │            ├───────────────────────────────►│ GPIO4
    GPIO4 (ADC) ────┼────────────┤                                │ (ADC1_CH3)
                    │            │                                │
                    │       ┌────┴────┐                           │
                    │       │  10kΩ   │  R2 - Pull-down           │
                    │       └────┬────┘                           │
                    │            │                                │
          GND ──────┼────────────┴───────────────────────────────┤
                    │                                            │
                    └────────────────────────────────────────────┘

Cálculo:
- Luz fuerte: LDR ≈ 5kΩ → Vout = 3.3V × 10k/(5k+10k) = 2.2V
- Luz media: LDR ≈ 50kΩ → Vout = 3.3V × 10k/(50k+10k) = 0.55V  
- Oscuridad: LDR ≈ 500kΩ → Vout = 3.3V × 10k/(500k+10k) = 0.065V

ADC 12-bit: 0-4095 → 0-3.3V
El firmware aplica filtrado median+mean para estabilidad
```

### 5.5 Sensores I2C (Temperatura/Humedad + RTC)

```
                    ┌────────────────────────────────────────────────────┐
                    │                  I2C BUS                            │
                    │                                                     │
       3.3V ────────┼───────┬─────────────────┬─────────────────┬────────┤
                    │       │                 │                 │         │
                    │  R3   │ 4.7kΩ      R4   │ 4.7kΩ           │         │
                    │  ┌────┴────┐       ┌────┴────┐            │         │
                    │  └────┬────┘       └────┬────┘            │         │
                    │       │                 │                 │         │
    ESP32-S3        │       │                 │       ┌─────────┴───────┐ │
    GPIO41 (SDA) ───┼───────┴─────────────────┼──────►│ SHT31 / BME280  │ │
                    │                         │       │   Addr: 0x44    │ │
    GPIO42 (SCL) ───┼─────────────────────────┴──────►│   or 0x76/0x77  │ │
                    │                                 └─────────┬───────┘ │
                    │                                           │         │
                    │                                 ┌─────────┴───────┐ │
                    │                        ────────►│ DS1307 RTC      │ │
                    │                                 │   Addr: 0x68    │ │
                    │                        ────────►│   + CR2032      │ │
                    │                                 └─────────┬───────┘ │
                    │                                           │         │
          GND ──────┼───────────────────────────────────────────┴────────┤
                    │                                                     │
                    └────────────────────────────────────────────────────┘

Direcciones I2C usadas:
- SHT31: 0x44 (default) o 0x45
- BME280: 0x76 (SDO→GND) o 0x77 (SDO→VCC)
- BMP280: 0x76 o 0x77
- HTU21D: 0x40
- DS1307: 0x68

Auto-detección:
El firmware escanea automáticamente y usa el primer sensor encontrado.
Prioridad: BME280 > BMP280 > SHT31 > HTU21D

RTC DS1307:
- Mantiene hora sin WiFi
- Batería CR2032 dura ~10 años
- Cristal 32.768kHz integrado en módulo
```

### 5.6 Buzzer

```
                    ┌────────────────────────────────────────────┐
                    │              BUZZER (Passive)               │
                    │                                            │
    ESP32-S3        │                                            │
    GPIO17 (PWM) ───┼───────────────────► (+) ┌──────────┐       │
                    │                         │  BUZZER  │       │
          GND ──────┼───────────────────► (-) │ Passive  │       │
                    │                         │  5V/12mm │       │
                    │                         └──────────┘       │
                    │                                            │
                    └────────────────────────────────────────────┘

Alternativa con transistor (para buzzer más potente):

    GPIO17 ────[1kΩ]────┬────► Base (NPN 2N2222)
                        │           │
                       [10kΩ]      Collector ◄──── Buzzer (+)
                        │           │
                        ▼          Emitter ──────► GND
                       GND                    
                                   Buzzer (-) ◄─── 5V

Notas:
- Buzzer pasivo: requiere PWM para generar tono
- Frecuencia típica: 2kHz-4kHz
- El firmware usa LEDC para PWM (8-bit resolución)
- Soporta melodías RTTTL
```

### 5.7 Batería (Opcional)

```
                    ┌─────────────────────────────────────────────────────┐
                    │              BATTERY SYSTEM (Optional)               │
                    │                                                      │
                    │     ┌──────────────────────────────────────────────┐ │
                    │     │              TP4056 Module                    │ │
   USB-C ───────────┼────►│ IN+  ┌─────────────┐  BAT+ │───► LiPo 3.7V   │ │
   VBUS (5V)        │     │      │   TP4056    │       │     (+)         │ │
                    │     │ IN-  │   + DW01    │  BAT- │───► LiPo 3.7V   │ │
   GND ─────────────┼────►│      │  Protection │       │     (-)         │ │
                    │     │      └─────────────┘       │                  │ │
                    │     │              OUT+          │                  │ │
                    │     └───────────────┬────────────┘                  │ │
                    │                     │                               │ │
                    │                     ▼                               │ │
                    │           ┌─────────────────┐                       │ │
                    │           │    MT3608       │                       │ │
                    │           │   Step-Up 5V    │───────► 5V Rail       │ │
                    │           │     2A          │       (to LEDs)       │ │
                    │           └────────┬────────┘                       │ │
                    │                    │                                │ │
                    │                   GND                               │ │
                    │                                                     │ │
                    │     Battery Voltage Monitor:                        │ │
                    │                                                     │ │
      LiPo (+) ─────┼────[100kΩ]────┬────[100kΩ]───► GND                  │ │
                    │               │                                     │ │
      ESP32-S3      │               └─────────────────► GPIO5 (ADC)       │ │
                    │                                                     │ │
                    │     Divisor 1:2 → 4.2V max → 2.1V ADC (safe)        │ │
                    │                                                     │ │
                    └─────────────────────────────────────────────────────┘

Cálculo de autonomía:
- Batería 4400mAh @ 3.7V = 16.28Wh
- Consumo promedio: ~500mA @ 5V = 2.5W (50% brillo)
- Autonomía teórica: 16.28Wh / 2.5W = 6.5 horas

Notas:
- TP4056 con DW01 incluye protección de sobrecarga/descarga
- MT3608 eficiencia ~93% hasta 2A
- Agregar switch físico para cortar batería
- LED de carga en TP4056: rojo=cargando, azul/verde=completo
```

---

## 6. Cálculos de Potencia

### 6.1 Consumo por Componente

| Componente | Voltaje | Corriente Típica | Corriente Máxima | Potencia |
|------------|---------|------------------|------------------|----------|
| ESP32-S3 (WiFi activo) | 3.3V | 80-160mA | 350mA | 0.5W |
| ESP32-S3 (light sleep) | 3.3V | 2-3mA | — | 0.01W |
| LED Matrix (idle) | 5V | 20mA | — | 0.1W |
| LED Matrix (25% brillo) | 5V | 400mA | — | 2W |
| LED Matrix (50% brillo) | 5V | 800mA | — | 4W |
| LED Matrix (100% blanco) | 5V | — | 15A | 75W |
| SHT31 | 3.3V | 0.2mA | 1.5mA | <1mW |
| DS1307 | 3.3V | 0.2mA | 1.5mA | <1mW |
| LDR circuit | 3.3V | <0.5mA | — | <2mW |
| Buzzer | 3.3V | — | 30mA | 0.1W |

### 6.2 Escenarios de Uso

| Escenario | Brillo LED | Consumo Total @ 5V | Fuente Recomendada |
|-----------|------------|--------------------|--------------------|
| Standby/Clock | 10% | ~300mA | USB 500mA |
| Normal use | 30% | ~700mA | USB 1A |
| Bright display | 50% | ~1.2A | USB 2A |
| Notification flash | 100% burst | ~3A pico | 5V/3A PSU |
| Max white (nunca) | 100% | ~15A | No usar! |

### 6.3 Fuente de Alimentación Recomendada

**Mínimo:** USB-C PD 5V/2A (10W)  
**Recomendado:** USB-C PD 5V/3A (15W) o 5V/3A PSU dedicado

```
                    ┌──────────────────────────────────────┐
                    │     POWER BUDGET @ 50% brightness    │
                    │                                      │
                    │   ESP32-S3:        160mA × 3.3V =  0.5W
                    │   AMS1117 loss:    160mA × 1.7V =  0.3W
                    │   LED Matrix:      800mA × 5V   =  4.0W
                    │   Sensors:           5mA × 3.3V = 0.02W
                    │   ─────────────────────────────────────
                    │   TOTAL:                        ≈ 4.8W
                    │                                      │
                    │   Con margen 20%:               ≈ 5.8W
                    │   Fuente necesaria: 5V / 1.2A mínimo │
                    │                                      │
                    └──────────────────────────────────────┘
```

### 6.4 Disipación Térmica

| Componente | Disipación | Mitigación |
|------------|------------|------------|
| AMS1117 | (5V-3.3V) × 0.16A = 0.27W | Pad de cobre, sin heatsink necesario |
| ESP32-S3 | 0.5W típico | Ventilación pasiva suficiente |
| LEDs | Calor en matriz | Separar de electrónica |

---

## 7. Diseño de PCB

### 7.1 Opciones de Construcción

| Opción | Complejidad | Costo | Ideal para |
|--------|-------------|-------|------------|
| Protoboard/perfboard | Baja | $2-5 | Prototipo rápido |
| PCB custom (JLCPCB) | Media | $2-10 (5pcs) | Producción limpia |
| Módulo DevKitC-1 | Muy baja | $8-12 | Desarrollo inicial |

### 7.2 Consideraciones de Layout PCB

```
┌────────────────────────────────────────────────────────────────────┐
│                        PCB LAYOUT GUIDELINES                        │
│                                                                    │
│  ┌──────────────────────────────────────────────────────────────┐  │
│  │                    POWER SECTION (left)                       │  │
│  │  ┌─────────┐  ┌─────────┐                                    │  │
│  │  │ USB-C   │  │ AMS1117 │  Bulk capacitors here              │  │
│  │  │ + ESD   │  │  3.3V   │  Ground pour this area              │  │
│  │  └─────────┘  └─────────┘                                    │  │
│  └──────────────────────────────────────────────────────────────┘  │
│                              │                                      │
│                              ▼                                      │
│  ┌──────────────────────────────────────────────────────────────┐  │
│  │                    MCU SECTION (center)                       │  │
│  │                                                              │  │
│  │              ┌─────────────────────┐                         │  │
│  │              │   ESP32-S3-WROOM-1  │                         │  │
│  │              │       N16R8         │                         │  │
│  │              │                     │                         │  │
│  │              │   Keep antenna area │ ◄── NO copper/traces    │  │
│  │              │   clear!            │     under antenna        │  │
│  │              └─────────────────────┘                         │  │
│  │                                                              │  │
│  │  100nF bypass caps as close as possible to VCC pins          │  │
│  │                                                              │  │
│  └──────────────────────────────────────────────────────────────┘  │
│                              │                                      │
│                              ▼                                      │
│  ┌──────────────────────────────────────────────────────────────┐  │
│  │                 PERIPHERALS (right/bottom)                    │  │
│  │                                                              │  │
│  │  ┌─────┐  ┌─────┐  ┌─────┐  ┌─────┐                          │  │
│  │  │BTN_L│  │BTN_S│  │BTN_R│  │BTN_X│  Buttons accessible      │  │
│  │  └─────┘  └─────┘  └─────┘  └─────┘                          │  │
│  │                                                              │  │
│  │  LDR window │ I2C header │ LED connector │ Buzzer            │  │
│  │                                                              │  │
│  └──────────────────────────────────────────────────────────────┘  │
│                                                                    │
│  GENERAL RULES:                                                    │
│  - 2-layer PCB mínimo (4-layer ideal para EMI)                     │
│  - Ground pour en bottom layer                                      │
│  - Power traces: 1mm para 1A, 2mm para 3A                          │
│  - Signal traces: 0.25mm mínimo                                    │
│  - Via size: 0.3mm drill, 0.6mm pad                                │
│  - Keep WiFi antenna area clear of copper                          │
│  - LED data line short and away from power                         │
│                                                                    │
└────────────────────────────────────────────────────────────────────┘
```

### 7.3 Conectores Recomendados

| Función | Conector | Pitch | Notas |
|---------|----------|-------|-------|
| LED Matrix | JST-XH 3-pin | 2.54mm | VCC, GND, DIN |
| I2C Sensors | JST-SH 4-pin | 1.0mm | VCC, GND, SDA, SCL |
| Batería | JST-PH 2-pin | 2.0mm | Standard LiPo |
| Debug | Pin header 4-pin | 2.54mm | 3V3, GND, TX, RX |
| Buttons | Soldered o JST | — | Depende del case |

### 7.4 Archivos para Fabricación

Para JLCPCB/PCBWay, necesitas:
1. **Gerber files** (generados desde KiCad/EasyEDA)
2. **BOM** (lista de componentes con designators)
3. **CPL** (pick and place file, si usas SMT assembly)

---

## 8. Ensamblaje y Cableado

### 8.1 Orden de Ensamblaje

```
PASO 1: Preparación
├── Verificar todos los componentes contra BOM
├── Preparar herramientas: soldador, flux, multímetro
└── Limpiar PCB/protoboard

PASO 2: Regulador de voltaje (probar primero)
├── Soldar AMS1117-3.3 + capacitores
├── Conectar 5V de prueba
├── Verificar: 3.3V ±5% en salida
└── Si falla: revisar orientación, cortos

PASO 3: ESP32-S3 (sin energizar aún)
├── Soldar módulo WROOM-1 o usar DevKit
├── Soldar capacitores bypass 100nF
├── NO conectar alimentación todavía
└── Inspeccionar soldaduras con lupa

PASO 4: Periféricos pasivos
├── Soldar resistores (LDR, LED serie)
├── Soldar capacitores restantes
├── Soldar conectores
└── Soldar botones

PASO 5: Test de continuidad
├── Verificar: 3.3V rail no corto a GND
├── Verificar: 5V rail no corto a GND  
├── Verificar: Todas las conexiones GPIO
└── Si hay cortos: buscar puentes de soldadura

PASO 6: Primer encendido (sin LEDs)
├── Conectar USB-C
├── Verificar: LED de power (si hay)
├── Verificar: 3.3V en pin 3V3 del ESP32
├── Verificar: USB detectado en PC
└── Si no arranca: revisar EN pin, strapping

PASO 7: Flash firmware de prueba
├── Conectar USB
├── pio run -e esp32s3 -t upload
├── Verificar: Serial output en monitor
└── Test básico de GPIO con blink

PASO 8: Conectar periféricos
├── Conectar sensores I2C (verificar dirección)
├── Conectar LDR
├── Conectar buzzer
├── Probar cada uno individualmente
└── Verificar en serial log

PASO 9: Conectar matriz LED (último)
├── Verificar polaridad 5V/GND
├── Conectar DIN al GPIO48 vía resistor
├── Encender con brillo bajo primero
├── Test de patrón de colores
└── Verificar: No hay LEDs muertos, colores correctos

PASO 10: Test completo del sistema
├── Todas las apps nativas
├── WiFi connection
├── Web UI access
├── Home Assistant integration
├── OTA update
└── Deep sleep / wake
```

### 8.2 Diagrama de Cableado Completo

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           COMPLETE WIRING DIAGRAM                            │
│                                                                             │
│                                                                             │
│    ┌─────────────┐                    ┌─────────────────────────────────┐   │
│    │   USB-C     │                    │       ESP32-S3 WROOM-1 N16R8    │   │
│    │   5V/3A     │                    │                                 │   │
│    │             │                    │                                 │   │
│    │  VBUS ──────┼──┬─────────────────┼──► VIN (if DevKit)              │   │
│    │             │  │                 │     or 5V via AMS1117 → 3V3    │   │
│    │  GND ───────┼──┼─────────────────┼──► GND                          │   │
│    │             │  │                 │                                 │   │
│    │  D+ ────────┼──┼─────────────────┼──► GPIO20 (USB_D+)              │   │
│    │  D- ────────┼──┼─────────────────┼──► GPIO19 (USB_D-)              │   │
│    └─────────────┘  │                 │                                 │   │
│                     │                 │                                 │   │
│    ┌─────────────┐  │                 │                                 │   │
│    │ LED Matrix  │  │                 │                                 │   │
│    │   32×8      │  │                 │                                 │   │
│    │             │  │    ┌───[330Ω]───┼──► GPIO48 ─────────► DIN        │   │
│    │  VCC ───────┼──┴────┤            │                                 │   │
│    │  GND ───────┼───────┴────────────┼──► GND ────────────► GND        │   │
│    │             │       [1000µF]     │                                 │   │
│    └─────────────┘                    │                                 │   │
│                                       │                                 │   │
│    ┌─────────────┐                    │                                 │   │
│    │  BUTTONS    │                    │                                 │   │
│    │             │                    │                                 │   │
│    │  LEFT ──────┼────────────────────┼──► GPIO9  ─────[BTN]──► GND     │   │
│    │  SELECT ────┼────────────────────┼──► GPIO10 ─────[BTN]──► GND     │   │
│    │  RIGHT ─────┼────────────────────┼──► GPIO11 ─────[BTN]──► GND     │   │
│    │  RESET ─────┼────────────────────┼──► GPIO12 ─────[BTN]──► GND     │   │
│    └─────────────┘                    │                                 │   │
│                                       │                                 │   │
│    ┌─────────────┐                    │                                 │   │
│    │    LDR      │                    │                                 │   │
│    │             │                    │                                 │   │
│    │  3.3V ──────┼──[LDR]──┬──────────┼──► GPIO4 (ADC)                  │   │
│    │             │         │          │                                 │   │
│    │  GND ───────┼─[10kΩ]──┘          │                                 │   │
│    └─────────────┘                    │                                 │   │
│                                       │                                 │   │
│    ┌─────────────┐                    │                                 │   │
│    │   I2C BUS   │                    │                                 │   │
│    │             │                    │                                 │   │
│    │  3.3V ──────┼──┬─[4.7kΩ]─┬───────┼──► GPIO41 (SDA) ◄──► Sensors    │   │
│    │             │  │         │       │                                 │   │
│    │             │  └─[4.7kΩ]─┼───────┼──► GPIO42 (SCL) ◄──► Sensors    │   │
│    │             │            │       │                                 │   │
│    │  GND ───────┼────────────┘       │                                 │   │
│    │             │                    │                                 │   │
│    │  SHT31 ─────┼──► SDA, SCL, VCC, GND (addr 0x44)                    │   │
│    │  DS1307 ────┼──► SDA, SCL, VCC, GND (addr 0x68)                    │   │
│    └─────────────┘                    │                                 │   │
│                                       │                                 │   │
│    ┌─────────────┐                    │                                 │   │
│    │   BUZZER    │                    │                                 │   │
│    │             │                    │                                 │   │
│    │  (+) ───────┼────────────────────┼──► GPIO17 (PWM)                 │   │
│    │  (-) ───────┼────────────────────┼──► GND                          │   │
│    └─────────────┘                    │                                 │   │
│                                       │                                 │   │
│                                       └─────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 8.3 Lista de Cables

| De | A | Cable | Longitud | Notas |
|----|---|-------|----------|-------|
| USB VBUS | 5V rail | AWG22 rojo | 5cm | Alimentación |
| USB GND | GND rail | AWG22 negro | 5cm | Alimentación |
| 5V rail | AMS1117 IN | AWG24 rojo | 2cm | — |
| AMS1117 OUT | ESP32 3V3 | AWG24 rojo | 2cm | — |
| GPIO48 | LED DIN | AWG26 señal | <10cm | Con R 330Ω |
| 5V rail | LED VCC | AWG20 rojo | 10-20cm | Grueso! |
| GND rail | LED GND | AWG20 negro | 10-20cm | Grueso! |
| GPIO41 | Sensor SDA | AWG28 | 5-10cm | Twisted con SCL |
| GPIO42 | Sensor SCL | AWG28 | 5-10cm | Twisted con SDA |
| Buttons | GPIOs | AWG28 | Variable | — |

---

## 9. Cambios en el Firmware

### 9.1 Archivos a Crear/Modificar

| Archivo | Acción | Descripción |
|---------|--------|-------------|
| `src/HardwarePins.h` | **Crear** | Abstracción de pines |
| `svitrix_s3_partition.csv` | **Crear** | Tabla de particiones 16MB |
| `platformio.ini` | Modificar | Agregar `[env:esp32s3]` |
| `src/PeripheryManager/PeripheryManager.cpp` | Modificar | Usar HardwarePins.h |
| `src/DisplayManager/DisplayManager.cpp` | Modificar | Usar HardwarePins.h |
| `src/PowerManager/PowerManager.cpp` | Modificar | Usar HardwarePins.h |

### 9.2 HardwarePins.h (Código Completo)

```cpp
#pragma once

#include <driver/gpio.h>

// ═══════════════════════════════════════════════════════════════════════════
// Hardware Pin Abstraction Layer
// Automatically selects correct pins based on ESP-IDF target
// ═══════════════════════════════════════════════════════════════════════════

#if defined(CONFIG_IDF_TARGET_ESP32S3)
// ───────────────────────────────────────────────────────────────────────────
// ESP32-S3 DIY Build (N16R8 or similar)
// ───────────────────────────────────────────────────────────────────────────
namespace HW {
    // LED Matrix (WS2812B 32×8)
    constexpr int kMatrixPin = 48;          // RMT compatible, 5V tolerant
    constexpr int kMatrixWidth = 32;
    constexpr int kMatrixHeight = 8;
    constexpr int kNumLeds = 256;

    // Navigation Buttons (active LOW, internal pull-up)
    constexpr int kButtonLeftPin   = 9;
    constexpr int kButtonSelectPin = 10;    // Also deep sleep wake
    constexpr int kButtonRightPin  = 11;
    constexpr int kButtonResetPin  = 12;    // Factory reset (5s hold)

    // Audio (passive piezo buzzer, PWM)
    constexpr int kBuzzerPin = 17;
    constexpr int kBuzzerChannel = 0;       // LEDC channel

    // I2C Bus (sensors: SHT31/BME280/DS1307)
    constexpr int kI2cSdaPin = 41;
    constexpr int kI2cSclPin = 42;
    constexpr uint32_t kI2cFreq = 400000;   // 400kHz Fast Mode

    // Analog Inputs (ADC1 only - ADC2 conflicts with WiFi)
    constexpr int kLdrPin = 4;              // ADC1_CH3, light sensor
    constexpr int kBatteryPin = 5;          // ADC1_CH4, battery voltage

    // Deep Sleep Wake
    constexpr gpio_num_t kWakeupPin = GPIO_NUM_10;

    // Hardware Feature Flags
    constexpr bool kHasBattery = false;     // Set true if battery circuit present
    constexpr bool kHasLdr = true;
    constexpr bool kHasPsram = true;
    constexpr bool kHasNativeUsb = true;

    // PSRAM settings (for N16R8)
    constexpr size_t kPsramSize = 8 * 1024 * 1024;  // 8MB
}

#elif defined(CONFIG_IDF_TARGET_ESP32)
// ───────────────────────────────────────────────────────────────────────────
// ESP32 Classic (Ulanzi TC001 / WROOM-32D)
// ───────────────────────────────────────────────────────────────────────────
namespace HW {
    constexpr int kMatrixPin = 32;
    constexpr int kMatrixWidth = 32;
    constexpr int kMatrixHeight = 8;
    constexpr int kNumLeds = 256;

    constexpr int kButtonLeftPin   = 26;
    constexpr int kButtonSelectPin = 27;
    constexpr int kButtonRightPin  = 14;
    constexpr int kButtonResetPin  = 13;

    constexpr int kBuzzerPin = 15;
    constexpr int kBuzzerChannel = 0;

    constexpr int kI2cSdaPin = 21;
    constexpr int kI2cSclPin = 22;
    constexpr uint32_t kI2cFreq = 400000;

    constexpr int kLdrPin = 35;
    constexpr int kBatteryPin = 34;

    constexpr gpio_num_t kWakeupPin = GPIO_NUM_27;

    constexpr bool kHasBattery = true;      // TC001 has battery
    constexpr bool kHasLdr = true;
    constexpr bool kHasPsram = false;
    constexpr bool kHasNativeUsb = false;

    constexpr size_t kPsramSize = 0;
}

#else
    #error "Unsupported target. Expected CONFIG_IDF_TARGET_ESP32 or CONFIG_IDF_TARGET_ESP32S3"
#endif

// ═══════════════════════════════════════════════════════════════════════════
// Common Definitions (shared across all platforms)
// ═══════════════════════════════════════════════════════════════════════════
namespace HW {
    // I2C Device Addresses
    constexpr uint8_t kSht31Addr   = 0x44;
    constexpr uint8_t kBme280Addr  = 0x76;  // or 0x77
    constexpr uint8_t kDs1307Addr  = 0x68;
    constexpr uint8_t kHtu21dAddr  = 0x40;

    // ADC Configuration
    constexpr int kAdcResolution = 12;      // 12-bit (0-4095)
    constexpr float kAdcRefVoltage = 3.3f;

    // Battery voltage divider (if kHasBattery)
    constexpr float kBatteryDividerRatio = 0.5f;  // 100k/100k
    constexpr float kBatteryMaxVoltage = 4.2f;
    constexpr float kBatteryMinVoltage = 3.0f;
}
```

### 9.3 platformio.ini Additions

```ini
; ═══════════════════════════════════════════════════════════════════════════
; ESP32-S3 DIY Build
; ═══════════════════════════════════════════════════════════════════════════
[env:esp32s3]
platform = espressif32@6.3.0
framework = arduino
board = esp32-s3-devkitc-1

; MCU Configuration
board_build.mcu = esp32s3
board_build.f_cpu = 240000000L

; Flash Configuration (16MB)
board_build.flash_mode = qio
board_build.flash_size = 16MB
board_build.partitions = svitrix_s3_partition.csv
board_build.filesystem = littlefs

; PSRAM Configuration (8MB Octal)
board_build.arduino.memory_type = qio_opi

; USB CDC Configuration (native USB)
build_flags = 
    ${env:ulanzi.build_flags}
    -DBOARD_HAS_PSRAM
    -DARDUINO_USB_MODE=1
    -DARDUINO_USB_CDC_ON_BOOT=1

; Serial/Upload
monitor_speed = 115200
upload_speed = 921600

; Dependencies (same as ulanzi)
lib_deps = ${env:ulanzi.lib_deps}
```

### 9.4 svitrix_s3_partition.csv

```csv
# Svitrix ESP32-S3 N16R8 Partition Table (16MB Flash)
# Name,     Type, SubType,  Offset,    Size,      Flags

# NVS for persistent settings
nvs,        data, nvs,      0x9000,    0x5000,

# OTA metadata
otadata,    data, ota,      0xe000,    0x2000,

# Application slot 0 (3MB)
app0,       app,  ota_0,    0x10000,   0x300000,

# Application slot 1 (3MB)  
app1,       app,  ota_1,    0x310000,  0x300000,

# LittleFS filesystem (~9.5MB for icons, custom apps, web UI)
spiffs,     data, spiffs,   0x610000,  0x9F0000,
```

---

## 10. Testing y Validación

### 10.1 Test Checklist

```
┌────────────────────────────────────────────────────────────────┐
│                    HARDWARE VALIDATION                          │
├────────────────────────────────────────────────────────────────┤
│                                                                │
│ POWER SYSTEM                                          [ ]      │
│ ├─ [ ] 5V rail: 4.9V - 5.1V                                   │
│ ├─ [ ] 3.3V rail: 3.25V - 3.35V                               │
│ ├─ [ ] No overcurrent/overheating                              │
│ └─ [ ] USB detection on PC                                     │
│                                                                │
│ ESP32-S3                                              [ ]      │
│ ├─ [ ] Boot successful (serial log)                            │
│ ├─ [ ] WiFi AP mode works                                      │
│ ├─ [ ] WiFi STA connection                                     │
│ ├─ [ ] USB CDC serial works                                    │
│ └─ [ ] PSRAM detected (if applicable)                          │
│                                                                │
│ LED MATRIX                                            [ ]      │
│ ├─ [ ] All 256 LEDs light up                                   │
│ ├─ [ ] No dead pixels                                          │
│ ├─ [ ] Colors correct (RGB order)                              │
│ ├─ [ ] Brightness control works                                │
│ └─ [ ] No flickering at 60 FPS                                 │
│                                                                │
│ BUTTONS                                               [ ]      │
│ ├─ [ ] LEFT: detected, no bounce                               │
│ ├─ [ ] SELECT: press, long-press, double-press                 │
│ ├─ [ ] RIGHT: detected, no bounce                              │
│ └─ [ ] RESET: 5s hold triggers factory reset                   │
│                                                                │
│ SENSORS                                               [ ]      │
│ ├─ [ ] I2C scan finds devices                                  │
│ ├─ [ ] Temperature reading sensible                            │
│ ├─ [ ] Humidity reading sensible                               │
│ ├─ [ ] LDR: varies with light                                  │
│ └─ [ ] Auto-brightness responds                                │
│                                                                │
│ AUDIO                                                 [ ]      │
│ ├─ [ ] Buzzer makes sound                                      │
│ ├─ [ ] Boot melody plays                                       │
│ └─ [ ] RTTTL tones work                                        │
│                                                                │
│ DEEP SLEEP (optional)                                 [ ]      │
│ ├─ [ ] Enters deep sleep                                       │
│ ├─ [ ] Timer wake works                                        │
│ └─ [ ] Button wake works                                       │
│                                                                │
└────────────────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────────────────┐
│                    FIRMWARE VALIDATION                          │
├────────────────────────────────────────────────────────────────┤
│                                                                │
│ BASIC FUNCTIONALITY                                   [ ]      │
│ ├─ [ ] Clock displays correctly                                │
│ ├─ [ ] Date displays correctly                                 │
│ ├─ [ ] App transitions work                                    │
│ └─ [ ] Settings persist after reboot                           │
│                                                                │
│ WEB UI                                                [ ]      │
│ ├─ [ ] mDNS works (http://svitrix.local)                       │
│ ├─ [ ] All pages load                                          │
│ ├─ [ ] Settings save correctly                                 │
│ └─ [ ] Custom apps work                                        │
│                                                                │
│ MQTT / HOME ASSISTANT                                 [ ]      │
│ ├─ [ ] MQTT connects                                           │
│ ├─ [ ] HA auto-discovery                                       │
│ ├─ [ ] Entities controllable                                   │
│ └─ [ ] Stats publishing                                        │
│                                                                │
│ OTA UPDATE                                            [ ]      │
│ ├─ [ ] OTA from web UI                                         │
│ └─ [ ] OTA from MQTT                                           │
│                                                                │
└────────────────────────────────────────────────────────────────┘
```

### 10.2 Comandos de Test

```bash
# Build for ESP32-S3
pio run -e esp32s3

# Flash
pio run -e esp32s3 -t upload

# Monitor serial
pio device monitor -e esp32s3

# Run native tests (shared code)
pio test -e native_test

# Check flash size
pio run -e esp32s3 -t size

# Erase flash completely (if issues)
pio run -e esp32s3 -t erase
```

---

## 11. Troubleshooting

### 11.1 Problemas Comunes

| Síntoma | Causa Probable | Solución |
|---------|----------------|----------|
| No arranca | EN pin flotante | Agregar pull-up 10kΩ a 3.3V |
| No arranca | GPIO0 a GND | Liberar GPIO0 (modo bootloader) |
| USB no detectado | GPIO19/20 mal conectados | Verificar D+/D- |
| LEDs no encienden | DIN sin resistor | Agregar 330Ω en serie |
| LEDs flickering | Capacitor faltante | Agregar 1000µF cerca de matriz |
| Colores incorrectos | Orden GRB vs RGB | Verificar tipo WS2812B |
| WiFi débil | Cobre bajo antena | PCB sin cobre en zona antena |
| I2C no detecta | Pull-ups faltantes | Agregar 4.7kΩ a 3.3V |
| ADC lee 0 | GPIO en ADC2 + WiFi | Usar solo ADC1 (GPIO1-10) |
| Crash en PSRAM | Memory_type incorrecto | Verificar qio_opi en platformio.ini |
| Buzzer silencioso | Buzzer activo vs pasivo | Usar buzzer pasivo con PWM |

### 11.2 Debug Serial Output

```cpp
// Habilitar debug verbose en platformio.ini:
build_flags = 
    ...
    -DCORE_DEBUG_LEVEL=5

// O en código:
Serial.setDebugOutput(true);
```

### 11.3 I2C Scanner

```cpp
#include <Wire.h>

void scanI2C() {
    Wire.begin(41, 42);  // SDA, SCL
    Serial.println("Scanning I2C...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.printf("Found device at 0x%02X\n", addr);
        }
    }
}
```

---

## 12. Plan de Trabajo

### Fase 1: Hardware Prototype (1-2 días)

| # | Tarea | Verificación |
|---|-------|--------------|
| 1.1 | Adquirir componentes | BOM completo |
| 1.2 | Ensamblar en protoboard | Continuidad OK |
| 1.3 | Test de voltajes | 5V/3.3V OK |
| 1.4 | Flash DevKit de prueba | Serial output |

### Fase 2: Firmware Adaptation (1 día)

| # | Tarea | Verificación |
|---|-------|--------------|
| 2.1 | Crear branch `feat/esp32s3-diy` | — |
| 2.2 | Crear `HardwarePins.h` | Compila ambos targets |
| 2.3 | Crear partition table | — |
| 2.4 | Agregar env a platformio.ini | Build OK |
| 2.5 | Refactorizar pin usage | No hardcoded pins |

### Fase 3: Integration Testing (1-2 días)

| # | Tarea | Verificación |
|---|-------|--------------|
| 3.1 | Test matriz LED | 256 LEDs OK |
| 3.2 | Test botones | Todas las funciones |
| 3.3 | Test sensores I2C | Lecturas correctas |
| 3.4 | Test LDR/auto-brightness | Responde a luz |
| 3.5 | Test buzzer | Melodías RTTTL |
| 3.6 | Test WiFi + Web UI | Acceso completo |
| 3.7 | Test MQTT/HA | Integración OK |
| 3.8 | Test OTA | Update funciona |

### Fase 4: Documentation & PCB (Opcional, 2-3 días)

| # | Tarea | Output |
|---|-------|--------|
| 4.1 | Diseñar PCB en KiCad/EasyEDA | Gerber files |
| 4.2 | Ordenar PCB (JLCPCB) | 5-10 unidades |
| 4.3 | Diseñar enclosure 3D | STL files |
| 4.4 | Documentar assembly final | Guía completa |

### Fase 5: PR & Release

| # | Tarea |
|---|-------|
| 5.1 | PR con soporte ESP32-S3 |
| 5.2 | Actualizar README |
| 5.3 | Agregar binarios pre-built |
| 5.4 | Release notes |

---

## Apéndice A: Proveedores Recomendados

| Componente | AliExpress | LCSC | Amazon |
|------------|------------|------|--------|
| ESP32-S3-WROOM-1 | $4-6 | $4 | $8-12 |
| WS2812B 32×8 Matrix | $8-15 | — | $15-25 |
| SHT31 Module | $2-4 | $3 | $6-10 |
| DS1307 Module | $1-2 | $1 | $5-8 |
| LDR GL5516 (10pcs) | $0.50 | $0.30 | $3 |
| Passive Buzzer (5pcs) | $1 | $0.50 | $4 |
| Tactile Buttons (50pcs) | $2 | $1 | $5 |
| AMS1117-3.3 (10pcs) | $1 | $0.50 | $5 |

---

## Apéndice B: Recursos y Referencias

- [ESP32-S3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf)
- [ESP32-S3 Technical Reference](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf)
- [WS2812B Datasheet](https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf)
- [FastLED Documentation](https://github.com/FastLED/FastLED/wiki)
- [PlatformIO ESP32-S3](https://docs.platformio.org/en/latest/boards/espressif32/esp32-s3-devkitc-1.html)

---

*Documento: Svitrix DIY ESP32-S3 Build Guide*  
*Versión: 2.0 (DIY Complete)*  
*Fecha: 2026-06-05*
