# Web Server

Async HTTP server for SVITRIX based on [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer).

## Directory Structure

```
lib/webserver/
├── pages/                          # Source HTML (edit these)
│   ├── setup.html                  # Device configuration page (WiFi, settings)
│   ├── edit.html                   # LittleFS file manager + Ace editor
│   ├── screen.html                 # Live 32x8 matrix view + PNG/GIF export
│   ├── screenfull.html             # Fullscreen matrix stream (WebSocket-like polling)
│   ├── backup.html                 # Settings backup/restore
│   ├── update.html                 # OTA firmware upload form
│   └── custom_icons_fragment.html  # Icon picker (HTML + CSS + JS injected into /setup)
├── generated/                      # Auto-generated headers (DO NOT edit)
│   ├── setup_htm.h                 # Gzipped PROGMEM — setup.html
│   ├── edit_htm.h                  # Gzipped PROGMEM — edit.html
│   └── htmls.h                     # Raw PROGMEM — screen, screenfull, backup, update, icons
├── esp-fs-webserver.h              # FSWebServer class declaration
└── esp-fs-webserver.cpp            # FSWebServer implementation
```

## Editing Pages

1. Edit the HTML file in `pages/`
2. Regenerate headers:
   ```bash
   python3 tools/web_compress.py
   ```
3. Rebuild firmware:
   ```bash
   pio run -e ulanzi
   ```

To verify headers are up-to-date without writing:
```bash
python3 tools/web_compress.py --check
```

### Compression

- `setup.html` and `edit.html` — gzip-compressed into byte arrays, served with `Content-Encoding: gzip`
- All other pages — stored as raw `PROGMEM` strings via C++ raw string literals (`R"EOF(...)EOF"`)

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

### OTA & System

| Method | Path | Body | Description |
|--------|------|------|-------------|
| GET | `/update` | — | OTA upload form |
| POST | `/update` | multipart | Upload firmware `.bin` / `.bin.gz` |
| POST | `/api/doupdate` | JSON | Check and trigger remote update |
| GET | `/version` | — | Firmware version string |
| POST | `/save` | — | Apply configuration |

### File Manager

| Method | Path | Body | Description |
|--------|------|------|-------------|
| GET | `/edit` | — | File manager page |
| GET | `/list?dir=/` | — | List directory contents |
| GET | `/status` | — | Filesystem usage stats |
| PUT | `/edit` | form (`path`) | Create file or directory |
| POST | `/edit` | multipart | Upload file |
| DELETE | `/edit` | form (`path`) | Delete file or directory |

### Web Pages

| Method | Path | Description |
|--------|------|-------------|
| GET | `/` | Index (redirects to `/setup` if no index.htm) |
| GET | `/setup` | Device configuration |
| GET | `/screen` | Live matrix view |
| GET | `/fullscreen?fps=30` | Fullscreen matrix stream |
| GET | `/backup` | Settings backup/restore |

### WiFi & Captive Portal

| Method | Path | Description |
|--------|------|-------------|
| GET | `/scan` | Scan WiFi networks |
| POST | `/connect` | Connect to WiFi (form: `ssid`, `password`, `persistent`) |
| GET | `/ipaddress` | Current IP as JSON |
| GET | `/restart` | Restart device |
| GET | `/redirect` | Captive portal redirect |

## Authentication

All routes are protected by HTTP Basic Auth when credentials are configured via `mws.setAuth(user, pass)`. Authentication is bypassed in AP mode (captive portal).

## CORS

Global headers on all responses:
```
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: PUT,POST,GET,OPTIONS,DELETE
Access-Control-Allow-Headers: *
```
