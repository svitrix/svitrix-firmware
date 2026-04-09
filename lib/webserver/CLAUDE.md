# Web Server — AI Reference

Async HTTP server wrapper around ESPAsyncWebServer. Provides WiFi management, captive portal, LittleFS file serving, OTA updates, and route registration used by `ServerManager`.

## Files

| File | Role |
|------|------|
| `esp-fs-webserver.h` | `FSWebServer` class declaration, `getBody()` helper, config option templates |
| `esp-fs-webserver.cpp` | WiFi, captive portal, file ops, OTA, route registration |
| `generated/setup_htm.h` | Gzipped PROGMEM — setup/config page (12 KB compressed) |
| `generated/edit_htm.h` | Gzipped PROGMEM — LittleFS file manager with Ace editor |
| `generated/htmls.h` | Raw PROGMEM strings — `screen_html`, `screenfull_html`, `backup_html`, `update_html`, `datafetcher_html`, plus custom icon picker |
| `pages/*.html` | Source HTML files (7 pages) — edit these, then regenerate headers |

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

### Setup Page Options (`INCLUDE_SETUP_HTM`)

Dynamic config form backed by `/DoNotTouch.json` on LittleFS:

```cpp
void addOptionBox(const char *boxTitle);
void addOption(const char *label, T val, bool hidden = false, ...);
void addDropdownList(const char *label, const char **array, size_t size);
bool getOptionValue(const char *label, T &var);
bool saveOptionValue(const char *label, T val);
```

### File Manager (`INCLUDE_EDIT_HTM`)

| Method | Path | Action |
|--------|------|--------|
| GET | `/edit` | File manager page |
| GET | `/list?dir=/` | List directory (JSON) |
| GET | `/status` | FS usage stats (JSON) |
| PUT | `/edit` | Create file/directory |
| POST | `/edit` | Upload file (multipart) |
| DELETE | `/edit` | Delete file/directory |

### OTA Update

Built-in at `/update` (GET = upload form, POST = firmware upload via `Update` API). Reboots on success.

## Built-in Routes (registered in `begin()`)

| Path | Description |
|------|-------------|
| `/` | Index: serves `/index.htm`, falls back to `/setup` |
| `/setup` | Device configuration page (gzipped PROGMEM) |
| `/scan` | WiFi network scan (JSON) |
| `/connect` | WiFi connection (POST) |
| `/ipaddress` | Current IP address |
| `/restart` | Reboot device |
| `/update` | OTA firmware upload |
| `/redirect`, `/connecttest.txt`, `/hotspot-detect.html`, `/generate_204` | Captive portal redirects |

## HTML Page Pipeline

```
pages/*.html  →  python3 tools/web_compress.py  →  generated/*.h  →  PROGMEM in firmware
```

- `setup.html`, `edit.html` — gzip-compressed byte arrays, `Content-Encoding: gzip`
- All others — raw C++ string literals (`R"EOF(...)EOF"`) as `PROGMEM`

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
| LittleFS | File storage and serving |
| DNSServer | Captive portal DNS in AP mode |
| WiFi / esp_wifi | STA/AP connection management |
| Update | OTA firmware flashing |
| ArduinoJson v6 | `/DoNotTouch.json` config persistence |
