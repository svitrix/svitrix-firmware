
# Online flasher
  
Available in Google Chrome and Microsoft Edge browsers.  
If you flash your Ulanzi Clock the first time you need to check "erase".

The flasher installs both the firmware and the web interface (SPA) in a single step.

## Ulanzi TC001 and custom builds flasher  

<iframe src="/svitrix-firmware/ulanzi_flasher/index.html" width="100%" height="400" frameborder="0" style="border: 1px solid var(--vp-c-border); border-radius: 8px;"></iframe>

## What gets flashed

| Partition | File | Offset | Description |
|-----------|------|--------|-------------|
| Bootloader | `bootloader.bin` | 0x1000 | ESP32 bootloader |
| Partitions | `partitions.bin` | 0x8000 | Partition table |
| Firmware | `firmware.bin` | 0x10000 | SVITRIX firmware |
| LittleFS | `littlefs.bin` | 0x3C0000 | Web interface (SPA) + default files |

