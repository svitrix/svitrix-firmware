# DataFetcher — AI Reference

Singleton module that periodically fetches values from external HTTP/HTTPS APIs and pushes them as custom apps to the display via `IDisplayNavigation::parseCustomPage()`.

## File Map

| File | LOC | Purpose |
|------|-----|---------|
| `DataFetcher.h` | 45 | Public API, singleton definition |
| `DataFetcher.cpp` | 403 | HTTP fetching, JSON extraction, source CRUD, LittleFS persistence |
| `DataFetcherConfig.h` | 18 | `DataSourceConfig` struct — per-source configuration |

## Interfaces

**Implements:** None (no interface of its own).

**Injected Dependencies (1):**
```cpp
void setNavigation(IDisplayNavigation *n);
```

Uses `IDisplayNavigation::parseCustomPage(name, json, show)` to feed fetched data into the custom app pipeline — the same path used by MQTT `/custom/#` and the HTTP API.

## DataSourceConfig

| Field | Type | Description |
|-------|------|-------------|
| `name` | String | Unique ID, becomes the custom app name (e.g., `"btc"`) |
| `url` | String | Full HTTP/HTTPS URL |
| `jsonPath` | String | Dot-notation path to extract (e.g., `"bitcoin.usd"`, `"data.0.price"`) |
| `displayFormat` | String | printf-style format (e.g., `"$%.0f"`) or empty for raw |
| `icon` | String | Icon name from LittleFS, or empty |
| `textColor` | String | Hex color `"#RRGGBB"` or empty for default |
| `interval` | uint32_t | Polling interval in seconds |

**Constraints:**
- `MIN_INTERVAL` = 60 seconds
- `DEFAULT_INTERVAL` = 900 seconds (15 min)
- `MAX_SOURCES` = 8

## Data Flow

```
External API
    │  HTTP GET (every N seconds)
    ▼
DataFetcher_::fetchAndPush()
    │  1. HTTP GET → response body (max 4 KB)
    │  2. extractJsonValue(body, jsonPath) → raw value string
    │  3. formatValue(src, raw) → printf-formatted string
    │  4. buildCustomAppJson(src, formatted) → {"text","icon","color","lifetime":0}
    ▼
IDisplayNavigation::parseCustomPage(name, json, true)
    │  (same pipeline as MQTT custom apps)
    ▼
Display shows as custom app
```

## Tick Behavior

- **Round-robin**: checks one source per `tick()` call to avoid blocking the main loop
- **Heap guard**: skips fetch if free heap < 40 KB (`MIN_FREE_HEAP`)
- **Staggered start**: all `lastFetch_` initialized to 0, so sources fetch on first eligible tick
- Only runs when `nav_` is set and `sources_` is non-empty

## Key Methods

| Method | Description |
|--------|-------------|
| `setup()` | Creates `/DATAFETCHER/` dir, loads sources from LittleFS |
| `tick()` | Round-robin poll — checks one source per call |
| `addSource(json)` | Parse JSON config, upsert by name, save to LittleFS |
| `removeSource(name)` | Remove source + clear its custom app from display |
| `forceFetch(name)` | Immediately fetch a specific source (used by API) |
| `getSourcesAsJson()` | Serialize all sources as JSON array |
| `loadSources()` / `saveSources()` | LittleFS persistence to `/DATAFETCHER/sources.json` |

### Private Methods

| Method | Description |
|--------|-------------|
| `fetchAndPush(index)` | HTTP GET + extract + format + push to display |
| `extractJsonValue(json, path)` | Walk dot-notation path through ArduinoJson (supports objects + arrays) |
| `formatValue(src, raw)` | Apply printf-style `displayFormat` to raw value |
| `buildCustomAppJson(src, value)` | Build JSON for `parseCustomPage()` with text, icon, color, lifetime=0 |

## HTTP API Endpoints (registered in ServerManager)

| Method | Endpoint | Description |
|--------|----------|-------------|
| `GET` | `/api/datafetcher` | List all sources as JSON array |
| `POST` | `/api/datafetcher` | Add/update source (body: DataSourceConfig JSON) |
| `DELETE` | `/api/datafetcher?name=X` | Remove source by name |
| `POST` | `/api/datafetcher/fetch?name=X` | Force immediate fetch for a source |
| `GET` | `/datafetcher` | Web UI page (`datafetcher_html`) |

### Add/Update Source JSON

```json
{
  "name": "btc",
  "url": "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd",
  "jsonPath": "bitcoin.usd",
  "displayFormat": "$%.0f",
  "icon": "btc",
  "color": "#F7931A",
  "interval": 300
}
```

Required fields: `name`, `url`, `jsonPath`. All others optional.

## HTTPS Handling

- HTTPS requests use `WiFiClientSecure` with `setInsecure()` (no cert validation)
- Rationale: DataFetcher hits arbitrary third-party APIs whose CAs cannot be pinned in advance
- HTTP requests use plain `WiFiClient`

## Persistence

- Sources stored in LittleFS at `/DATAFETCHER/sources.json`
- Directory created in `setup()` if missing
- `saveSources()` called on every add/remove operation
- Sources loaded automatically on `setup()`

## Wiring in main.cpp

```cpp
DataFetcher.setNavigation(&DisplayManager);     // IDisplayNavigation
assert(DataFetcher.hasNavigation());

// In setup(), after WiFi:
DataFetcher.setup();

// In loop(), only when connected:
if (ServerManager.isConnected) {
    DataFetcher.tick();
}
```

## Important Constraints

- Max response body: 4 KB (`MAX_RESPONSE_SIZE`) — larger responses rejected
- JSON parsing buffer: 2 KB `DynamicJsonDocument` for response extraction
- Connect timeout: 5 s, read timeout: 10 s
- One HTTP request per tick (round-robin) to avoid blocking
- Custom apps created with `lifetime: 0` — DataFetcher manages their lifecycle, they never auto-expire
- On `removeSource()`, the custom app is cleared from display via `parseCustomPage(name, "{}", false)`
- No authentication support — only public APIs (no API key headers)
