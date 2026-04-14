# Svitrix Web UI

Preact + Vite + TypeScript SPA for the Svitrix ESP32 pixel clock.

Replaces the embedded PROGMEM HTML pages. Served from LittleFS `/web/` on the device.

## Quick Start

```bash
cd web
npm install
npm run dev       # Dev server at localhost:5173, proxies API to device
```

## Build & Deploy

```bash
npm run build     # Build to ../data/web/ (gzipped)
npm run upload    # Build + upload to device LittleFS via PlatformIO
```

The PlatformIO pre-build script (`tools/build_web.py`) also auto-builds the SPA before firmware compilation if source files changed.

## Architecture

```
Browser  ←→  ESP32
  │              │
  │  GET /       │  → LittleFS /web/index.html.gz (SPA shell)
  │  GET /app.js │  → LittleFS /web/app.js.gz (all pages)
  │              │
  │  /api/*      │  → ServerManager JSON endpoints (35 routes)
  │  /DoNotTouch │  → Config file (Network, MQTT, Time, Auth)
  │  /edit       │  → LittleFS file operations
  │  /scan       │  → WiFi network scan
  │  /connect    │  → WiFi connection
```

## Pages

| Route | Page | Replaces |
|-------|------|----------|
| `/` | Screen — live 32x8 canvas, app navigation | `screen_html`, `screenfull_html` |
| `/settings` | Settings — display, WiFi, MQTT, NTP, auth, icons | `SETUP_HTML`, `custom_html/css/script` |
| `/datafetcher` | DataFetcher — CRUD for HTTP data sources | `datafetcher_html` |
| `/backup` | Backup — download/upload device config | `backup_html` |
| `/update` | Update — OTA firmware upload | `update_html` (fallback kept in firmware) |
| `/files` | Files — LittleFS browser and text editor | `edit_htm_gz` |

## Dev Proxy

In dev mode, API requests are proxied to the real device. Default IP: `192.168.50.92`.

To override, create `web/.env.local` (not committed to git):

```
VITE_DEVICE_IP=192.168.1.42
```

## Bundle Size

Target: < 30 KB gzip. Current: ~17.7 KB gzip.

```
app.js.gz      ~16.6 KB   (Preact + router + signals + all pages)
style.css.gz    ~0.9 KB   (global styles, dark theme)
index.html.gz   ~0.2 KB   (minimal shell)
```

## Tech Stack

- **Preact** — 3.5 KB React-compatible framework
- **Preact Router** — client-side routing
- **Preact Signals** — reactive state (toast notifications)
- **Vite** — build tool with gzip compression
- **TypeScript** — strict mode, typed API client
- **CSS Modules** — scoped styles per component (`.module.css`)

## Project Structure

```
src/
  main.tsx                    # Entry point, router setup
  api/
    client.ts                 # Fetch-based API client (~30 endpoints)
    types.ts                  # TypeScript interfaces
  components/
    Nav.tsx + Nav.module.css   # Navigation bar + theme toggle
    Toast.tsx                 # Toast notifications (signal-based)
    ui/                       # Reusable UI components (8)
      Toggle.tsx              # On/off switch
      TextField.tsx           # Text input with label
      ColorField.tsx          # Color picker with hex conversion
      Slider.tsx              # Range input with value display
      Select.tsx              # Dropdown with typed onChange
      Card.tsx                # Section card with title
      FormRow.tsx             # 2-column grid layout
      Button.tsx              # Primary/danger/default variants
      index.ts                # Barrel export
  context/
    SettingsContext.tsx        # Shared state for settings + config
  pages/
    screen/                   # Live LED matrix preview
    settings/                 # Device configuration
      sections/               # 15 independent settings sections (incl. NightModeSection)
    data-fetcher/             # External API data sources
    files/                    # LittleFS file manager
    backup/                   # Config backup/restore
    update/                   # OTA firmware upload
  styles/
    global.css                # CSS variables, resets, base elements
```

## Conventions

- **Directory-per-page**: each page in its own directory with `index.ts` barrel export
- **CSS Modules**: use `.module.css` files, no inline styles for static layout
- **CSS variables**: use `var(--bg)`, `var(--accent)`, etc. for theming (dark/light)
- **Preact**: use `class` not `className`, import from `preact` and `preact/hooks`
- **Components**: reusable UI in `components/ui/`, page-specific in page directory
- **Context**: shared state via `SettingsContext`, sections save only their own fields
- **Types**: strict TypeScript, explicit interfaces for all API data
