# Home Assistant Integration — Roadmap de Mejoras

## Estado Actual (actualizado)

### Entidades HA Existentes (39 total)

| Tipo | Cantidad | Entidades |
|------|----------|-----------|
| **HALight** | 5 | Matrix (brillo+RGB), Indicator 1/2/3 (RGB), nightColor (RGB) |
| **HASelect** | 2 | BriMode (Manual/Auto), transEffect (14 transiciones) |
| **HAButton** | 6 | Dismiss, doUpdate, nextApp, prevApp, reboot, playSound |
| **HASwitch** | 4 | transition, nightMode, nightBlockTransition, soundEnabled |
| **HANumber** | 2 | nightBrightness (1-50), soundVolume (0-100) |
| **HASensor** | 16-17 | curApp, deviceId, temp, hum, lux, signal, version, ram, uptime, ipAddr, battery*, outdoorTemp, outdoorHum, pressure, aqi, weatherCond, uvIndex |
| **HABinarySensor** | 3 | btnleft, btnmid, btnright |

*battery solo en build ULANZI

### Comandos MQTT Soportados

| Topic | Acción |
|-------|--------|
| `/notify` | Enviar notificación |
| `/custom/#` | Custom apps |
| `/switch`, `/nextapp`, `/previousapp` | Navegación |
| `/settings` | Actualizar configuración |
| `/power`, `/sleep`, `/reboot` | Control de energía |
| `/indicator1-3`, `/moodlight` | Indicadores visuales |
| `/rtttl`, `/sound`, `/r2d2` | Audio |
| `/doupdate` | Actualizar firmware |

---

## Implementado

### Night Mode Controls ✅
- `nightModeSwitch` (HASwitch) — Habilitar/deshabilitar modo nocturno
- `nightBrightnessNum` (HANumber) — Brillo nocturno (1-50)
- `nightColorLight` (HALight) — Color del texto en modo nocturno (RGB)
- `nightBlockSwitch` (HASwitch) — Bloquear transiciones en modo nocturno

### UV Index Sensor ✅
- `uvIndex` (HASensor) — Índice UV actual desde WeatherAPI

### System Buttons ✅
- `rebootBtn` (HAButton) — Reiniciar dispositivo

### Estado Inicial en Reconexión ✅
- Al conectar/reconectar a MQTT, se publican los estados de todas las entidades interactivas

### Audio Controls ✅
- `soundEnabled` (HASwitch) — Habilitar/deshabilitar sonido
- `soundVolume` (HANumber) — Volumen del buzzer (0-100)
- `playSoundBtn` (HAButton) — Reproducir sonido de prueba

---

## Propuestas Pendientes

### Prioridad Alta

#### 1. ~~Night Mode Controls~~ ✅ IMPLEMENTADO

#### 2. ~~Sound/Audio Controls~~ ✅ IMPLEMENTADO

---

#### 2. Sound/Audio Controls
**Valor:** Integrar audio del reloj con automatizaciones HA.

| Entidad | Tipo | Descripción |
|---------|------|-------------|
| `soundEnabled` | HASwitch | Habilitar/deshabilitar sonido |
| `soundVolume` | HANumber | Volumen (0-100) |
| `playSound` | HAButton | Reproducir sonido de prueba |

**Casos de uso:**
- Silenciar reloj cuando "casa en modo película"
- Subir volumen durante el día, bajar de noche
- Confirmación auditiva en automatizaciones

---

#### 3. Native App Visibility Switches
**Valor:** Mostrar/ocultar apps nativas desde HA sin abrir la web UI.

| Entidad | Tipo | Descripción |
|---------|------|-------------|
| `showTime` | HASwitch | Mostrar app de hora |
| `showDate` | HASwitch | Mostrar app de fecha |
| `showTemp` | HASwitch | Mostrar temperatura interior |
| `showHum` | HASwitch | Mostrar humedad interior |
| `showBat` | HASwitch | Mostrar batería (ULANZI) |

**Casos de uso:**
- Ocultar batería cuando está conectado a corriente
- Mostrar solo hora cuando hay invitados
- Dashboard HA con toggles para cada app

---

### Prioridad Media

#### 4. Background Effect Select
**Valor:** Cambiar efecto de fondo desde HA.

| Entidad | Tipo | Descripción |
|---------|------|-------------|
| `backgroundEffect` | HASelect | Lista de efectos disponibles |

**Opciones:** None, Fade, Matrix, Plasma, Starfield, Noise, Fire, Fireflies, Pacifica, Lava, Ocean, Pride, Rainbow, ColorWaves, Sparkles

**Casos de uso:**
- Efecto festivo durante navidad
- Modo "fiesta" activado por HA
- Efecto relajante por la noche

---

#### 5. Display Timing Controls
**Valor:** Ajustar tiempos de display desde HA.

| Entidad | Tipo | Descripción |
|---------|------|-------------|
| `timePerApp` | HANumber | Duración por app (1-60s) |
| `scrollSpeed` | HANumber | Velocidad de scroll (50-200ms) |
| `timeDuration` | HANumber | Duración de app de hora |
| `dateDuration` | HANumber | Duración de app de fecha |

