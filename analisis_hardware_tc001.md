# Análisis Técnico: Ulanzi TC001 + SVITRIX-XE1E

Evaluación completa del hardware y propuestas para máximo aprovechamiento.

---

## 1. Especificaciones de Hardware

### MCU - ESP32-WROOM-32D

| Componente | Especificación | Uso Actual |
|------------|----------------|------------|
| CPU | Dual-core Xtensa LX6, 240 MHz | Un núcleo (Arduino loop) |
| RAM | 520 KB SRAM | ~40 KB mínimo libre requerido |
| Flash | 8 MB | 4 MB usados (partición dual OTA) |
| WiFi | 802.11 b/g/n, 2.4 GHz | STA + AP fallback |
| Bluetooth | BLE 4.2 + Classic | **NO USADO** |

### Matriz LED

| Parámetro | Valor |
|-----------|-------|
| Tipo | WS2812B-Mini |
| Resolución | 32 × 8 pixels (256 LEDs) |
| GPIO | 32 |
| Disposición | Serpentina (zigzag) |
| FPS actual | 30-60 fps |
| Consumo | ~15W máx (todos blancos 100%) |

### Sensores I2C (Bus: GPIO 21 SDA, GPIO 22 SCL)

| Sensor | Dirección | Función | Estado |
|--------|-----------|---------|--------|
| SHT3x | 0x44 | Temp + Humedad | Activo |
| DS1307 | 0x68 | RTC | **NO USADO** |
| BME280* | 0x76/0x77 | Temp + Hum + Presión | Auto-detectado |
| BMP280* | 0x76/0x77 | Temp + Presión | Auto-detectado |
| HTU21DF* | - | Temp + Humedad | Auto-detectado |

*Sensores opcionales, se detecta el primero disponible

### Botones

| Botón | GPIO | Funciones Implementadas |
|-------|------|------------------------|
| Izquierdo | 26 | Navegación, menú |
| Centro/Select | 27 | Confirmar, wake desde sleep |
| Derecho | 14 | Navegación, menú |
| Reset (oculto) | 13 | Factory reset (5s hold) |

### Periféricos Analógicos

| Componente | GPIO | Uso |
|------------|------|-----|
| LDR (GL5516) | 35 (ADC1_CH7) | Auto-brillo |
| Batería | 34 (ADC1_CH6) | Nivel carga (4400 mAh) |
| Buzzer | 15 (LEDC PWM) | RTTTL melodies |

### Conectividad USB

| Chip | Función |
|------|---------|
| CH340 | USB-Serial para programación y debug |

---

## 2. Capacidades del Firmware Actual

### Apps Nativas (9)

| App | Estado | Personalizable |
|-----|--------|----------------|
| Time | Activo | 8 modos (TMODE) |
| Date | Activo | 9 formatos |
| Temperature (indoor) | Activo | Color, duración |
| Humidity (indoor) | Activo | Color, duración |
| Battery | Activo | Color, duración |
| OutdoorTemp | Activo | Color, duración, icono dinámico |
| OutdoorHum | Activo | Color, duración |
| Pressure | Activo | Color, duración |
| AirQuality | Activo | Color dinámico por AQI |

### Efectos Visuales (20)

| Categoría | Efectos |
|-----------|---------|
| Wave (6) | Pacifica, Plasma, ColorWaves, PlasmaCloud, SwirlIn, SwirlOut |
| Pattern (5) | TheaterChase, Checkerboard, Fade, MovingLine, Radar |
| Particle (5) | TwinklingStars, Fireworks, Ripple, Matrix, Fire |
| Game (4) | Snake, PingPong, BrickBreaker, LookingEyes |

### Weather Overlays (6)

Rain, Drizzle, Storm, Snow, Thunder, Frost

### Conectividad

| Protocolo | Estado | Funciones |
|-----------|--------|-----------|
| WiFi STA | Activo | Multi-network (3 redes) |
| WiFi AP | Activo | Configuración inicial |
| HTTP API | Activo | 40+ endpoints |
| MQTT | Activo | 20 comandos, 25 entidades HA |
| Art-Net/DMX | Activo | 768 canales (256 LEDs × RGB) |
| mDNS | Activo | svitrix_XXXXX.local |
| NTP | Activo | Sincronización horaria |

### Audio

| Función | Estado |
|---------|--------|
| RTTTL melodies | Activo |
| Volumen ajustable | 0-30 |
| Async playback | Activo |
| Notificación sonora | Activo |

