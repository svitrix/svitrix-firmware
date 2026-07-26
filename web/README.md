# Svitrix Web UI

Preact + Vite + TypeScript SPA for the Svitrix ESP32 pixel clock.

Replaces the embedded PROGMEM HTML pages. Served from LittleFS `/web/` on the device.

## Documentation

- **[CLAUDE.md](CLAUDE.md)** — build guide: how to add a component / page / settings
  section following the Liquid Glass + WCAG 2.1 AA rules (start here when coding).
- **[DESIGN.md](DESIGN.md)** — design spec: principles, tokens (§9), materials,
  components (§6), motion (§7), accessibility (§8), rationale (§11).
- This file — run/build, architecture, project structure, conventions.

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
| `/settings` | Settings — master–detail: category rail + auto-save | `SETUP_HTML`, `custom_html/css/script` |
| `/datafetcher` | DataFetcher — CRUD for HTTP data sources | `datafetcher_html` |
| `/backup` | Backup — download/upload device config | `backup_html` |
| `/update` | Update — OTA firmware upload | `update_html` (fallback kept in firmware) |
| `/files` | Files — LittleFS browser and text editor | `edit_htm_gz` |

**Settings** is a master–detail screen (`SettingsLayout` + `SettingsNav`, an ARIA
tablist): sections are grouped into five categories — **Appearance** (display,
clock face, night mode), **Screens** (apps), **Network** (WiFi, MQTT, NTP), **System**
(auth, sound, device actions), **Tools** (notify, icon picker) — replacing the old
flat card list. **Display settings auto-save** on change (debounced `setSetting()` in `SettingsContext`, status shown
by `SaveIndicator`; no per-section Save buttons). **Network/auth settings save
explicitly** via `InfraSaveBar` and trigger a `RebootOverlay` that polls the device
back online. Controls disable via a `<fieldset disabled>` while the device is
unreachable.

## Dev Proxy

In dev mode, API requests are proxied to the real device. Default IP: `192.168.50.92`.

To override, create `web/.env.local` (not committed to git):

```
VITE_DEVICE_IP=192.168.1.42
```

## Bundle Size

```
app.js.gz      ~44 KB    (Preact + router + signals + all pages + i18n × 5 langs)
style.css.gz   ~5.7 KB   (global styles + Liquid Glass tokens, light/dark)
index.html.gz  ~0.2 KB   (minimal shell)
```

The original < 30 KB gzip target is now exceeded because all five locales are
bundled into a single IIFE (no code-splitting on the device). The glass/token CSS
weighs almost nothing; the growth is i18n + the accessibility pass.

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
    LivePreview/              # Live 32x8 canvas (polls /api/screen); page|compact
    SaveIndicator/            # Auto-save status label (idle/saving/saved/error)
    RebootOverlay/            # Reconnect modal (polls /version until back online)
    ui/                       # Reusable UI primitives (glass + a11y)
      Toggle/ TextField/ ColorField/ TimeField/ Slider/ Select/
      Card/ FormRow/ Button/ Dialog/   # (Dialog = Confirm/Prompt)
      index.ts                # Barrel export
  context/
    SettingsContext.tsx        # Shared settings/config state + auto-save + heartbeat
  pages/
    screen/                   # Live LED matrix preview
    settings/                 # Master–detail: SettingsLayout + SettingsNav (tablist)
      sections/               # Per-category settings sections + InfraSaveBar
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
- **CSS variables**: use the Liquid Glass tokens (`var(--surface-tint)`, `var(--label)`, `var(--accent)`, etc.) for theming (dark/light) — see `DESIGN.md` §9
- **Typography**: use the `rem`-based type tokens (`--text-body`, `--text-footnote`, `--lh-*`, `--weight-*`) — never hard-code `px` font sizes (WCAG 1.4.4); see `DESIGN.md` §5
- **Preact**: use `class` not `className`, import from `preact` and `preact/hooks`
- **Components**: reusable UI in `components/ui/`, page-specific in page directory
- **Context**: shared state via `SettingsContext` — display sections auto-save with `setSetting()`; infra sections save explicitly through `InfraSaveBar`
- **Types**: strict TypeScript, explicit interfaces for all API data