**Casos de uso:**
- Scroll más lento para personas mayores
- Apps más rápidas cuando hay mucho que mostrar

---

#### 6. Text Notification Service
**Valor:** Enviar texto simple sin JSON complejo.

| Entidad | Tipo | Descripción |
|---------|------|-------------|
| `notifyText` | HAText | Texto a mostrar como notificación |

**Implementación:** Al escribir en esta entidad, envía una notificación básica con el texto y color por defecto.

**Casos de uso:**
- Notificaciones rápidas desde HA UI
- Integración con TTS o alertas simples

---

#### 7. Native App Colors
**Valor:** Personalizar colores de apps nativas desde HA.

| Entidad | Tipo | Descripción |
|---------|------|-------------|
| `timeColor` | HALight | Color de la hora (RGB) |
| `dateColor` | HALight | Color de la fecha (RGB) |
| `tempColor` | HALight | Color de temperatura (RGB) |

**Casos de uso:**
- Hora roja cuando hay alerta de clima
- Temperatura azul cuando hace frío, naranja cuando calor
- Colores festivos en fechas especiales

---

### Prioridad Baja

#### 8. ~~System Buttons~~ ✅ PARCIAL (reboot implementado)
checkUpdate pendiente

---

#### 9. ~~UV Index Sensor~~ ✅ IMPLEMENTADO

---

#### 10. Alarm Indicator
**Valor:** Saber si hay alarmas activas desde HA.

| Entidad | Tipo | Descripción |
|---------|------|-------------|
| `alarmsActive` | HABinarySensor | true si hay alarmas configuradas |
| `nextAlarm` | HASensor | Hora de la próxima alarma (texto) |

---

#### 11. Weather App Visibility
**Valor:** Control granular de apps meteorológicas.

| Entidad | Tipo | Descripción |
|---------|------|-------------|
| `showOutdoorTemp` | HASwitch | Mostrar temperatura exterior |
| `showOutdoorHum` | HASwitch | Mostrar humedad exterior |
| `showPressure` | HASwitch | Mostrar presión atmosférica |
| `showAirQuality` | HASwitch | Mostrar índice de calidad de aire |

---

## Mejoras de Infraestructura

### A. Límite de Entidades ✅ RESUELTO
**Problema original:** HAMqtt tenía límite de 26 entidades.
**Solución:** Aumentado a 32 en constructor de HAMqtt. Actualmente tenemos 36 entidades.

### B. Sincronización Bidireccional
**Problema:** Algunos cambios desde web UI no se reflejan inmediatamente en HA.
**Solución:** `sendStats()` sincroniza periódicamente. Llamar explícitamente después de cambios críticos.

### C. Estado Inicial ✅ RESUELTO
**Problema original:** Al reconectar, algunas entidades no reportaban su estado.
**Solución:** En `onMqttConnected()`, se publican estados de: transition, BriMode, transEffect, Matrix (brillo/estado/color), nightMode, nightBrightness, nightColor, nightBlockTransition.

---

## Resumen de Entidades Propuestas

| Prioridad | Nuevas Entidades | Tipos |
|-----------|------------------|-------|
| **Alta** | 7 | 3 HASwitch, 2 HANumber, 1 HALight, 1 HAButton |
| **Media** | 8 | 1 HASelect, 4 HANumber, 1 HAText, 2 HALight |
| **Baja** | 8 | 2 HAButton, 2 HASensor, 1 HABinarySensor, 4 HASwitch |
| **Total** | **23** | — |

---

## Plan de Implementación Sugerido

### Fase 1 — Night Mode (1-2 días)
1. Agregar 4 entidades de night mode
2. Tests de callbacks
3. Documentar en MQTTManager/CLAUDE.md

### Fase 2 — Audio + App Visibility (1-2 días)
1. Agregar 3 entidades de audio
2. Agregar 5 switches de visibilidad
3. Verificar límite de entidades

### Fase 3 — Effects + Timing (1 día)
1. HASelect para background effect
2. HANumbers para timing

### Fase 4 — Polish (1 día)
1. UV sensor (trivial)
2. Botones de sistema
3. Sincronización bidireccional
4. Estado inicial en reconexión

---

## Notas Técnicas

### Patrón para agregar entidad

1. **HADiscovery.cpp** — agregar constantes y descriptor
2. **MQTTManager_internal.h** — declarar puntero extern
3. **MQTTManager.cpp** — definir puntero como nullptr
4. **MQTTManager_Discovery.cpp** — crear entidad en `setup()`
5. **MQTTManager_Callbacks.cpp** — handler si es interactiva
6. **MQTTManager_StateUpdates.cpp** — actualizar en `sendStats()` si es sensor
7. **Tests** — verificar en native_test

### Skill disponible
Usar `/add-ha-entity` para guía paso a paso de cada entidad.

---

## Decisiones Pendientes

1. **¿Agrupar colores?** — Un solo "color picker" vs múltiples HALight
2. **¿HAText para notificaciones?** — ArduinoHA no tiene HAText nativo, requiere implementación custom o usar MQTT directo
3. **¿Alarmas?** — Requiere primero implementar sistema de alarmas en el dispositivo
4. **¿Límite de memoria?** — Cada entidad HA consume ~200-400 bytes de heap
