---
description: Analyze firmware binary size and partition layout in detail
---

Perform a detailed analysis of the firmware flash usage.

> Compute the current usage from the build output below — do NOT rely on any baked-in number. Project rule: flash is comfortable; do not panic-optimize unless `> 90%`.

## Steps

1. **Build firmware** (if not recently built):
   ```bash
   pio run -e ulanzi
   ```

2. **Get firmware binary size:**
   ```bash
   stat -f%z .pio/build/ulanzi/firmware.bin 2>/dev/null || stat -c%s .pio/build/ulanzi/firmware.bin
   ```

3. **Parse build output** for section sizes:
   Look for the RAM/Flash usage line in the build output, e.g.:
   ```
   RAM:   [===       ]  XX.X% (used XXXXX bytes from 327680 bytes)
   Flash: [========= ]  XX.X% (used XXXXXXX bytes from 3932160 bytes)
   ```

4. **Read partition table:**
   ```bash
   cat svitrix_partition.csv
   ```
   Show each partition: name, type, offset, size.

5. **Analyze LittleFS usage:**
   ```bash
   ls -la data/web/
   ```
   Total size of web assets in LittleFS.

6. **Section breakdown** (if available):
   ```bash
   pio run -e ulanzi -t size
   ```
   Show .text, .data, .rodata, .bss sizes.

7. **Report:**
   ```
   === Flash Usage Report ===

   Firmware partition:  3,932 KB total
   Firmware binary:     X,XXX KB (XX.X%)
   Free space:          XXX KB

   LittleFS partition:  256 KB total
   Web assets:          XXX KB
   Icons/data:          XXX KB

   RAM usage:           XXX KB / 320 KB (XX.X%)

   Status: OK | WARNING (>90%) | CRITICAL (>95%)
   ```

8. **If CRITICAL**, suggest optimization strategies:
   - Check for unused effects in [src/effects/EffectRegistry.cpp](../../src/effects/EffectRegistry.cpp) — drop unused entries
   - Check for large string literals or lookup tables
   - Review `lib_deps` in [platformio.ini](../../platformio.ini) for unused libraries
   - Consider `-Os` vs `-Oz` optimization
   - Check that games are excluded by `build_src_filter` in [platformio.ini](../../platformio.ini)
