# AlarmManager — AI Reference

Local alarm clock + reminder engine for "autonomous mode" (works without WiFi via the DS1307 RTC). Singleton implementing `IAlarmProvider`.

## TL;DR

- **Provides:** `IAlarmProvider`
- **Consumes:** `ISound` + `IPeripheryProvider` (melody/stop), `IDisplayNotifier` (visual alert), `ITimeProvider` (`RtcTimeProvider` — NTP→RTC fallback)
- **Entry point:** `setup()` (load alarms), `tick(time_t now)` (called every loop iteration in `main.cpp`, **not** gated by WiFi)
- **Storage:** up to `MAX_ALARMS` (10) alarms in `/alarms.json` (LittleFS)
- **Scheduling logic:** extracted to pure `lib/services/AlarmLogic` (tested)

> 📌 Auto-loads when reading files in `src/AlarmManager/`

## Files

| File | Purpose |
|------|---------|
| `AlarmManager.h` | Singleton, `IAlarmProvider` impl, ringing/snooze state |
| `AlarmManager.cpp` | tick loop, trigger, snooze, persistence (`/alarms.json`) |

## Alarm model (`struct Alarm`, in `IAlarmProvider.h`)

| Field | Notes |
|-------|-------|
| `id` | 1-based, assigned by `generateId()` |
| `hour` / `minute` | trigger time |
| `days` | bitmask, **bit 0 = Sunday** … bit 6 = Saturday (`days & (1<<tm_wday)`) |
| `enabled` | active flag |
| `oneTime` | reminder: fires once then auto-disables; **ignores the day mask** |
| `snoozeMinutes` | per-alarm snooze duration (default 5) |
| `label` | shown on the notification |
| `melody` | RTTTL string or melody filename |

## Trigger flow (`tick`)

1. Get current time via `timeProvider_->now(t)` (RTC/NTP). Bail if no valid time.
2. **Snooze expiry** → re-ring (checked independently of `ringing_`).
3. Skip new triggers while `ringing_` or snoozed.
4. Dedup by **absolute epoch-minute** (`now / 60`) — fires once per occurrence, again next day.
5. For each alarm, `alarmMatches()` (AlarmLogic). On match: `triggerAlarm()` (play melody + red `hold:true, repeat:-1` notification). One-time alarms then auto-disable + save.

## Control surfaces

| Surface | How |
|---------|-----|
| **Physical buttons** (DisplayManager) | While ringing: Left/Right = `snooze(snoozeMinutes)`, Select / Select-long = `dismiss()`. Long-press while ringing does not open the menu. |
| **On-device menu** (MenuManager) | "ALARMS" item: list/toggle/edit time. See [MenuManager/CLAUDE.md](../MenuManager/CLAUDE.md) |
| **HTTP** (ServerManager) | `/api/alarms` GET/POST/PUT/DELETE (+ `{action:snooze|dismiss}`) |
| **MQTT/HA** (MQTTManager) | `binary_sensor` ringing, snooze/dismiss buttons, next-alarm sensor; topics `/alarm/snooze`, `/alarm/dismiss`, `/alarm/add` |

## Wiring in main.cpp

```cpp
AlarmManager.setSound(&PeripheryManager, &PeripheryManager);
AlarmManager.setNotifier(&DisplayManager.getNotifier());
AlarmManager.setTimeProvider(&rtcTimeProvider);   // RtcTimeProvider (NTP→RTC)
MenuManager.setAlarmProvider(&AlarmManager);       // on-device menu
// loop(): AlarmManager.tick(time(nullptr));       // every iteration, WiFi-independent
```

## Gotchas

- `tick(now)` uses `now` (system epoch) for dedup/snooze timing, but `timeProvider_` (RTC/NTP wall-clock) for hh:mm matching — intentional so alarms ring on the displayed time while dedup stays monotonic.
- `lastTriggerMinute_` is the absolute epoch-minute, **not** minute-of-day (fixing a bug where recurring alarms fired only once).
- Snooze sets `ringing_=false` + `snoozeUntil_`; re-ring must **not** require `ringing_` (fixed bug that froze all alarms after one snooze).
