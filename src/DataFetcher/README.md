# DataFetcher

External HTTP data source polling module. Periodically fetches values from
HTTP/HTTPS APIs, extracts JSON fields via dot-notation paths, and pushes
formatted results as custom apps to the display.

## Key Files

| File | Purpose |
|------|---------|
| `DataFetcher.h` | Singleton API |
| `DataFetcher.cpp` | HTTP fetching, JSON extraction, source CRUD, LittleFS persistence |
| `DataFetcherConfig.h` | `DataSourceConfig` struct (name, url, jsonPath, format, icon, color, interval) |

## How It Works

- Up to 8 data sources, each with a configurable polling interval (min 60s)
- Round-robin: one source checked per `tick()` call to avoid blocking
- Fetched values are pushed via `IDisplayNavigation::parseCustomPage()` (same path as MQTT custom apps)
- Sources persisted to `/DATAFETCHER/sources.json` on LittleFS

## Detailed Reference

See `CLAUDE.md` in this directory for full API, data flow, and endpoint docs.
