# ServerManager

HTTP REST API server and primary external control interface (alongside MQTT).
Handles WiFi connectivity, mDNS/UDP discovery, 35 REST endpoints for device
control, and a TCP server for game controller input.

## Key Files

| File | Purpose |
|------|---------|
| `ServerManager.h` | Singleton, implements `IButtonReporter` |
| `ServerManager.cpp` | WiFi setup, 35 HTTP endpoints, UDP/TCP servers, settings loader |

## Responsibilities

- WiFi station + AP fallback (`192.168.4.1`)
- mDNS registration (`http` + `svitrix` services)
- UDP discovery (port 4210/4211) — responds to `FIND_SVITRIX`
- TCP game input (port 8080) — single client, newline-delimited
- HTTP button callback to external URL on press/release
- SPA served from LittleFS root

## Detailed Reference

See the ServerManager section in [src/CLAUDE.md](../CLAUDE.md) for full
endpoint table, config initialization, and communication protocols.