### Almacenamiento (LittleFS)

| Directorio | Contenido |
|------------|-----------|
| `/ICONS/` | GIFs animados LaMetric |
| `/CUSTOMAPPS/` | Apps custom persistidas |
| `/PALETTES/` | Paletas de color custom |
| `/DATAFETCHER/` | Fuentes de datos externas |
| `/web/` | SPA web interface |

---

## 3. Recursos NO Utilizados

### 3.1 Bluetooth (BLE + Classic)

**Estado:** Completamente inactivo

**Potencial:**
- Control desde app móvil sin WiFi
- Beacons para presencia/proximidad
- Audio streaming (A2DP) para anuncios
- Keyboard HID para input
- Mesh networking con otros dispositivos ESP32

**Complejidad:** Alta (requiere stack BLE completo)

### 3.2 RTC DS1307

**Estado:** Hardware presente, no usado en firmware

**Potencial:**
- Mantener hora sin WiFi/NTP
- Alarmas por hardware
- Timestamp para logs offline
- Wake desde deep sleep por RTC

**Complejidad:** Baja (librería RTClib existente)

### 3.3 Segundo Core CPU

**Estado:** Arduino corre en Core 1, Core 0 parcialmente usado por WiFi

**Potencial:**
- Renderizado de efectos en Core 0
- Audio processing dedicado
- HTTP requests sin bloquear display
- Background data fetching

**Complejidad:** Media (FreeRTOS tasks)

### 3.4 Deep Sleep Avanzado

**Estado:** Básico (timer + GPIO 27)

**Potencial:**
- Wake por múltiples GPIOs
- Wake por touch pins (si se agregan)
- ULP coprocessor para monitoreo en sleep
- Hibernate mode (consumo µA)

**Complejidad:** Media

### 3.5 Memoria Flash Adicional

**Estado:** 4 MB de 8 MB usados

**Potencial:**
- Más iconos/GIFs
- Logs persistentes
- Grabación de datos históricos
- Fonts adicionales
- Más paletas/efectos

**Complejidad:** Baja (solo cambiar particiones)

### 3.6 PWM Adicionales

**Estado:** Solo GPIO 15 (buzzer)

**Potencial:**
- Control de LEDs externos
- Servomotores para efectos físicos
- Vibración feedback

**Complejidad:** Baja

---

## 4. Funciones Subutilizadas

### 4.1 Art-Net/DMX

**Uso actual:** Básico, sin documentación prominente

**Mejoras:**
- Integración con software VJ (Resolume, MadMapper)
- Modo Art-Net sync con múltiples dispositivos
- Documentación/tutoriales

### 4.2 DataFetcher

**Uso actual:** Fuentes externas genéricas

**Mejoras:**
- Templates predefinidos (crypto, stocks, weather, sports)
- Parsing más robusto (JSONPath completo)
- Autenticación (API keys en headers)

### 4.3 Custom Apps

**Uso actual:** JSON estático desde smart home

**Mejoras:**
- Templates/macros para patrones comunes
- Scripting simple (condicionales, variables)
- Herencia de propiedades

### 4.4 Notificaciones

**Uso actual:** Básico

**Mejoras:**
- Prioridades (urgente sobrescribe)
- Agrupación (stack similar de misma fuente)
- Historial accesible

### 4.5 Indicadores (3 LEDs esquina)

**Uso actual:** Básico (on/off, color)

**Mejoras:**
- Patrones predefinidos (breathing, pulse)
- Asociación semántica (indicador 1 = nuevo mensaje, etc.)

---

## 5. Propuestas de Máximo Aprovechamiento

### Fase 1: Quick Wins (Bajo Esfuerzo, Alto Impacto)

#### 1.1 Activar RTC DS1307
```
Beneficio: Hora precisa sin WiFi
Esfuerzo: 1-2 días
Archivos: PeripheryManager, main.cpp
```

#### 1.2 Exponer Weather a HA
```
Beneficio: 5 sensores más en Home Assistant
Esfuerzo: 1 día
Archivos: MQTTManager (ya planificado en plan_mqtt.md)
```

#### 1.3 Expandir menú físico
```
Beneficio: Control sin app/web
Esfuerzo: 1-2 días
Archivos: MenuManager (ya planificado en plan_menu_pantalla.md)
```

#### 1.4 Usar más Flash (particiones)
```
Beneficio: Más iconos, fonts, logs
Esfuerzo: Medio día
Archivos: platformio.ini, partitions.csv
```

