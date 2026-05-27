
# Flasher Online

Disponible en navegadores Google Chrome y Microsoft Edge.
Si flasheas tu reloj Ulanzi por primera vez, necesitas marcar "erase".

El flasher instala tanto el firmware como la interfaz web (SPA) en un solo paso.

## Flasher para Ulanzi TC001 y builds personalizados

<iframe src="/svitrix-firmware-XE1E/ulanzi_flasher/index.html?lang=es" width="100%" height="400" frameborder="0" style="border: 1px solid var(--vp-c-border); border-radius: 8px;"></iframe>

## Qué se flashea

| Partición | Archivo | Offset | Descripción |
|-----------|---------|--------|-------------|
| Bootloader | `bootloader.bin` | 0x1000 | Bootloader del ESP32 |
| Particiones | `partitions.bin` | 0x8000 | Tabla de particiones |
| Firmware | `firmware.bin` | 0x10000 | Firmware SVITRIX |
| LittleFS | `littlefs.bin` | 0x3C0000 | Interfaz web (SPA) + archivos por defecto |
