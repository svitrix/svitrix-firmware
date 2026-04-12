# data

Static data-heavy header and source files that don't belong to any specific module.
Separated from logic code to keep module directories focused.

## Files

| File | Purpose |
|------|---------|
| `SvitrixFont.h` | Unicode sparse glyph table (336 glyphs, binary search lookup) |
| `cert.h` | TLS CA certificate for OTA firmware update server |
| `icons.h` | Built-in icon bitmaps (PROGMEM arrays) |
| `Dictionary.h` / `.cpp` | Key-value string storage utility |
