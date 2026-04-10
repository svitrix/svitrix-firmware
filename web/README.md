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
  │  /api/*      │  → ServerManager JSON endpoints (38 routes)
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

In dev mode, API requests are proxied to the real device at `192.168.50.92` (configured in `vite.config.ts`). Change the IP if your device has a different address.

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
- **TypeScript** — typed API client
