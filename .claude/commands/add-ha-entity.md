---
description: Step-by-step guide to add a new Home Assistant entity with MQTT auto-discovery
---

Guide through adding a new Home Assistant entity to the Svitrix firmware. Currently there are 25 HA entities.

## Before starting

1. **Read current HA integration docs:**
   - `src/MQTTManager/CLAUDE.md` — current entities, topics, callbacks
   - `lib/home-assistant-integration/CLAUDE.md` — enabled entity types
   - `lib/services/CLAUDE.md` — HADiscovery service API

2. **Ask the user:**
   - Entity type: `sensor`, `switch`, `number`, `button`, `light`, `select`, or `text`
   - Entity name and purpose
   - Value range (for number), options (for select), unit (for sensor)
   - Whether it needs a command topic (writeable from HA) or is read-only

## Implementation steps

3. **Add entity descriptor** to `lib/services/HADiscovery/`:
   - Add a new `HAEntityDescriptor` to the descriptors array
   - Follow the existing pattern: name, unique_id, component type, config fields
   - Add test for the new descriptor in `test/test_native/test_ha_discovery/`

4. **Add MQTT command handler** (if entity is writeable):
   - Add route to `lib/services/MessageRouter/`
   - Add handler function in `src/MQTTManager/MQTTManager_callbacks.cpp`
   - Add test for the route in `test/test_native/test_message_router/`

5. **Add state publishing:**
   - In `src/MQTTManager/MQTTManager_.cpp` — publish entity state in the stats cycle
   - Use the existing `publishState()` pattern

6. **Wire in DisplayManager** (if entity affects display):
   - Add config field in `lib/config/ConfigTypes.h` if needed
   - Read `lib/config/CLAUDE.md` for config struct conventions
   - Handle the new setting in `src/DisplayManager/`

7. **Run tests:**
   ```bash
   pio test -e native_test
   ```

8. **Build and verify:**
   ```bash
   pio run -e ulanzi
   ```

9. **Update documentation:**
   - Update `src/MQTTManager/CLAUDE.md` — add entity to the list
   - Update entity count in root `CLAUDE.md` if needed

## Checklist
- [ ] HAEntityDescriptor added
- [ ] Test for descriptor added
- [ ] MQTT command handler added (if writeable)
- [ ] Test for command handler added
- [ ] State publishing added
- [ ] Config struct updated (if needed)
- [ ] DisplayManager integration (if needed)
- [ ] All tests pass
- [ ] Build succeeds
- [ ] CLAUDE.md updated
