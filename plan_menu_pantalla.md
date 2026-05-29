# Plan: Menú en Pantalla

Análisis y propuesta de mejoras para el menú on-screen accesible mediante botones físicos.

## Estado Actual

### Estructura del Menú (15 items)

| # | Menú | Función | Valores | Estado |
|---|------|---------|---------|--------|
| 1 | BRIGHT | Brillo manual/auto | 1-100% o AUTO | OK |
| 2 | COLOR | Color de texto global | 15 colores preset | OK |
| 3 | SWITCH | Auto-transición entre apps | ON/OFF | OK |
| 4 | T-SPEED | Velocidad de transición | 200-2000ms (pasos 100ms) | OK |
| 5 | APPTIME | Tiempo por app | 1-30s (pasos 1s) | OK |
| 6 | TIME | Formato de hora | 8 opciones strftime | OK |
| 7 | DATE | Formato de fecha | 9 opciones strftime | OK |
| 8 | WEEKDAY | Inicio de semana | MON/SUN | OK |
| 9 | TEMP | Unidad temperatura | °C/°F | OK |
| 10 | APPS | Toggle apps nativas | 9 apps (Time, Date, Temp, Hum, Bat + 4 weather) | OK |
| 11 | NIGHT | Toggle modo nocturno | ON/OFF | OK |
| 12 | INFO | Info del sistema (solo lectura) | IP, WIFI, VER, ID, RAM | OK |
| 13 | SOUND | Sonido activado | ON/OFF | OK |
| 14 | VOLUME | Volumen buzzer | 0-30 | OK |
| 15 | UPDATE | Actualización OTA | Trigger | OK |

### Navegación Actual

| Botón | En Menú Principal | En Submenú |
|-------|-------------------|------------|
| Derecho | Siguiente item | Aumentar valor |
| Izquierdo | Anterior item | Disminuir valor |
| Select (corto) | Entrar submenú | Toggle valor |
| Select (largo) | Salir del menú | Guardar + volver |

### Archivos Involucrados

| Archivo | Propósito |
|---------|-----------|
| `src/MenuManager/MenuManager.h` | Singleton, interfaz IButtonHandler |
| `src/MenuManager/MenuManager.cpp` | State machine, items, handlers |
| `src/MenuManager/CLAUDE.md` | Documentación AI |

---

## Problemas Detectados

### ~~1. Apps de Weather no accesibles~~ ✅ RESUELTO
- ~~El submenú APPS solo muestra 5 apps: Time, Date, Temp, Hum, Bat~~
- ~~Las 4 apps de weather (OutdoorTemp, OutdoorHum, Pressure, AirQuality) no se pueden activar/desactivar desde el menú~~
- ~~Código relevante: `appsCount = 5` hardcodeado~~
- **Solución:** Expandido a 9 apps (commit 38dfe15)

### ~~2. Night Mode no accesible~~ ✅ RESUELTO
- ~~El modo nocturno solo se puede configurar desde web UI~~
- ~~No hay forma de activarlo/desactivarlo rápidamente desde el dispositivo~~
- **Solución:** Nuevo menú NIGHT con toggle ON/OFF (commit 38dfe15)

### ~~3. Sin información del sistema~~ ✅ RESUELTO
- ~~No se puede ver IP, señal WiFi, versión de firmware desde el menú~~
- ~~Útil para troubleshooting sin acceso a web UI~~
- **Solución:** Nuevo menú INFO con 5 datos: IP, WIFI, VER, ID, RAM (commit 38dfe15)

### 4. Sin control de efectos
- Los 19 efectos de fondo solo se configuran vía API/web
- Útil tener acceso rápido desde el dispositivo

---

## Propuesta de Cambios

### Fase 1: Correcciones (Prioridad Alta) ✅ COMPLETADA

#### 1.1 Agregar Weather Apps al submenú APPS ✅
```
Cambio: Expandir appsCount de 5 a 9
Nuevos items:
- OutdoorTemp (icon_sunny)
- OutdoorHum (icon_53628)
- Pressure (icon_66892)
- AirQuality (icon_6622)

Estado: IMPLEMENTADO - commit 38dfe15
```

