# Web Server

Async HTTP server for SVITRIX based on [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer).

## Architecture

The web server is a **pure HTTP server** — it handles routing, WiFi, file operations, and OTA. All UI pages are served from the **Preact SPA** stored in LittleFS root.

```
Browser request
  → API route (/api/*)     → ServerManager handler → JSON response
  → File route (/list, /edit) → FSWebServer handler → JSON/file
  → SPA route (/, /settings, etc.) → LittleFS /index.html → client-side routing
  → OTA (/update GET)      → inline update_html (1 KB PROGMEM fallback)
```

## Directory Structure

```
lib/webserver/
├── esp-fs-webserver.h     # FSWebServer class declaration
└── esp-fs-webserver.cpp   # Implementation: WiFi, file ops, OTA, SPA routing
```

The SPA source lives in `web/` (project root) — see `web/README.md`.

## API Routes

### Device Control

| Method | Path | Body | Description |
|--------|------|------|-------------|
| GET | `/api/stats` | — | Device stats (IP, version, UID, uptime) |
| GET | `/api/screen` | — | LED matrix state as JSON (256 RGB values) |
| GET | `/api/settings` | — | Current device settings |
| POST | `/api/settings` | JSON | Update device settings |
| POST | `/api/power` | JSON | Set power on/off |
| POST | `/api/sleep` | JSON | Set sleep mode |
| ANY | `/api/reboot` | — | Reboot device |
| ANY | `/api/erase` | — | Factory reset (WiFi, LittleFS, settings) |
| ANY | `/api/resetSettings` | — | Reset settings to defaults |

### Apps

| Method | Path | Body | Description |
|--------|------|------|-------------|
| GET | `/api/apps` | — | All apps with icons |
| GET | `/api/loop` | — | Available app list |
| POST | `/api/apps` | JSON | Update app list |
| POST | `/api/switch` | JSON | Switch to specific app |
| ANY | `/api/nextapp` | — | Switch to next app |
| POST | `/api/previousapp` | — | Switch to previous app |
| POST | `/api/reorder` | JSON | Reorder apps |
| POST | `/api/custom` | JSON | Push custom app (query: `?name=`) |

### Notifications

| Method | Path | Body | Description |
|--------|------|------|-------------|
| POST | `/api/notify` | JSON | Show notification (`text`, `icon`, `duration`, `color`, `rainbow`, `rtttl`, `sound`) |
| ANY | `/api/notify/dismiss` | — | Dismiss current notification |

### Indicators

| Method | Path | Body | Description |
|--------|------|------|-------------|
| POST | `/api/indicator1` | JSON | Configure indicator 1 |
| POST | `/api/indicator2` | JSON | Configure indicator 2 |
| POST | `/api/indicator3` | JSON | Configure indicator 3 |

### Effects & Transitions

| Method | Path | Body | Description |
|--------|------|------|-------------|
| GET | `/api/effects` | — | Available effect names |
| GET | `/api/transitions` | — | Available transition names |

### Sound

| Method | Path | Body | Description |
|--------|------|------|-------------|
| POST | `/api/rtttl` | text | Play RTTTL ringtone |
| POST | `/api/sound` | JSON | Play sound file |
| POST | `/api/r2d2` | JSON | Play R2D2 sound |
| POST | `/api/moodlight` | JSON | Set moodlight |

### DataFetcher

| Method | Path | Body | Description |
|--------|------|------|-------------|
| GET | `/api/datafetcher` | — | List data sources |
| POST | `/api/datafetcher` | JSON | Add data source |
| DELETE | `/api/datafetcher` | query `?name=` | Delete data source |
| POST | `/api/datafetcher/fetch` | query `?name=` | Force-fetch a source |

### OTA & System

| Method | Path | Body | Description |
|--------|------|------|-------------|
| GET | `/update` | — | OTA upload form (inline HTML fallback) |
| POST | `/update` | multipart | Upload firmware `.bin` / `.bin.gz` |
| POST | `/api/doupdate` | JSON | Check and trigger remote update |
| GET | `/version` | — | Firmware version string |
| POST | `/save` | — | Apply `/DoNotTouch.json` configuration |

### File Manager

| Method | Path | Body | Description |
|--------|------|------|-------------|
| GET | `/edit` | — | Serves SPA (file manager at `/files`) |
| GET | `/list?dir=/` | — | List directory contents |
| GET | `/status` | — | Filesystem usage stats |
| PUT | `/edit` | form (`path`) | Create file or directory |
| POST | `/edit` | multipart | Upload file |
| DELETE | `/edit` | form (`path`) | Delete file or directory |

### WiFi & Captive Portal

| Method | Path | Description |
|--------|------|-------------|
| GET | `/` | SPA index from LittleFS root |
| GET | `/setup` | Redirects to SPA |
| GET | `/scan` | Async WiFi scan (202 while scanning, 200 with results) |
| POST | `/connect` | Connect to WiFi (form: `ssid`, `password`, `persistent`) |
| GET | `/ipaddress` | Current IP as JSON |
| GET | `/restart` | Restart device |
| GET | `/redirect` | Captive portal redirect to `/settings` |

## Authentication

All routes are protected by HTTP Basic Auth when credentials are configured via `mws.setAuth(user, pass)`. Authentication is bypassed in AP mode (captive portal).

## CORS

Global headers on all responses:
```
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: PUT,POST,GET,OPTIONS,DELETE
Access-Control-Allow-Headers: *
```
