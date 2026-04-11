---
description: Add a new visual effect following the IPixelCanvas pattern
---

Guide through adding a new visual effect to the Svitrix firmware. Currently there are 19 effects.

## Before starting

1. **Read current effects documentation:**
   - `src/effects/README.md` — effect architecture, IPixelCanvas API, registration
   - `lib/interfaces/CLAUDE.md` — IPixelCanvas interface definition

2. **Ask the user:**
   - Effect name and visual description
   - Category: pattern, wave, weather overlay, or game effect
   - Parameters: speed, color palette, density, etc.
   - Whether it uses noise functions (inoise8), palettes, or simple math

## Implementation steps

3. **Create effect function:**
   - Add new `.cpp` file in `src/effects/` (or add to existing category file)
   - Function signature follows the pattern:
     ```cpp
     void effectName(IPixelCanvas& canvas, uint8_t speed, const CRGBPalette16& palette);
     ```
   - Use `IPixelCanvas` methods: `setPixel()`, `getWidth()`, `getHeight()`, `fill()`
   - Never access FastLED or NeoMatrix directly — always go through IPixelCanvas

4. **Register the effect:**
   - Add to the effect registry/list in the effects system
   - Assign an effect ID (check existing IDs to avoid conflicts)
   - Add to the effect name mapping

5. **Add test** (if pure logic):
   - Add test in `test/test_native/test_effects/`
   - Use MockDisplay for IPixelCanvas mock
   - Test edge cases: speed=0, speed=255, single pixel canvas

6. **Wire to DisplayManager:**
   - Effect should be selectable via MQTT and REST API
   - Check that `DisplayManager_` can invoke the new effect

7. **Build and verify:**
   ```bash
   pio test -e native_test && pio run -e ulanzi
   ```
   IMPORTANT: Check flash size after — effects add to binary size.

8. **Update documentation:**
   - Update `src/effects/README.md` — add effect to the list
   - Update effect count in root `CLAUDE.md` (currently says 19)

## IPixelCanvas API reference (quick)
```cpp
void setPixel(int16_t x, int16_t y, CRGB color);
CRGB getPixel(int16_t x, int16_t y);
int16_t getWidth();   // 32
int16_t getHeight();  // 8
void fill(CRGB color);
void clear();
```