### Fase 2: Mejoras Medianas

#### 2.1 FreeRTOS Tasks para Efectos
```
Beneficio: Efectos más fluidos, sin bloqueo
Esfuerzo: 3-5 días
Cambios: Mover renderizado a Core 0
```

#### 2.2 DataFetcher Templates
```
Beneficio: Setup más fácil para usuarios
Esfuerzo: 2-3 días
Incluir: Crypto (CoinGecko), Weather, RSS
```

#### 2.3 Night Mode via MQTT
```
Beneficio: Automatización desde HA
Esfuerzo: 1 día
Archivos: MQTTManager, HADiscovery
```

#### 2.4 Notificaciones Avanzadas
```
Beneficio: Mejor UX para alertas
Esfuerzo: 2-3 días
Features: Prioridad, agrupación, historial
```

### Fase 3: Características Mayores

#### 3.1 Bluetooth BLE
```
Beneficio: Control sin WiFi, beacons
Esfuerzo: 2-4 semanas
Riesgo: Conflictos con WiFi, memoria
```

#### 3.2 ULP Coprocessor
```
Beneficio: Monitoreo en deep sleep (<10µA)
Esfuerzo: 1-2 semanas
Uso: Sensor de luz, botón wake
```

#### 3.3 App Móvil Companion
```
Beneficio: Setup más fácil, notificaciones push
Esfuerzo: 1-2 meses
Tecnología: Flutter/React Native + BLE
```

#### 3.4 Multi-Device Sync
```
Beneficio: Varios TC001 sincronizados
Esfuerzo: 2-3 semanas
Protocolo: ESP-NOW o MQTT sync
```

---

## 6. Limitaciones Físicas

| Limitación | Impacto | Workaround |
|------------|---------|------------|
| 32×8 resolución | Texto limitado ~6 chars | Scroll, abreviaciones |
| Sin micrófono | No hay audio reactivo | Externo vía MQTT |
| Sin speaker real | Solo buzzer piezo | RTTTL monofónico |
| Batería interna | No reemplazable | Usar conectado |
| Sin touch | Solo 3 botones | Menú jerárquico |
| WiFi 2.4 GHz only | Sin 5 GHz | OK para IoT |

---

## 7. Métricas de Rendimiento Actual

| Métrica | Valor Típico | Óptimo |
|---------|--------------|--------|
| FPS display | 30-60 | 60 |
| Heap libre | 80-120 KB | >40 KB |
| Latencia MQTT | 10-50 ms | <100 ms |
| Boot time | 3-5 s | <5 s |
| WiFi reconnect | 2-10 s | <5 s |
| Consumo idle | ~200 mA | - |
| Consumo max | ~800 mA | - |

---

## 8. Resumen de Prioridades

| # | Mejora | Impacto | Esfuerzo | ROI |
|---|--------|---------|----------|-----|
| 1 | Weather sensors HA | Alto | Bajo | Excelente |
| 2 | Menú expandido | Alto | Bajo | Excelente |
| 3 | Night mode MQTT | Medio | Bajo | Muy bueno |
| 4 | RTC DS1307 | Medio | Bajo | Muy bueno |
| 5 | Más flash storage | Medio | Muy bajo | Bueno |
| 6 | DataFetcher templates | Medio | Medio | Bueno |
| 7 | FreeRTOS tasks | Medio | Medio | Bueno |
| 8 | Notificaciones avanzadas | Medio | Medio | Bueno |
| 9 | Bluetooth BLE | Alto | Alto | Incierto |
| 10 | ULP coprocessor | Bajo | Alto | Bajo |

---

## 9. Conclusiones

### Fortalezas Actuales
- Hardware bien aprovechado para display y conectividad
- API HTTP/MQTT completa y funcional
- Weather integration ya implementada
- Efectos visuales diversos
- OTA updates funcionando

### Oportunidades Inmediatas
1. **MQTT Weather Sensors** - Ya tienes HA, esto es el paso lógico
2. **Menú expandido** - Control offline completo
3. **RTC** - Hardware presente, no usado

### Oportunidades a Mediano Plazo
1. **Bluetooth** - Control móvil nativo
2. **Multi-core** - Mejor rendimiento
3. **ULP** - Batería extendida

### Recomendación
Empezar con **Weather sensors para HA** (máximo impacto para tu setup actual), luego **menú físico expandido**, después evaluar **RTC** y **Bluetooth** según prioridades.
