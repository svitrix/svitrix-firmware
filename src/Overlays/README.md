# Overlays

Overlay rendering functions drawn on top of the LED matrix during app display.
Provides the status overlay (WiFi/MQTT connection indicators), menu overlay,
and notification overlay with queue management.

## Key Files

| File | Purpose |
|------|---------|
| `Overlays.h` | `Notification` struct, overlay function declarations, notification queue |
| `Overlays.cpp` | `StatusOverlay`, `MenuOverlay`, `NotifyOverlay` implementations |

## Overlay Functions

| Function | Purpose |
|----------|---------|
| `StatusOverlay` | Blinking pixel indicators for WiFi (red) and MQTT (yellow) disconnection |
| `MenuOverlay` | Renders the on-device settings menu when active |
| `NotifyOverlay` | Renders notifications from the `notifications` deque with icon, text, effects |

## Notification Struct

Extends `AppContentBase` with notification-specific fields: `duration`, `repeat`,
`hold`, `wakeup`, `sound`, `rtttl`, and scroll starting from the right edge.