#### 1.2 Agregar menú NIGHT ✅
```
Nuevo menú: NIGHT
Función: Toggle nightMode ON/OFF
Toggle: appConfig.nightMode

Estado: IMPLEMENTADO - commit 38dfe15
```

#### 1.3 Agregar menú INFO ✅
```
Nuevo menú: INFO
Función: Mostrar información del sistema (solo lectura)
Items (con etiquetas cortas para caber en 32px):
- IP 1.100 (últimos 2 octetos)
- WIFI -43 (señal dBm)
- V0.4.0 (versión)
- ID AABBCC (últimos 6 chars del deviceId)
- RAM 93K (heap libre)

Navegación: Izq/Der cambia entre datos, Select largo sale

Estado: IMPLEMENTADO - commit 38dfe15
```

### Fase 2: Mejoras (Prioridad Media)

#### 2.1 Agregar menú EFFECT
```
Nuevo menú: EFFECT
Función: Cambiar efecto de fondo
Valores: 19 efectos disponibles (cycle con nombre)
- Ninguno, Fireworks, Matrix, Plasma, Snake, etc.
```

#### 2.2 Agregar menú TRANS
```
Nuevo menú: TRANS
Función: Cambiar tipo de transición
Valores: Slide, Fade, None, etc.
```

#### 2.3 Agregar menú SLEEP
```
Nuevo menú: SLEEP
Función: Apagar pantalla temporalmente
Opciones: OFF, 1m, 5m, 15m, 30m
Cualquier botón despierta
```

#### 2.4 Agregar menú RESTART
```
Nuevo menú: RESTART
Función: Reiniciar dispositivo
Confirmación: Select para confirmar, cualquier otro cancela
```

### Fase 3: Refinamientos (Prioridad Baja)

#### 3.1 Auto-brightness rango
```
Expandir BRIGHT cuando está en AUTO:
- ABRI MIN: 1-50%
- ABRI MAX: 50-100%
```

#### 3.2 Factory Reset
```
Nuevo menú: RESET
Función: Borrar configuración
Confirmación doble requerida
Actualmente solo accesible con botón oculto (GPIO13, 5s)
```

#### 3.3 Timezone rápido
```
Nuevo menú: TZONE
Función: Cambiar zona horaria
Valores: UTC-12 a UTC+14 (o lista de ciudades)
```

---

## Mejoras de UX

### Indicadores Visuales
- [ ] Mostrar icono junto al nombre del menú (como ya hace APPS)
- [ ] Flash o sonido al guardar exitosamente
- [ ] Indicador de "modificado sin guardar"

### Información Contextual
- [ ] Mostrar valor actual en menú principal: "BRIGHT 75%" en lugar de solo "BRIGHT"
- [ ] Preview en tiempo real de cambios (ya funciona para brillo y color)

### Navegación
- [ ] Atajos: doble-click para valores comunes (ej: 50% brillo)
- [ ] Wrap-around más intuitivo en valores numéricos

---

## Orden de Implementación Sugerido

1. ~~**APPS expandido** - Agregar weather apps (cambio menor)~~ ✅
2. ~~**NIGHT toggle** - Alta demanda, cambio simple~~ ✅
3. ~~**INFO** - Útil para debugging sin web~~ ✅
4. **EFFECT** - Acceso rápido a efectos ⏳ PENDIENTE
5. **RESTART** - Conveniencia ⏳ PENDIENTE
6. **SLEEP** - Ahorro energía ⏳ PENDIENTE
7. Resto según necesidad

---

## Notas Técnicas

### Limitaciones de Pantalla
- Display: 32x8 pixels
- Máximo ~6 caracteres legibles con fuente estándar
- Nombres de menú deben ser cortos (5-6 chars máx)

### Persistencia
- Cambios se guardan en NVS al hacer Select largo
- No guardar en cada cambio (desgaste flash)

### Interfaces Necesarias
- `IDisplayRenderer` - Para dibujar menú
- `IDisplayControl` - Para aplicar brillo, transiciones
- `IDisplayNavigation` - Para recargar apps
- `IPeripheryProvider` - Para volumen, sensores
- `IUpdater` - Para OTA

---

## Referencias

- Código actual: `src/MenuManager/MenuManager.cpp`
- Documentación: `src/MenuManager/CLAUDE.md`
- Issue relacionado: TODO.md → "Analizar menú en pantalla"
