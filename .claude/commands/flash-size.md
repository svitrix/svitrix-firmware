---
description: Analyze firmware binary size and partition layout in detail
---

Perform a detailed analysis of the firmware flash usage. This is critical — flash is ~96% full.

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
   - Check for unused effects (19 effects may not all be needed)
   - Check for large string literals or lookup tables
   - Review lib_deps for unused libraries
   - Consider `-Os` vs `-Oz` optimization
   - Check if games are accidentally included (should be excluded by build_src_filter)
