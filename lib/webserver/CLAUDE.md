# Web Server — AI Reference

Async HTTP server wrapper around ESPAsyncWebServer. Provides WiFi management, captive portal, LittleFS file serving, SPA routing, OTA updates, and route registration used by `ServerManager`.

## Files

| File | Role |
|------|------|
| `esp-fs-webserver.h` | `FSWebServer` class declaration, `getBody()` helper |
| `esp-fs-webserver.cpp` | WiFi, captive portal, file ops, OTA, SPA fallback, route registration |

## Architecture

All HTML pages are served from the **Preact SPA** stored in LittleFS `/web/`. The server's role is:
1. Route registration for API endpoints (used by `ServerManager`)
2. Serve SPA files from LittleFS (`/web/index.html.gz`, `/web/app.js.gz`, etc.)
3. SPA fallback: non-API 404 → `/web/index.html` (client-side routing)
4. WiFi management (STA/AP mode, captive portal)
5. LittleFS file operations (list, upload, delete, create)
6. OTA firmware update (with inline `update_html` fallback)

Only `update_html` (~1 KB) is embedded in firmware as PROGMEM — a safety fallback for OTA if LittleFS is corrupted.

## FSWebServer Class

Wraps `AsyncWebServer` + `fs::FS` (LittleFS). Instance `mws` created in `ServerManager.cpp`.

### Route Registration (used by ServerManager)

```cpp
void addHandler(const char *uri, WebRequestMethodComposite method, ArRequestHandlerFunction fn);
void addHandler(const char *uri, ArRequestHandlerFunction handler);           // HTTP_ANY
void addHandlerWithBody(const char *uri, WebRequestMethodComposite method, ArRequestHandlerFunction fn);
void onNotFound(ArRequestHandlerFunction fn);
```

`addHandlerWithBody()` accumulates chunked POST body into `request->_tempObject`. Use `getBody(request)` to read as `String`.

### WiFi Management

```cpp
IPAddress startWiFi(uint32_t timeout, const char *apSSID, const char *apPsw);  // STA → AP fallback
IPAddress setAPmode(const char *ssid, const char *psk);                         // Force AP mode
void setCaptiveWebage(const char *url);                                         // Captive portal target
```

### Authentication

```cpp
void setAuth(const String &user, const String &pass);  // HTTP Basic Auth on all routes
```

### File Manager

| Method | Path | Action |
|--------|------|--------|
| GET | `/edit` | Serves SPA (file manager page at `/files`) |
| GET | `/list?dir=/` | List directory (JSON) |
| GET | `/status` | FS usage stats (JSON) |
| PUT | `/edit` | Create file/directory |
| POST | `/edit` | Upload file (multipart) |
| DELETE | `/edit` | Delete file/directory |

### OTA Update

Built-in at `/update` (GET = inline `update_html` fallback form, POST = firmware upload via `Update` API). Reboots on success.

## Built-in Routes (registered in `begin()`)

| Path | Description |
|------|-------------|
| `/` | SPA index: serves `/web/index.html` from LittleFS |
| `/setup` | Redirects to SPA (captive portal compatibility) |
| `/scan` | WiFi network scan (JSON) |
| `/connect` | WiFi connection (POST) |
| `/ipaddress` | Current IP address |
| `/restart` | Reboot device |
| `/update` | OTA firmware upload (inline HTML fallback) |
| `/redirect`, `/connecttest.txt`, `/hotspot-detect.html`, `/generate_204` | Captive portal redirects to `/settings` |

## SPA Routing

Request flow for non-API paths:

```
Request → handleFileRead(url)     → found? serve file
        → /web/index.html         → found? serve SPA (client-side routing)
        → 404
```

The SPA (`web/` directory) handles all page routing client-side via `preact-router`.

## CORS

Global headers on all responses:
```
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: PUT,POST,GET,OPTIONS,DELETE
Access-Control-Allow-Headers: *
```

## Dependencies

| Dependency | Usage |
|------------|-------|
| ESPAsyncWebServer | Underlying async HTTP server |
| LittleFS | File storage and SPA serving |
| DNSServer | Captive portal DNS in AP mode |
| WiFi / esp_wifi | STA/AP connection management |
| Update | OTA firmware flashing |
