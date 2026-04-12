# contrib

Third-party header-only libraries bundled with the project.

## Files

| File | Purpose |
|------|---------|
| `ArtnetWifi.h` | Art-Net DMX receiver/sender (fork of [rstephan/ArtnetWifi](https://github.com/rstephan/ArtnetWifi)) |
| `GifPlayer.h` | GIF decoder for animated icons on the LED matrix |

These are included directly in `src/` rather than `lib/` because they are
single-header files with tight integration to the firmware's rendering pipeline.
