# SVITRIX Firmware — Análisis Completo del Proyecto

> Documento de análisis técnico para entender la arquitectura, metodología de desarrollo y patrones utilizados en el proyecto SVITRIX-XE1E.

---

## Tabla de Contenidos

1. [Resumen Ejecutivo](#1-resumen-ejecutivo)
2. [Arquitectura del Proyecto](#2-arquitectura-del-proyecto)
3. [Metodología de Desarrollo con Claude](#3-metodología-de-desarrollo-con-claude)
4. [Organización del Código](#4-organización-del-código)
5. [Estrategia de Testing](#5-estrategia-de-testing)
6. [Build y CI/CD](#6-build-y-cicd)
7. [Patrones de Documentación](#7-patrones-de-documentación)
8. [Módulos Principales](#8-módulos-principales)
9. [Decisiones Arquitectónicas Clave](#9-decisiones-arquitectónicas-clave)
10. [Métricas del Proyecto](#10-métricas-del-proyecto)
11. [Workflow de Desarrollo](#11-workflow-de-desarrollo)
12. [Conclusiones y Recomendaciones](#12-conclusiones-y-recomendaciones)
13. [Sugerencias de Mejora y Nuevas Funcionalidades](#13-sugerencias-de-mejora-y-nuevas-funcionalidades)
    - [A. Mejoras de Código y Arquitectura](#a-mejoras-de-código-y-arquitectura)
    - [B. Nuevas Funcionalidades de Hardware](#b-nuevas-funcionalidades-de-hardware)
    - [C. Nuevas Apps y Funcionalidades](#c-nuevas-apps-y-funcionalidades)
    - [D. Mejoras de UI/UX](#d-mejoras-de-uiux)
    - [E. Mejoras de Rendimiento](#e-mejoras-de-rendimiento)
    - [F. Integración y Conectividad](#f-integración-y-conectividad)
    - [G. Seguridad](#g-seguridad)
    - [H. Herramientas de Desarrollo](#h-herramientas-de-desarrollo)
    - [I. Priorización de Sugerencias](#i-priorización-de-sugerencias)
    - [J. Arquitectura Dual: Autónomo + Home Assistant](#j-arquitectura-dual-autónomo--home-assistant)

---

## 1. Resumen Ejecutivo

### ¿Qué es SVITRIX?

SVITRIX-XE1E es un **firmware profesional para ESP32** diseñado para el reloj Ulanzi TC001 (matriz LED 32×8 WS2812B). Lo que lo hace especial es su enfoque **AI-first**: está construido usando Claude Code como herramienta principal de desarrollo.

### Stack Tecnológico

| Componente | Tecnología |
|------------|------------|
| **MCU** | ESP32-WROOM-32D (dual Xtensa LX6, 240 MHz) |
| **Framework** | Arduino + PlatformIO |
| **Lenguaje** | C++17 |
| **LEDs** | FastLED + FastLED NeoMatrix |
| **Storage** | LittleFS (256 KB) |
| **Conectividad** | WiFi (AsyncWebServer), MQTT, mDNS, UDP/TCP |
| **Sensores** | I2C (SHT3x, BME280, BMP280, HTU21DF), LDR, ADC batería |
| **Frontend** | Preact + Vite + TypeScript (17.7 KB gzipped) |
| **RTOS** | FreeRTOS (nativo ESP32) |

### Filosofía de Diseño

1. **Desacoplamiento por interfaces** — 15 interfaces puras virtuales
2. **Servicios sin estado** — Lógica reutilizable 100% testeable
3. **Documentación para IA** — CLAUDE.md en cada módulo
4. **Automatización** — 13 skills personalizados para tareas comunes

---

## 2. Arquitectura del Proyecto

### Patrón de Desacoplamiento por Interfaces

El corazón del proyecto son **15 interfaces puras virtuales** que actúan como contratos entre módulos. Ningún módulo incluye directamente a otro; toda comunicación pasa por interfaces.

```
┌─────────────────┐     ┌──────────────────┐     ┌─────────────────┐
│ PeripheryManager│     │  DisplayManager  │     │   MQTTManager   │
│                 │     │                  │     │                 │
│ Implementa:     │     │ Implementa:      │     │ Implementa:     │
│ - IPeripheryProv│────▶│ - IDisplayControl│◀────│ - INotifier     │
│ - ISound        │     │ - IDisplayNav    │     │ - IButtonReporter│
└─────────────────┘     │ - IMatrixHost    │     └─────────────────┘
                        │ - IButtonHandler │
                        └──────────────────┘
```

### Tabla de Interfaces

| Interface | Proveedor | Consumidores |
|-----------|-----------|--------------|
| `IDisplayRenderer` | DisplayRenderer_ | UpdateManager, MenuManager, ServerManager |
| `IDisplayControl` | DisplayManager_ | MenuManager, ServerManager, MQTTManager |
| `IDisplayNavigation` | DisplayManager_ | MenuManager, ServerManager, MQTTManager, DataFetcher |
| `IDisplayNotifier` | NotificationManager_ | ServerManager, MQTTManager |
| `IMatrixHost` | DisplayManager_ | MatrixDisplayUi |
| `IButtonHandler` | DisplayManager_, MenuManager_ | PeripheryManager |
| `IPeripheryProvider` | PeripheryManager_ | DisplayManager, NotificationManager, MenuManager, MQTTManager |
| `ISound` | PeripheryManager_ | ServerManager, MQTTManager |
| `IPower` | PowerManager_ | ServerManager, MQTTManager |
| `IUpdater` | UpdateManager_ | ServerManager, MQTTManager, MenuManager |
| `IPixelCanvas` | NeoMatrixCanvas | Sistema de efectos |
| `IDisplayPolicy` | NightModePolicy | DisplayManager |
| `ITimeProvider` | RealTimeProvider | NightModePolicy |
| `INotifier` | MQTTManager_ | DisplayManager, NotificationManager |
| `IButtonReporter` | MQTTManager_, ServerManager_ | PeripheryManager |

### Inyección de Dependencias

Todos los módulos usan **setter injection con guards de assert**:

```cpp
// En el módulo
void setDisplay(IDisplayRenderer *r, IDisplayControl *c, IDisplayNavigation *n);
bool hasDisplay() const { return renderer_ && control_ && nav_; }

// En main.cpp (composición)
MenuManager.setDisplay(&DisplayRenderer, &DisplayManager, &DisplayManager);
assert(MenuManager.hasDisplay());  // Falla en compile-time si falta
```

### Composition Root (main.cpp)

Todo el cableado de los 9 singletons ocurre en `main.cpp` (~200 líneas):

```cpp
void setup() {
    // 1. Inicializar hardware
    PeripheryManager.setup();
    
    // 2. Crear módulos
    DisplayManager.setup();
    UpdateManager.setup();
    // ...
    
    // 3. Inyectar dependencias
    MenuManager.setDisplay(&DisplayRenderer, &DisplayManager, &DisplayManager);
    MenuManager.setPeriphery(&PeripheryManager);
    MQTTManager.setDisplay(&DisplayManager, &NotificationManager);
    // ...
    
    // 4. Verificar
    assert(MenuManager.hasDisplay());
    assert(MQTTManager.hasDisplay());
}
```

---

## 3. Metodología de Desarrollo con Claude

### Estructura del Directorio .claude/

```
.claude/
├── commands/           ← 13 skills personalizados
│   ├── add-effect.md
│   ├── add-ha-entity.md
│   ├── add-service.md
│   ├── build.md
│   ├── flash-size.md
│   ├── sync-docs.md
│   ├── update-docs.md
│   ├── pr.md
│   ├── delegate.md
│   ├── parallel.md
│   ├── release.md
│   ├── changelog.md
│   └── sync-public-docs.md
├── git-workflow.md     ← Flujo de trabajo Git
├── settings.json       ← Configuración Claude Code
└── weather-api-plan.md ← Ejemplo de planificación
```

### Skills Personalizados (Automatización)

Cada skill es una **receta opinionada** que automatiza tareas comunes:

| Skill | Uso | Qué Automatiza |
|-------|-----|----------------|
| `/add-effect` | Nuevo efecto visual | Scaffold: archivo, registro, test, docs |
| `/add-ha-entity` | Entidad MQTT con HA discovery | 3 archivos: entidad, callback, estado |
| `/add-service` | Extraer lógica a lib/services/ | Boilerplate: header, impl, test |
| `/build` | Pipeline CI completo | Tests → build → flash-size (secuencial) |
| `/flash-size` | Análisis de binario | Reporte: RAM/Flash, warnings |
| `/sync-docs` | Re-sync CLAUDE.md | Antes de PR: actualiza 15+ docs |
| `/update-docs` | Actualizar docs específicos | Cambio puntual |
| `/pr` | Crear GitHub PR | Branch → commit → PR estructurado |
| `/delegate` | Delegar feature a agente | Branch, código, tests, PR (paralelo) |
| `/parallel` | Dividir feature en worktrees | Desarrollo coordinado |
| `/release` | Workflow de release | Beta/RC/stable: tag, version, Release |
| `/changelog` | Generar changelog | Desde conventional commits |
| `/sync-public-docs` | Actualizar VitePress docs | Pre-release: docs/ ← código |

### Ejemplo de Skill: /add-effect

```markdown
# Agregar Nuevo Efecto

## Paso 1: Crear archivo
Crear `src/effects/<Categoría>Effects.cpp` con:
- Incluir IPixelCanvas
- Implementar función callback
- No usar FastLED directamente

## Paso 2: Registrar
En `EffectRegistry.cpp`:
- Agregar al array de efectos
- Incrementar kNumEffects

## Paso 3: Test
Crear `test/test_native/test_effects/test_<nombre>.cpp`:
- Usar MockPixelCanvas
- Verificar que se dibuja algo

## Paso 4: Documentar
Actualizar `src/effects/README.md`
```

### Git Workflow

**Modelo de Ramas:**
```
upstream (svitrix/svitrix-firmware)  ← Original (solo lectura)
    ↓ git pull upstream main
origin (XE1E/svitrix-firmware-XE1E)  ← Tu fork
    ↓ git push / pull
Local PC
```

**Convención de Commits (Conventional Commits v1.0.0):**
```
<tipo>(<scope>): <descripción>

[cuerpo — por qué, no qué]

[footer: Refs: #123 o BREAKING CHANGE: ...]
```

Tipos válidos: `feat`, `fix`, `refactor`, `perf`, `docs`, `test`, `chore`, `build`, `ci`, `style`

**Versionado (SemVer con pre-release):**
```
v0.2.0-beta.1 → v0.2.0-beta.2 → v0.2.0-rc.1 → v0.2.0
```

---

## 4. Organización del Código

### Estructura de src/ (87 archivos)

```
src/
├── main.cpp                         ← Composition root (todo el cableado)
├── Globals.{cpp,h}                  ← Store de config + persistencia
├── AppContent.h                     ← Datos compartidos de rendering
├── Functions.{cpp,h}                ← Helpers globales
├── NightModeWindow.h                ← Predicado de ventana nocturna
├── RealTimeProvider.h               ← Implementación ITimeProvider
├── timer.{cpp,h}                    ← Abstracciones de timer ESP32
│
├── DisplayManager/                  ← 9 archivos, 3 clases
│   ├── DisplayManager.h/cpp         ← Coordinador (IButtonHandler, IMatrixHost, etc.)
│   ├── DisplayRenderer.h/cpp        ← Motor de dibujo (IDisplayRenderer)
│   ├── NotificationManager.h/cpp    ← Cola de notificaciones (IDisplayNotifier)
│   └── CLAUDE.md                    ← Documentación completa
│
├── MatrixDisplayUi/                 ← Framework de apps (10 transiciones)
├── PeripheryManager/                ← Hardware: botones, sensores, buzzer
├── MQTTManager/                     ← MQTT + Home Assistant (25 entidades)
├── ServerManager/                   ← HTTP/WiFi (~35 endpoints)
├── MenuManager/                     ← Menú en pantalla
├── UpdateManager/                   ← OTA firmware
├── PowerManager/                    ← Deep sleep / wake
├── DataFetcher/                     ← Fuentes HTTP externas
│
├── Apps/                            ← Apps nativas + custom
│   ├── Apps_NativeApps.cpp          ← Time (7 modos), Date, Temp, Hum, Bat
│   ├── Apps_CustomApp.cpp           ← Pipeline de rendering (12 fases)
│   └── README.md
│
├── effects/                         ← 20 efectos visuales
│   ├── WaveEffects.h/cpp            ← 6 efectos de onda
│   ├── PatternEffects.h/cpp         ← 5 efectos de patrón
│   ├── ParticleEffects.h/cpp        ← 5 efectos de partículas
│   ├── GameEffects.h/cpp            ← 4 efectos de juego
│   ├── EffectRegistry.h/cpp         ← Registro y dispatch
│   ├── NeoMatrixCanvas.h            ← Adaptador IPixelCanvas
│   └── README.md
│
├── MelodyPlayer/                    ← Parser RTTTL, PWM async
├── Overlays/                        ← Overlays de clima/estado
├── policies/                        ← Políticas de display (NightModePolicy)
├── data/                            ← Assets estáticos (font, cert, icons)
└── contrib/                         ← Headers de terceros
```

### Estructura de lib/ (88 archivos)

```
lib/
├── interfaces/                      ← 15 interfaces puras virtuales
│   ├── src/
│   │   ├── IDisplayRenderer.h
│   │   ├── IDisplayControl.h
│   │   ├── IPixelCanvas.h
│   │   └── ... (12 más)
│   └── CLAUDE.md
│
├── services/                        ← 15 librerías sin estado (100% testeable)
│   ├── src/
│   │   ├── ColorUtils.h             ← Hex↔uint32, Kelvin→RGB, HSV
│   │   ├── TextUtils.h              ← Ancho de texto (UTF-8 aware)
│   │   ├── UnicodeFont.h            ← Lookup de glifos, rendering
│   │   ├── SensorCalc.h             ← % batería, brillo, calibración
│   │   ├── StatsBuilder.h           ← Telemetría → JSON
│   │   ├── MessageRouter.h          ← Topic MQTT → comando
│   │   ├── HADiscovery.h            ← Descriptores de entidades HA
│   │   ├── FormatStringValidator.h  ← Defensa CWE-134 (printf seguro)
│   │   └── ... (7 más)
│   └── CLAUDE.md
│
├── config/                          ← 13 structs de configuración
│   ├── src/ConfigTypes.h
│   └── CLAUDE.md
│
├── home-assistant-integration/      ← Fork de ArduinoHA (trimmed)
├── webserver/                       ← Wrapper de AsyncWebServer
└── TJpg_Decoder/                    ← Decodificador JPEG
```

### Estructura de test/ (30 test suites)

```
test/
├── mocks/                           ← Test doubles
│   ├── Arduino.h                    ← Mock de Arduino API
│   ├── FastLED.h                    ← Mock de FastLED
│   ├── MockPixelCanvas.h            ← IPixelCanvas para tests
│   └── ...
│
└── test_native/                     ← 30 suites de tests nativos
    ├── test_color/
    ├── test_effects/
    ├── test_format_validator/
    ├── test_ha_discovery/
    ├── test_message_router/
    ├── test_night_mode_policy/
    ├── test_unicode_font/
    └── ... (23 más)
```

---

## 5. Estrategia de Testing

### Tests Nativos (sin dispositivo)

- **Framework:** Unity (ambiente nativo de PlatformIO)
- **Cobertura:** `lib/services/` mantiene **100% de cobertura**
- **Mocks:** `test/mocks/` provee test doubles
- **Ejecución:** `pio test -e native_test`

### Patrones de Testing

**1. Testing de Servicios Puros:**
```cpp
// test_color/test_color.cpp
void test_hexToUint32() {
    TEST_ASSERT_EQUAL(0xFF0000, ColorUtils::hexToUint32("#FF0000"));
    TEST_ASSERT_EQUAL(0x00FF00, ColorUtils::hexToUint32("#00FF00"));
}
```

**2. Testing de Efectos (con MockPixelCanvas):**
```cpp
// test_effects/test_effects.cpp
void test_plasma_effect_draws_pixels() {
    MockPixelCanvas canvas;
    EffectState state = {.speed = 3, .palette = "Rainbow"};
    
    plasmaEffect(&canvas, state);
    
    TEST_ASSERT_TRUE(canvas.pixelsDrawn > 0);
}
```

**3. Testing de Contratos de Interface:**
```cpp
// test_idisplay/test_idisplay.cpp
void test_interface_methods_exist() {
    // Compila = pasa. Detecta cambios breaking en interfaces.
    IDisplayRenderer* renderer = nullptr;
    (void)renderer->drawText(0, 0, "test", 0xFFFFFF);
}
```

### Gaps Conocidos de Testing

- No hay tests de integración en dispositivo
- No hay tests automatizados de UI (web frontend)
- Endpoints MQTT/HTTP se testean manualmente

---

## 6. Build y CI/CD

### Configuración PlatformIO

**Ambientes:**
- `ulanzi` — Firmware ESP32 (default)
- `native_test` — Tests nativos C++

**Artefactos de Build:**
```
.pio/build/ulanzi/
├── firmware.bin      ← Firmware principal
├── littlefs.bin      ← SPA web + assets
└── partitions.bin    ← Tabla de particiones
```

**Scripts Pre-build:**
- `tools/build_web.py` — Compila SPA Preact, inyecta en LittleFS
- `tools/inject_version.py` — Inyecta versión desde git tag

### GitHub Actions Workflows

| Workflow | Trigger | Jobs |
|----------|---------|------|
| **ci.yml** | PR/push a main | lint → static-analysis → test → build |
| **main.yml** (Release) | Push tags `v*` | CI → build → update flasher → GitHub Release |
| **sync-upstream.yml** | Diario 6:00 UTC | Pull de upstream, manejar conflictos |
| **deploy-docs.yml** | Push a main (docs/) | Deploy VitePress a GitHub Pages |

### Pipeline CI

```
┌─────────┐   ┌─────────────────┐   ┌──────────┐   ┌─────────┐
│  Lint   │──▶│ Static Analysis │──▶│  Tests   │──▶│  Build  │
│(clang)  │   │   (cppcheck)    │   │ (Unity)  │   │(PlatformIO)
└─────────┘   └─────────────────┘   └──────────┘   └─────────┘
```

### Límites de Flash Size

| Uso | Estado |
|-----|--------|
| < 90% | OK |
| 90-95% | WARNING |
| > 95% | CRITICAL (optimizar) |

---

## 7. Patrones de Documentación

### Archivos CLAUDE.md (Índice para IA)

Toda la documentación está **optimizada para Claude Code**. Cada CLAUDE.md se carga automáticamente cuando lees archivos en ese directorio.

**Estructura del CLAUDE.md Raíz:**

1. **TL;DR** — Stack, entry points, patrones clave
2. **Build & Test** — Comandos rápidos
3. **Hardware Pinout** — Tabla de especificaciones
4. **Project Structure** — Árbol de directorios
5. **Architecture Rules** — Interface-first, singletons, effects
6. **Interface Wiring** — Tabla: proveedor → interface → consumidores
7. **Main Loop** — Secuencia de tick
8. **Service Consumption** — Qué módulos usan qué servicios
9. **Module Doc Map** — Links a todos los docs de subdirectorios
10. **Common Change Patterns** — Por tarea: "Nuevo efecto → toca X, Y, Z"
11. **Cross-module Impact Map** — Cuando cambias X, actualiza Y, Z
12. **Coding Conventions** — Build flags, colores, MQTT, LittleFS, JSON
13. **Testing** — Native-only, 100% coverage target
14. **User Preferences** — Conciso, verificar con build + tests
15. **Available Skills** — Lista de `/command` personalizados

**Plantilla CLAUDE.md por Módulo:**

1. **TL;DR** — Qué hace, qué posee, qué consume
2. **File Map** — LOC, propósito por archivo
3. **Interfaces** — Implementa, Consume, Despacha a
4. **Key Concepts** — State machine, pipeline, callbacks
5. **API Reference** — Métodos públicos con firmas
6. **Services Used** — Dependencias de `lib/services/`
7. **Tests** — Ubicación y qué cubren
8. **Known Issues / TODOs** — Problemas conocidos, workarounds

---

## 8. Módulos Principales

### Display Ecosystem (3 clases)

**DisplayManager_** (coordinador)
- Posee estado de rendering, brillo, power, app switching
- Implementa: IButtonHandler, IMatrixHost, IDisplayControl, IDisplayNavigation
- Registra políticas (NightModePolicy)

**DisplayRenderer_** (motor de dibujo)
- Implementa: IDisplayRenderer
- Texto: UTF-8 + UnicodeFont (binary search)
- Primitivas: pixels, líneas, rectángulos, círculos
- Imágenes: JPEG (TJpg_Decoder), GIF (GifPlayer)
- Gráficos: bar, line (auto-scaled 0-8)

**NotificationManager_** (cola)
- Implementa: IDisplayNotifier
- Deque de notificaciones, 3 indicadores RGB en esquinas

### MQTT y Home Assistant (25 entidades)

**MQTTManager_**
- Implementa: INotifier, IButtonReporter
- **25 entidades auto-discovery**: 4 lights, 2 selects, 4 buttons, 1 switch, 10-11 sensors, 3 binary sensors
- **20 topics de comando**: notify, custom, switch, apps, settings, power, sleep, indicators, audio, update, stats
- **7 callbacks ArduinoHA**: onButton, onSwitch, onSelect, onRGBColor, onState, onBrightness, onNumber

### HTTP y WiFi

**ServerManager_**
- Implementa: IButtonReporter
- **35+ endpoints REST**: device, display, apps, notifications, audio, settings, stats, system, data fetcher
- WiFi: modo AP, fallback multi-red, mDNS discovery
- UDP/TCP device discovery

### Framework de Apps

**MatrixDisplayUi** (10 transiciones)
- Estados: FIXED ↔ IN_TRANSITION
- 20 efectos de fondo
- Overlays de clima/estado
- 3 indicadores RGB con blink/fade

### Pipeline de Rendering de Custom Apps (12 fases)

```
1. Guard (notify flag)
2. Background + effect
3. Lazy icon loading
4. Placeholder {{key}} resolution
5. Icon rendering
6. Charts (bar/line)
7. Progress bar
8. Scroll animation (repeat logic)
9. Text rendering (fragments + solid)
10. Lifetime red border
11. Weather overlay
12. Final composite
```

---

## 9. Decisiones Arquitectónicas Clave

### 1. Desacoplamiento por Interfaces
> Cada módulo comunica vía interfaces puras virtuales. No hay `#include` directo entre módulos.

**Resultado:** Testeable, componible, bajo acoplamiento.

### 2. Display Manager de 3 Clases
> DisplayManager_ (coordinador) + DisplayRenderer_ (dibujo) + NotificationManager_ (cola)

**Resultado:** Separación de responsabilidades dentro de un subsistema complejo.

### 3. Servicios Sin Estado
> Toda lógica utilitaria extraída a `lib/services/`. Funciones puras sin side effects.

**Resultado:** 100% testeable en ambiente nativo.

### 4. Composition Root
> Todo el cableado en `main.cpp`. Verificación compile-time con asserts.

**Resultado:** Un solo lugar para ver todas las dependencias.

### 5. Abstracción IPixelCanvas
> Efectos nunca tocan FastLED directo. Todo va por IPixelCanvas.

**Resultado:** Testing de efectos sin hardware.

### 6. Configuración en 3 Capas
> Defaults compile-time ← NVS (Preferences) ← /dev.json (LittleFS)

**Resultado:** Usuarios avanzados pueden override sin recompilar.

### 7. Políticas de Display Pluggables
> NightModePolicy puede override brillo, color, auto-transition.

**Resultado:** Fácil agregar nuevas políticas sin modificar DisplayManager.

### 8. Paridad MQTT + HTTP
> Ambos exponen los mismos comandos. Routing via MessageRouter.

**Resultado:** Agregar endpoints sin duplicar lógica.

### 9. Home Assistant Auto-Discovery
> Fork de ArduinoHA encapsula toda la lógica HA.

**Resultado:** HA auto-crea UI. Sin configuración manual.

### 10. Desarrollo AI-First
> Skills auto-scaffold tareas comunes. CLAUDE.md estructura conocimiento para carga de contexto.

**Resultado:** Productividad maximizada con Claude Code.

---

## 10. Métricas del Proyecto

| Métrica | Valor | Notas |
|---------|-------|-------|
| **Archivos fuente** | 175 | 87 en src/, 88 en lib/ |
| **Test suites** | 30 | C++ nativo (Unity) |
| **Interfaces** | 15 | Puras virtuales, capa de desacoplamiento |
| **Servicios** | 15 | Librerías sin estado |
| **Structs de config** | 13 | Persistentes, 3 capas |
| **Efectos** | 20 | 6 wave, 5 pattern, 5 particle, 4 game |
| **Entidades HA** | 25 | 4 lights, 2 selects, 4 buttons, 1 switch, 10-11 sensors, 3 binary |
| **Topics MQTT (in)** | 20 | Sufijos de comando |
| **Endpoints REST** | 35+ | Device, display, apps, notifications, etc. |
| **Transiciones** | 10 | SLIDE, FADE, ZOOM, ROTATE, etc. |
| **Apps nativas** | 5 | Time (7 modos), Date, Temp, Hum, Bat |
| **Sensores I2C** | 4 | BME280, BMP280, HTU21DF, SHT31 (auto-detect) |
| **Botones** | 4 | Left, Right, Select, Reset |
| **Skills personalizados** | 13 | Recetas de automatización |
| **Archivos CLAUDE.md** | 17 | Raíz + 16 módulos |
| **Cobertura de tests** | 100% | Solo lib/services/; resto manual |
| **Tamaño firmware** | ~2.8 MB | ~75% de 3.8 MB partición |
| **Tamaño Web UI** | 17.7 KB | Gzipped |

---

## 11. Workflow de Desarrollo

### Agregar Nueva Feature

**Nuevo Efecto Visual:**
```bash
# 1. Leer docs
cat src/effects/README.md
cat lib/interfaces/CLAUDE.md

# 2. Implementar
# src/effects/<Categoria>Effects.cpp

# 3. Registrar
# EffectRegistry.cpp + incrementar kNumEffects

# 4. Test
# test/test_native/test_effects/

# 5. Build + test
pio test -e native_test && pio run -e ulanzi

# 6. Verificar flash size (< 90% OK)

# 7. Documentar
# src/effects/README.md + CLAUDE.md

# O usar skill:
/add-effect
```

**Nueva Entidad MQTT/HA:**
```bash
/add-ha-entity
# Cubre: tipo de entidad, callbacks, state updates
```

**Nuevo Campo de Config:**
```bash
# 1. lib/config/src/ConfigTypes.h (agregar al struct)
# 2. src/Globals.cpp (defaults + NVS load/save)
# 3. ServerManager API endpoint
# 4. web UI field
# 5. lib/config/CLAUDE.md

/sync-docs  # Antes de PR
```

### Checklist Antes de Commit

```bash
pio test -e native_test          # Tests pasan
pio run -e ulanzi                # Build exitoso
# Verificar flash size < 95%
clang-format -i src/**/*.{cpp,h} # Formatear
git commit -m "tipo(scope): descripción"
```

### Checklist Antes de PR

```bash
/sync-docs        # Actualizar todos los CLAUDE.md
/build            # CI completo: tests → build → flash-size
git push origin feature/mi-feature
/pr               # Crear GitHub PR
```

### Proceso de Release

```bash
git tag -a v0.X.0 -m "v0.X.0: descripción"
git push origin v0.X.0
# CI: test → build → update flasher → GitHub Release
```

---

## 12. Conclusiones y Recomendaciones

### Lo que Hace Bien el Proyecto

1. **Arquitectura rigurosa** — 15 interfaces enforce desacoplamiento
2. **Testing comprehensivo** — 30 test suites, 100% cobertura en services
3. **Documentación AI-friendly** — 17 CLAUDE.md con contexto auto-cargado
4. **Automatización** — 13 skills para tareas comunes
5. **CI/CD profesional** — lint → test → build → release
6. **Convenciones productivas** — Conventional Commits, SemVer, squash-merge

### Recomendaciones para Continuar

1. **Usar los skills existentes** — No reinventar, usar `/add-effect`, `/add-ha-entity`, etc.
2. **Mantener el patrón de interfaces** — Nunca agregar `#include` directo entre módulos
3. **Tests primero para services** — Mantener 100% cobertura en `lib/services/`
4. **Documentar en CLAUDE.md** — No en README.md genéricos
5. **Seguir Conventional Commits** — Para changelog automático
6. **Verificar flash size** — Antes de cada PR

### Próximos Pasos Sugeridos

1. Revisar los skills en `.claude/commands/` para entender la automatización
2. Leer `lib/interfaces/CLAUDE.md` para entender los contratos
3. Explorar `lib/services/` como ejemplo de código limpio y testeable
4. Practicar con `/build` y `/sync-docs` para familiarizarse con el workflow

---

---

## 13. Sugerencias de Mejora y Nuevas Funcionalidades

### A. Mejoras de Código y Arquitectura

#### A.1 Resolver Bypasses de Interfaces (Deuda Técnica)

Actualmente 16 archivos incluyen módulos directamente en lugar de usar interfaces:

| Archivo | Bypassa | Debería Usar |
|---------|---------|--------------|
| `Overlays.cpp` | DisplayManager, MenuManager, PeripheryManager, MQTTManager | Interfaces vía inyección |
| `Apps_internal.h` | DisplayManager | IDisplayRenderer |
| `Apps_CustomApp.cpp` | MQTTManager | INotifier |
| `MenuManager.cpp` | ServerManager | Nueva interface IServerConnectivity |

**Sugerencia:** Crear `IServerConnectivity` con método `isConnected()` para que MenuManager no dependa directamente de ServerManager.

#### A.2 Migrar a ESP-IDF Nativo (Opcional pero Potente)

Arduino abstrae mucho pero sacrifica control. Para maximizar el Ulanzi:

```cpp
// Actual (Arduino)
delay(10);

// Mejor (ESP-IDF)
vTaskDelay(pdMS_TO_TICKS(10));  // No bloquea otros tasks
```

**Beneficios:**
- Control fino de FreeRTOS tasks
- Mejor manejo de memoria
- APIs más eficientes (esp_wifi, esp_http_client)
- Watchdog configurable por task

#### A.3 Doble Buffer para Rendering

Actualmente se dibuja directo al buffer de LEDs, causando posible flickering.

```cpp
// Propuesta: IPixelCanvas con doble buffer
class DoubleBufferedCanvas : public IPixelCanvas {
    CRGB buffer[2][256];
    uint8_t activeBuffer = 0;
    
    void flip() { activeBuffer ^= 1; }
    CRGB* getDrawBuffer() { return buffer[activeBuffer ^ 1]; }
    CRGB* getDisplayBuffer() { return buffer[activeBuffer]; }
};
```

#### A.4 Async/Event-Driven Architecture

Convertir polling a eventos donde sea posible:

```cpp
// Actual
void loop() {
    ServerManager.tick();  // Polling
    MQTTManager.tick();
}

// Mejor: Event queue
void onMqttMessage(const char* topic, const char* payload) {
    eventQueue.push({EVENT_MQTT, topic, payload});
}

void loop() {
    Event e;
    while (eventQueue.pop(e)) {
        handleEvent(e);
    }
}
```

---

### B. Nuevas Funcionalidades de Hardware

#### B.1 Usar el RTC DS1307 (Actualmente Ignorado)

El Ulanzi tiene un RTC pero no se usa. Beneficio: hora precisa sin WiFi.

```cpp
// Nueva interface
class IRealTimeClock {
public:
    virtual DateTime now() = 0;
    virtual void adjust(const DateTime& dt) = 0;
    virtual bool isRunning() = 0;
};

// Implementación
class DS1307Clock : public IRealTimeClock {
    RTC_DS1307 rtc;
public:
    DateTime now() override { return rtc.now(); }
};
```

**Flujo sugerido:**
1. Al arrancar, sincronizar RTC con NTP (si hay WiFi)
2. Si no hay WiFi, usar hora del RTC
3. Re-sync periódico cuando hay conexión

#### B.2 Modo Offline Completo

Actualmente depende de WiFi para muchas funciones. Agregar:

```cpp
struct OfflineConfig {
    bool enabled;
    uint8_t defaultBrightness;
    char timeFormat[16];
    char dateFormat[16];
    bool showBattery;
    // Apps offline: Time, Date, Temp, Hum, Bat
};
```

**Features offline:**
- Alarmas locales (sin HA)
- Cronómetro / Timer
- Pomodoro
- Animaciones predefinidas

#### B.3 Sensor de Proximidad (Usando LDR)

El LDR puede detectar sombras de manos cercanas:

```cpp
class ProximityDetector {
    MovingAverage<10> baseline;
    
    bool detectWave() {
        int current = analogRead(LDR_PIN);
        int avg = baseline.average();
        // Caída súbita = mano pasando
        if (current < avg * 0.7) {
            return true;
        }
        baseline.add(current);
        return false;
    }
};
```

**Usos:**
- Gesto para cambiar app
- Gesto para silenciar alarma
- Wake on wave

#### B.4 Ahorro de Batería Inteligente

```cpp
class PowerOptimizer {
    enum Mode { FULL, ECO, SLEEP };
    
    Mode calculateMode() {
        if (batteryPercent < 20) return ECO;
        if (batteryPercent < 10) return SLEEP;
        if (hour >= 23 || hour < 6) return ECO;
        return FULL;
    }
    
    void applyMode(Mode m) {
        switch(m) {
            case ECO:
                setBrightness(brightness * 0.5);
                setRefreshRate(15);  // 15 FPS vs 30
                disableEffects();
                break;
            case SLEEP:
                showMinimalClock();  // Solo HH:MM, color rojo
                break;
        }
    }
};
```

#### B.5 Bluetooth (No Usado Actualmente)

El ESP32 tiene BT 4.2 + BLE. Posibles usos:

- **BLE Beacon:** Presencia de dispositivo (para automatización HA)
- **BLE Keyboard:** Recibir texto desde teléfono
- **BLE Audio:** Notificaciones de teléfono
- **Mesh Networking:** Múltiples Ulanzis sincronizados

```cpp
class BLENotificationReceiver {
    void onNotification(const char* app, const char* title, const char* text) {
        NotificationManager.push({
            .text = text,
            .icon = getIconForApp(app),
            .duration = 5
        });
    }
};
```

---

### C. Nuevas Apps y Funcionalidades

#### C.1 Apps Nativas Adicionales

| App | Descripción | Complejidad |
|-----|-------------|-------------|
| ~~**Stopwatch**~~ | ~~Cronómetro con lap times~~ | ~~Removido~~ |
| ~~**Timer**~~ | ~~Cuenta regresiva con alarma~~ | ~~Removido~~ |
| **Pomodoro** | 25/5 min work/break cycles | Media |
| **World Clock** | Múltiples zonas horarias | Baja |
| **Sunrise/Sunset** | Basado en ubicación | Media |
| **Moon Phase** | Fase lunar actual | Baja |
| **Calendar Events** | Próximos eventos de HA | Media |
| **System Monitor** | CPU/RAM/Disk del servidor | Baja |
| **Crypto Ticker** | Múltiples cryptos rotando | Media |
| **Stock Ticker** | Acciones en tiempo real | Media |
| **RSS Headlines** | Titulares de noticias | Media |
| **Spotify Now Playing** | Canción actual | Alta |

#### C.2 Modo Juego

El Ulanzi tiene 3 botones — suficiente para juegos simples:

```cpp
class SnakeGame : public IGameApp {
    Point snake[64];
    uint8_t length = 3;
    Direction dir = RIGHT;
    Point food;
    
    void onButton(Button b) {
        if (b == LEFT) turnLeft();
        if (b == RIGHT) turnRight();
        if (b == SELECT) pause();
    }
    
    void tick() {
        moveSnake();
        checkCollision();
        render();
    }
};
```

**Juegos posibles:**
- Snake
- Pong (1 jugador vs CPU)
- Breakout
- Tetris (simplificado)
- Simon Says (memoria)
- Flappy Bird

#### C.3 Modo Reloj de Arena

Efecto visual de arena cayendo al girar el dispositivo:

```cpp
class HourglassMode {
    Particle sand[50];
    
    void onAccelerometerChange(float pitch) {
        // Simular gravedad según inclinación
        for (auto& p : sand) {
            p.vy += pitch * GRAVITY;
            p.update();
        }
    }
};
```

> Nota: Requeriría agregar acelerómetro externo (MPU6050) o usar detección por LDR.

#### C.4 Visualizador de Audio

Usando el micrófono del ESP32 o entrada de línea:

```cpp
class AudioVisualizer {
    uint8_t bands[8];  // 8 bandas de frecuencia
    
    void analyze(int16_t* samples, size_t count) {
        // FFT simple o aproximación
        computeFFT(samples, bands);
    }
    
    void render(IPixelCanvas* canvas) {
        for (int x = 0; x < 32; x += 4) {
            int band = x / 4;
            int height = map(bands[band], 0, 255, 0, 8);
            for (int y = 7; y >= 8 - height; y--) {
                canvas->drawPixel(x, y, getColor(height));
            }
        }
    }
};
```

---

### D. Mejoras de UI/UX

#### D.1 Wizard de Primera Configuración

```
┌─────────────────────────────────┐
│  WELCOME TO SVITRIX             │
│  ─────────────────              │
│  1. Select WiFi    [▶]         │
│  2. Set timezone   [ ]         │
│  3. Choose apps    [ ]         │
│  4. Done!          [ ]         │
└─────────────────────────────────┘
```

#### D.2 Temas de Color Predefinidos

```cpp
struct ColorTheme {
    uint32_t primary;
    uint32_t secondary;
    uint32_t accent;
    uint32_t background;
    const char* name;
};

const ColorTheme themes[] = {
    {0xFF0000, 0xFF5500, 0xFFAA00, 0x000000, "Fire"},
    {0x00FF00, 0x00FF55, 0x00FFAA, 0x000000, "Matrix"},
    {0x0000FF, 0x5500FF, 0xAA00FF, 0x000000, "Cyberpunk"},
    {0xFFFFFF, 0xCCCCCC, 0x888888, 0x000000, "Minimal"},
    // ...
};
```

#### D.3 App Drawer con Preview

En lugar de solo listar apps, mostrar preview en vivo:

```
┌─────────────────────────────────┐
│ [◀] Time         [preview] [▶] │
│     Date         [preview]     │
│     Weather      [preview]     │
│     Bitcoin      [preview]     │
└─────────────────────────────────┘
```

#### D.4 Notificaciones Contextuales

Diferentes estilos según tipo:

```cpp
enum NotificationType {
    INFO,      // Azul, sin sonido
    SUCCESS,   // Verde, ding
    WARNING,   // Amarillo, beep
    ERROR,     // Rojo, alarma
    URGENT     // Parpadeo, hold until dismissed
};
```

---

### E. Mejoras de Rendimiento

#### E.1 Caché de Iconos en PSRAM

Si hay PSRAM disponible (algunos ESP32):

```cpp
class IconCache {
    std::unordered_map<uint32_t, CRGB*> cache;
    
    CRGB* getIcon(uint32_t id) {
        if (cache.count(id)) return cache[id];
        
        CRGB* icon = (CRGB*)ps_malloc(64 * sizeof(CRGB));
        loadIconFromFlash(id, icon);
        cache[id] = icon;
        return icon;
    }
};
```

#### E.2 Compresión de GIFs

Los GIFs ocupan mucho espacio. Implementar RLE o LZ4:

```cpp
// Comprimir al guardar
size_t compressGif(const uint8_t* raw, size_t size, uint8_t* out) {
    return lz4_compress(raw, size, out);
}

// Descomprimir al cargar (streaming)
class LZ4GifPlayer {
    LZ4StreamDecode stream;
    void nextFrame() { stream.decode(frameBuffer); }
};
```

#### E.3 Render Dirty Regions

Solo redibujar lo que cambió:

```cpp
class DirtyRegionTracker {
    bool dirty[32][8];
    
    void markDirty(int x, int y, int w, int h) {
        for (int i = x; i < x + w; i++)
            for (int j = y; j < y + h; j++)
                dirty[i][j] = true;
    }
    
    void render(IPixelCanvas* canvas) {
        for (int x = 0; x < 32; x++)
            for (int y = 0; y < 8; y++)
                if (dirty[x][y]) {
                    canvas->drawPixel(x, y, buffer[x][y]);
                    dirty[x][y] = false;
                }
    }
};
```

---

### F. Integración y Conectividad

#### F.1 Webhook Endpoints

Recibir notificaciones de servicios externos:

```
POST /api/webhook/github
POST /api/webhook/stripe
POST /api/webhook/custom/:name
```

```cpp
void handleWebhook(AsyncWebServerRequest* req, const char* service) {
    String payload = req->arg("plain");
    
    if (strcmp(service, "github") == 0) {
        parseGitHubEvent(payload);
    } else if (strcmp(service, "stripe") == 0) {
        parseStripeEvent(payload);
    }
}
```

#### F.2 Multi-Device Sync

Múltiples Ulanzis sincronizados:

```cpp
class DeviceSync {
    std::vector<IPAddress> peers;
    
    void broadcastState() {
        StaticJsonDocument<256> doc;
        doc["currentApp"] = currentApp;
        doc["brightness"] = brightness;
        
        for (auto& peer : peers) {
            udp.beginPacket(peer, SYNC_PORT);
            serializeJson(doc, udp);
            udp.endPacket();
        }
    }
    
    void onPeerState(const JsonDocument& doc) {
        // Sincronizar estado
    }
};
```

#### F.3 Screen Mirroring (OBS/Streaming)

Exponer pantalla como video stream:

```cpp
// Endpoint: GET /api/stream
void handleStream(AsyncWebServerRequest* req) {
    AsyncResponseStream* response = req->beginResponseStream("multipart/x-mixed-replace");
    
    while (true) {
        // Capturar frame actual
        uint8_t jpeg[1024];
        size_t len = encodeFrameAsJpeg(leds, jpeg);
        
        // Enviar como MJPEG
        response->printf("--frame\r\n");
        response->printf("Content-Type: image/jpeg\r\n\r\n");
        response->write(jpeg, len);
        response->printf("\r\n");
        
        delay(33);  // ~30 FPS
    }
}
```

---

### G. Seguridad

#### G.1 HTTPS para API

Actualmente solo HTTP:

```cpp
// En platformio.ini
build_flags = -DASYNC_TCP_SSL_ENABLED=1

// En código
server.beginSecure("/cert.pem", "/key.pem");
```

#### G.2 Token-Based Auth

En lugar de basic auth:

```cpp
class TokenAuth {
    std::set<String> validTokens;
    
    bool validateRequest(AsyncWebServerRequest* req) {
        String token = req->header("Authorization");
        token.replace("Bearer ", "");
        return validTokens.count(token) > 0;
    }
    
    String generateToken() {
        // UUID o JWT
    }
};
```

#### G.3 Rate Limiting

Prevenir abuse:

```cpp
class RateLimiter {
    std::map<IPAddress, std::vector<uint32_t>> requests;
    
    bool allow(IPAddress ip) {
        auto& times = requests[ip];
        uint32_t now = millis();
        
        // Limpiar requests viejos (> 1 min)
        times.erase(
            std::remove_if(times.begin(), times.end(),
                [now](uint32_t t) { return now - t > 60000; }),
            times.end()
        );
        
        // Max 60 requests/min
        if (times.size() >= 60) return false;
        
        times.push_back(now);
        return true;
    }
};
```

---

### H. Herramientas de Desarrollo

#### H.1 Simulador Web

Probar firmware sin hardware:

```typescript
// web/src/simulator/Simulator.tsx
class MatrixSimulator {
    private pixels: Uint32Array = new Uint32Array(256);
    
    // WebSocket para recibir frames del firmware (WASM o mock)
    connect(url: string) {
        this.ws = new WebSocket(url);
        this.ws.onmessage = (e) => {
            this.pixels = new Uint32Array(e.data);
            this.render();
        };
    }
    
    render() {
        const ctx = this.canvas.getContext('2d');
        for (let i = 0; i < 256; i++) {
            const x = i % 32, y = Math.floor(i / 32);
            ctx.fillStyle = `#${this.pixels[i].toString(16).padStart(6, '0')}`;
            ctx.fillRect(x * 10, y * 10, 10, 10);
        }
    }
}
```

#### H.2 CLI para Desarrollo

```bash
# svitrix-cli
svitrix flash                    # Flash firmware
svitrix upload-icons ./icons/    # Subir iconos
svitrix logs                     # Ver serial output
svitrix screenshot               # Capturar pantalla
svitrix send-notification "Test" # Enviar notificación
svitrix config get brightness    # Leer config
svitrix config set brightness 50 # Escribir config
```

#### H.3 GitHub Actions para PRs

Auto-comentar en PRs con análisis:

```yaml
- name: Flash Size Analysis
  run: |
    pio run -e ulanzi
    python tools/analyze_size.py > size_report.md
    
- name: Comment PR
  uses: actions/github-script@v6
  with:
    script: |
      const report = fs.readFileSync('size_report.md', 'utf8');
      github.rest.issues.createComment({
        issue_number: context.issue.number,
        body: report
      });
```

---

### J. Arquitectura Dual: Autónomo + Home Assistant

Esta es la dirección propuesta para el desarrollo: **dos modos de operación independientes pero complementarios**.

#### J.1 Visión General

```
┌─────────────────────────────────────────────────────────────────┐
│                        SVITRIX-XE1E                             │
├────────────────────────────┬────────────────────────────────────┤
│     MODO AUTÓNOMO          │      MODO HOME ASSISTANT          │
│     (Sin conexión)         │      (Integración completa)       │
├────────────────────────────┼────────────────────────────────────┤
│ • Reloj preciso (RTC)      │ • Custom Apps desde HA            │
│ • Alarmas locales          │ • Notificaciones push             │
│ • Timer / Cronómetro       │ • Sensores bidireccionales        │
│ • Temperatura/Humedad      │ • Automatizaciones                │
│ • Batería                  │ • Control remoto total            │
│ • Efectos decorativos      │ • Scenes / Scripts                │
│ • Juegos simples           │ • Media player info               │
│ • Calendario offline       │ • Calendario desde HA             │
│ • Clima (si hay WiFi)      │ • Estado de dispositivos          │
└────────────────────────────┴────────────────────────────────────┘
```

#### J.2 Cambios Arquitectónicos Necesarios

**1. Separar Dependencias de Conectividad**

```cpp
// Nueva interface para abstracción de modo
class IOperationMode {
public:
    virtual bool requiresNetwork() = 0;
    virtual void onNetworkAvailable() = 0;
    virtual void onNetworkLost() = 0;
    virtual void tick() = 0;
};

class AutonomousMode : public IOperationMode {
    bool requiresNetwork() override { return false; }
    void tick() override {
        // Solo apps locales: Time, Date, Temp, Hum, Bat, Alarms
    }
};

class HomeAssistantMode : public IOperationMode {
    bool requiresNetwork() override { return true; }
    void tick() override {
        // MQTT, Custom Apps, Notifications, etc.
    }
};
```

**2. Modularizar MQTTManager**

```cpp
// Actual: MQTTManager hace todo
// Propuesta: Separar responsabilidades

class IMQTTClient {
    virtual void publish(const char* topic, const char* payload) = 0;
    virtual void subscribe(const char* topic) = 0;
};

class IHomeAssistantBridge {
    virtual void registerEntity(const HAEntity& entity) = 0;
    virtual void updateState(const char* entityId, const char* state) = 0;
};

// MQTTManager solo maneja conexión
// HABridge maneja lógica de Home Assistant
// Pueden existir independientemente
```

**3. Sistema de Apps Modular**

```cpp
enum AppType {
    APP_LOCAL,      // No necesita red
    APP_NETWORK,    // Necesita WiFi (clima, data fetcher)
    APP_HA_ONLY     // Solo con Home Assistant
};

struct AppDefinition {
    const char* name;
    AppType type;
    AppCallback render;
    AppCallback onActivate;
    AppCallback onDeactivate;
};

// Registry de apps por tipo
std::vector<AppDefinition> localApps;    // Siempre disponibles
std::vector<AppDefinition> networkApps;  // Con WiFi
std::vector<AppDefinition> haApps;       // Solo con HA
```

#### J.3 Funcionalidades del Modo Autónomo

| Categoría | Funcionalidad | Estado Actual | Propuesta |
|-----------|---------------|---------------|-----------|
| **Tiempo** | Reloj | ✅ Funciona | Usar RTC DS1307 para precisión sin WiFi |
| **Tiempo** | Fecha | ✅ Funciona | OK |
| **Tiempo** | Alarmas | ❌ No existe | Crear sistema de alarmas local |
| **Tiempo** | Timer | ❌ No existe | App de cuenta regresiva |
| **Tiempo** | Cronómetro | ❌ No existe | App de stopwatch |
| **Tiempo** | Pomodoro | ❌ No existe | 25/5 min cycles |
| **Sensores** | Temperatura | ✅ Funciona | OK |
| **Sensores** | Humedad | ✅ Funciona | OK |
| **Sensores** | Batería | ✅ Funciona | Agregar alertas de batería baja |
| **Visual** | Efectos | ✅ Funciona | OK |
| **Visual** | Brightness auto | ✅ Funciona | OK |
| **Visual** | Night mode | ✅ Funciona | OK |
| **Juegos** | Snake | ❌ No existe | Crear mini-juegos |
| **Juegos** | Pong | ❌ No existe | |
| **Utilidad** | Nivel burbuja | ❌ No existe | Usando LDR como proxy |

**Implementación de Alarmas Locales:**

```cpp
struct Alarm {
    uint8_t hour;
    uint8_t minute;
    uint8_t daysOfWeek;  // Bitmask: 0b01111111 = todos los días
    bool enabled;
    bool repeat;
    char sound[32];
    char label[32];
};

class AlarmManager {
    static constexpr int MAX_ALARMS = 10;
    Alarm alarms[MAX_ALARMS];
    
    void check(const DateTime& now) {
        for (const auto& alarm : alarms) {
            if (!alarm.enabled) continue;
            if (alarm.hour != now.hour() || alarm.minute != now.minute()) continue;
            if (!(alarm.daysOfWeek & (1 << now.dayOfWeek()))) continue;
            
            triggerAlarm(alarm);
        }
    }
    
    void triggerAlarm(const Alarm& alarm) {
        // Mostrar pantalla de alarma
        DisplayManager.showAlarmScreen(alarm.label);
        // Reproducir sonido
        MelodyPlayer.play(alarm.sound);
        // Vibrar (si hay motor)
        // Esperar dismiss (botón central)
    }
};
```

**Persistencia Offline:**

```cpp
// Guardar alarmas en LittleFS
// /config/alarms.json
[
    {"hour": 7, "minute": 0, "days": 31, "sound": "alarm1", "label": "Despertar"},
    {"hour": 12, "minute": 0, "days": 127, "sound": "chime", "label": "Almuerzo"}
]
```

#### J.4 Funcionalidades del Modo Home Assistant

| Categoría | Funcionalidad | Estado Actual | Propuesta |
|-----------|---------------|---------------|-----------|
| **Entidades** | Sensores | ✅ 10+ sensores | OK |
| **Entidades** | Lights | ✅ 4 lights | OK |
| **Entidades** | Buttons | ✅ 4 buttons | OK |
| **Entidades** | Switches | ✅ 1 switch | Agregar más (effects, apps) |
| **Entidades** | Selects | ✅ 2 selects | Agregar más (themes, transitions) |
| **Custom Apps** | Crear apps | ✅ Funciona | OK |
| **Custom Apps** | Actualizar apps | ✅ Funciona | OK |
| **Custom Apps** | Placeholders MQTT | ✅ Funciona | OK |
| **Notificaciones** | Push | ✅ Funciona | OK |
| **Notificaciones** | Stack | ✅ Funciona | OK |
| **Notificaciones** | Hold | ✅ Funciona | OK |
| **Control** | Brightness | ✅ Funciona | OK |
| **Control** | Power | ✅ Funciona | OK |
| **Control** | Sleep | ✅ Funciona | OK |
| **Automatización** | Scenes | ❌ Parcial | Crear presets guardables |
| **Automatización** | Scripts | ❌ No existe | Secuencias de acciones |
| **Media** | Now Playing | ❌ No existe | Integrar con media_player |
| **Calendario** | Eventos | ❌ No existe | Sincronizar con calendar |

**Nuevas Entidades HA Propuestas:**

```cpp
// Selectores adicionales
HASelect effectSelect("effect", effectNames, kNumEffects);
HASelect themeSelect("theme", themeNames, kNumThemes);
HASelect transitionSelect("transition", transitionNames, 10);

// Switches adicionales
HASwitch autoTransitionSwitch("auto_transition");
HASwitch soundSwitch("sound");
HASwitch nightModeSwitch("night_mode");

// Numbers
HANumber appDuration("app_duration", 1, 60);
HANumber scrollSpeed("scroll_speed", 50, 200);

// Sensores adicionales
HASensor currentApp("current_app");
HASensor nextAlarm("next_alarm");
HASensor uptime("uptime_formatted");  // "2d 5h 30m"
```

**Integración con Media Player:**

```yaml
# Home Assistant automation
automation:
  - alias: "Mostrar canción actual en SVITRIX"
    trigger:
      platform: state
      entity_id: media_player.spotify
    action:
      service: mqtt.publish
      data:
        topic: "svitrix/custom/spotify"
        payload: >
          {
            "text": "{{ states.media_player.spotify.attributes.media_title }}",
            "icon": "spotify",
            "color": "#1DB954",
            "duration": 10
          }
```

#### J.5 Transición Entre Modos

```cpp
enum OperatingMode {
    MODE_AUTONOMOUS,     // Sin red
    MODE_WIFI_ONLY,      // WiFi pero sin MQTT
    MODE_HOME_ASSISTANT  // MQTT + HA completo
};

class ModeManager {
    OperatingMode currentMode = MODE_AUTONOMOUS;
    
    void evaluateMode() {
        if (!WiFi.isConnected()) {
            setMode(MODE_AUTONOMOUS);
        } else if (!MQTTManager.isConnected()) {
            setMode(MODE_WIFI_ONLY);
        } else {
            setMode(MODE_HOME_ASSISTANT);
        }
    }
    
    void setMode(OperatingMode mode) {
        if (mode == currentMode) return;
        
        // Notificar cambio
        onModeChange(currentMode, mode);
        currentMode = mode;
        
        // Ajustar apps disponibles
        updateAvailableApps();
    }
    
    void updateAvailableApps() {
        apps.clear();
        
        // Siempre disponibles
        apps.push_back(timeApp);
        apps.push_back(dateApp);
        apps.push_back(tempApp);
        apps.push_back(humApp);
        apps.push_back(batApp);
        apps.push_back(alarmApp);
        apps.push_back(timerApp);
        
        if (currentMode >= MODE_WIFI_ONLY) {
            apps.push_back(weatherApp);
            for (auto& df : dataFetcherApps) {
                apps.push_back(df);
            }
        }
        
        if (currentMode == MODE_HOME_ASSISTANT) {
            for (auto& ca : customApps) {
                apps.push_back(ca);
            }
        }
    }
};
```

#### J.6 UI de Configuración de Modo

**En pantalla (menú):**
```
MODE
├── AUTO     ← Detectar automáticamente
├── OFFLINE  ← Forzar modo autónomo
├── WIFI     ← Solo WiFi (sin MQTT)
└── HA       ← Home Assistant completo
```

**En Web UI:**
```
┌─────────────────────────────────────┐
│  Operating Mode                     │
│  ───────────────                    │
│  ○ Automatic (recommended)          │
│  ○ Standalone (no network)          │
│  ○ WiFi only (weather, data fetch)  │
│  ○ Home Assistant (full features)   │
│                                     │
│  [Save Mode]                        │
└─────────────────────────────────────┘
```

#### J.7 Roadmap de Implementación

**Fase 1: Modo Autónomo Básico (2-3 semanas)**
- [ ] Integrar RTC DS1307
- [ ] Sistema de alarmas (CRUD + trigger)
- [ ] Timer / Cronómetro apps
- [ ] Persistencia offline (alarms.json)
- [ ] Indicador de modo en pantalla

**Fase 2: Separación de Código (1-2 semanas)**
- [ ] Crear IOperationMode interface
- [ ] Refactorizar MQTTManager → IMQTTClient + IHABridge
- [ ] Sistema de apps por tipo
- [ ] ModeManager con transiciones

**Fase 3: Mejoras Home Assistant (2-3 semanas)**
- [ ] Nuevas entidades (selects, switches, numbers)
- [ ] Integración media_player
- [ ] Scenes/presets guardables
- [ ] Sincronización calendario

**Fase 4: Pulido (1 semana)**
- [ ] UI de configuración de modo
- [ ] Documentación actualizada
- [ ] Tests para cada modo
- [ ] Release con changelog

---

### I. Priorización de Sugerencias

#### Prioridad Crítica (Arquitectura Dual)

| Sugerencia | Esfuerzo | Impacto | Fase |
|------------|----------|---------|------|
| **Integrar RTC DS1307** | Bajo | Alto | Autónomo |
| **Sistema de alarmas locales** | Medio | Alto | Autónomo |
| **Timer / Cronómetro** | Bajo | Alto | Autónomo |
| **Separar MQTTManager** | Medio | Alto | Arquitectura |
| **ModeManager (transiciones)** | Medio | Alto | Arquitectura |
| **Apps por tipo (local/network/HA)** | Medio | Alto | Arquitectura |

#### Alta Prioridad (Quick Wins)

| Sugerencia | Esfuerzo | Impacto |
|------------|----------|---------|
| Resolver bypasses de interfaces | Medio | Alto |
| Indicador de batería baja | Bajo | Medio |
| Persistencia offline | Bajo | Alto |
| Nuevas entidades HA (selects, switches) | Bajo | Medio |

#### Media Prioridad (Features Valiosas)

| Sugerencia | Esfuerzo | Impacto |
|------------|----------|---------|
| Temas de color | Bajo | Medio |
| Doble buffer para rendering | Medio | Medio |
| BLE beacon (presencia) | Medio | Medio |
| Integración media_player | Medio | Medio |
| Scenes/presets guardables | Medio | Medio |

#### Baja Prioridad (Nice to Have)

| Sugerencia | Esfuerzo | Impacto |
|------------|----------|---------|
| Migrar a ESP-IDF | Alto | Medio |
| Visualizador de audio | Alto | Bajo |
| Simulador web | Alto | Medio |
| Juegos (Snake, Pong) | Medio | Bajo |
| Screen mirroring | Medio | Bajo |

---

> **Nota:** Este documento fue generado analizando la estructura del proyecto SVITRIX-XE1E. Para la versión más actualizada, consulta los archivos CLAUDE.md en el repositorio.
