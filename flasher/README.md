# SVITRIX XE1E - Web Flasher

Flasher web para instalar el firmware SVITRIX en el Ulanzi TC001.

## Configurar GitHub Pages

1. Ve a tu repositorio en GitHub: https://github.com/XE1E/svitrix-firmware-XE1E
2. Click en **Settings** (configuracion)
3. En el menu lateral, click en **Pages**
4. En "Source", selecciona:
   - **Branch:** `main`
   - **Folder:** `/flasher` (o `/root` si no aparece la opcion)
5. Click en **Save**
6. Espera 1-2 minutos

Tu flasher estara disponible en:
```
https://xe1e.github.io/svitrix-firmware-XE1E/
```

## Actualizar Firmware

Despues de compilar el firmware con PlatformIO, copia los archivos:

```bash
# Desde la raiz del proyecto
cp .pio/build/ulanzi/bootloader.bin flasher/firmware/
cp .pio/build/ulanzi/partitions.bin flasher/firmware/
cp .pio/build/ulanzi/firmware.bin flasher/firmware/
cp .pio/build/ulanzi/littlefs.bin flasher/firmware/
```

O en PowerShell:
```powershell
Copy-Item .pio\build\ulanzi\bootloader.bin flasher\firmware\ -Force
Copy-Item .pio\build\ulanzi\partitions.bin flasher\firmware\ -Force
Copy-Item .pio\build\ulanzi\firmware.bin flasher\firmware\ -Force
Copy-Item .pio\build\ulanzi\littlefs.bin flasher\firmware\ -Force
```

Luego haz commit y push:
```bash
git add flasher/firmware/*.bin
git commit -m "Update firmware binaries"
git push
```

## Estructura de Archivos

```
flasher/
├── index.html           # Pagina del flasher
├── README.md            # Este archivo
└── firmware/
    ├── manifest.json    # Configuracion ESP Web Tools
    ├── bootloader.bin   # Bootloader ESP32
    ├── partitions.bin   # Tabla de particiones
    ├── boot_app0.bin    # Boot app
    ├── firmware.bin     # Firmware SVITRIX
    └── littlefs.bin     # Sistema de archivos (SPA)
```

## Requisitos del Navegador

- Google Chrome (recomendado)
- Microsoft Edge
- Opera

**No funciona en:** Firefox, Safari, navegadores moviles.
