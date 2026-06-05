# Plan: Unified Apps Tab

## Context

El tab de Apps actual tiene 5 secciones separadas (Playlist, AppOrder, AppsSection, WeatherAppsSection, AppTransitionsSection) lo cual es confuso y fragmentado. El usuario quiere unificarlas en una sola lista drag & drop donde:
- Apps, efectos y fuentes de DataFetcher aparezcan juntos
- Cada item tenga settings inline (color, duración, icono)
- Se pueda repetir el mismo item múltiples veces
- Los iconos personalizados persistan entre actualizaciones

## Data Model

### Frontend (`web/src/api/types.ts`)

```typescript
interface RotationItem {
  id: string;              // UUID para identificar instancias duplicadas
  type: "app" | "effect";  // Tipo de item
  name: string;            // Nombre del app/efecto
  enabled: boolean;        // Toggle per-item
  duration: number;        // Segundos (0 = usar default)
  color: number | null;    // Override de color (null = heredar)
  icon: string | null;     // Override de icono (null = default)
}

interface RotationConfig {
  items: RotationItem[];
}
```

### Backend (`lib/config/src/ConfigTypes.h`)

```cpp
struct RotationItem {
    String id;           // 8-char UUID
    uint8_t type;        // 0=app, 1=effect
    String name;
    bool enabled;
    uint16_t duration;   // 0 = use default
    uint32_t color;      // 0 = use default
    String icon;         // empty = use default
};

struct RotationConfig {
    String items;        // JSON array serializado (NVS key: ROT_ITEMS)
};
```

## API Changes

| Endpoint | Método | Descripción |
|----------|--------|-------------|
| `/api/rotation` | GET | Retorna `{items: RotationItem[]}` |
| `/api/rotation` | POST | Guarda items (parcial o completo) |

Deprecar gradualmente:
- `POST /api/reorder` → usar `/api/rotation`
- `GET/POST /api/playlist` → usar `/api/rotation`

## UI Structure

```
AppsPage
├── UnifiedRotationSection (nuevo)
│   ├── List con drag & drop
│   │   └── RotationItemRow (por cada item)
│   │       ├── DragHandle (☰)
│   │       ├── EnableToggle
│   │       ├── Icon preview + Name + TypeBadge
│   │       ├── ExpandButton (v)
│   │       └── [Expanded panel]
│   │           ├── ColorField
│   │           ├── DurationSlider
│   │           └── IconPicker (para apps nativas también)
│   └── AddButton → Modal con tabs (Apps | Efectos)
└── AppTransitionsSection (sin cambios)
```

## Implementation Phases

### Phase 1: Backend Data Model
**Files:**
- `lib/config/src/ConfigTypes.h` — Agregar `RotationConfig`
- `src/Globals.h/.cpp` — Agregar `rotationConfig`, migración desde `appOrder` + `playlistConfig`
- `src/ServerManager/ServerManager.cpp` — Endpoints `/api/rotation`

**Migration logic:**
1. Si existe `ROT_ITEMS` en NVS → ya migrado
2. Si `playlistConfig.enabled` → convertir `playlistConfig.items` al nuevo formato
3. Si no → convertir `appOrder` al nuevo formato con defaults
4. Guardar y limpiar keys viejas

### Phase 2: Backend Rendering
**Files:**
- `src/DisplayManager/DisplayManager_Artnet.cpp` — Usar `rotationConfig.items` para resolver next app
- `src/Apps/Apps_NativeApps.cpp` — Soportar override de icono para apps nativas
- `src/Apps/Apps_Helpers.cpp` — Aplicar override de color per-item

### Phase 3: Frontend Components
**Files nuevos:**
- `web/src/pages/apps/components/UnifiedRotationSection.tsx`
- `web/src/pages/apps/components/RotationItemRow.tsx`
- `web/src/pages/apps/components/AddItemModal.tsx`

**Files a modificar:**
- `web/src/api/types.ts` — Agregar `RotationItem`, `RotationConfig`
- `web/src/api/client.ts` — Agregar `getRotation()`, `saveRotation()`
- `web/src/pages/apps/Apps.tsx` — Reemplazar sections con nuevo componente

### Phase 4: Cleanup
- Eliminar `AppsSection.tsx`, `WeatherAppsSection.tsx`, `AppOrderSection.tsx`, `PlaylistSection.tsx`
- Eliminar endpoints deprecados
- Actualizar i18n

## Icon Persistence

Los iconos en `/ICONS/` **ya persisten** entre actualizaciones OTA porque:
- OTA solo actualiza la partición de firmware, no LittleFS
- Factory reset (`formatSettings()`) solo borra NVS, no LittleFS

Para apps nativas con icono custom:
```cpp
// En el render de app nativa
if (!item.icon.isEmpty()) {
    String path = "/ICONS/" + item.icon;
    if (LittleFS.exists(path + ".gif") || LittleFS.exists(path + ".jpg")) {
        // Usar icono custom
    }
} else {
    // Usar icono hardcoded de icons.h
}
```

## Verification

1. Build: `pio run -e ulanzi`
2. Test migración: Crear config vieja → actualizar → verificar que se migra
3. Web UI: `cd web && npm run dev` → probar drag & drop, toggles, settings inline
4. Device: Verificar que la rotación funciona con items repetidos y overrides

## Weather Apps

Las Weather Apps (OutdoorTemp, OutdoorHum, Pressure, AQI, UV) aparecen como **items individuales** en la lista unificada:
- Cada una se puede ordenar, repetir, habilitar/deshabilitar independientemente
- Tienen sus propios overrides de color, duración, icono
- Se agregan via el modal "Add Item" igual que cualquier otra app
- Eliminamos `WeatherAppsSection.tsx` completamente

## Notes

- Mantener `AppTransitionsSection` sin cambios (efectos de transición, velocidad, etc.)
- DataFetcher apps aparecen automáticamente cuando están habilitadas
- Per-item duration override ya existe en playlist, solo extenderlo
- Apps nativas (Time, Date, Temp, Hum, Bat) + Weather apps = todas items individuales
