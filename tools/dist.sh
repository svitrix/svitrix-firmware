#!/bin/bash
# Build firmware + SPA + LittleFS image and collect all artifacts into dist/
# Also updates the online flasher in docs/public/ulanzi_flasher/firmware/
set -e

cd "$(dirname "$0")/.."

MKLITTLEFS="$HOME/.platformio/packages/tool-mklittlefs/mklittlefs"
LITTLEFS_SIZE=0x40000  # 256 KB (from svitrix_partition.csv)
FLASHER_DIR="docs/public/ulanzi_flasher/firmware"

echo "=== Building SPA ==="
cd web && npm run build && cd ..

echo "=== Building firmware ==="
pio run -e ulanzi

echo "=== Building LittleFS image ==="
if [ -f "$MKLITTLEFS" ]; then
    $MKLITTLEFS -c data -s $LITTLEFS_SIZE -p 256 -b 4096 .pio/build/ulanzi/littlefs.bin
    echo "LittleFS image created"
else
    echo "ERROR: mklittlefs not found, run 'pio run -t uploadfs' once to install it"
    exit 1
fi

echo "=== Collecting artifacts ==="
mkdir -p dist
cp .pio/build/ulanzi/firmware.bin dist/
cp .pio/build/ulanzi/bootloader.bin dist/
cp .pio/build/ulanzi/partitions.bin dist/
cp .pio/build/ulanzi/littlefs.bin dist/

echo "=== Updating online flasher ==="
cp dist/firmware.bin "$FLASHER_DIR/"
cp dist/bootloader.bin "$FLASHER_DIR/"
cp dist/partitions.bin "$FLASHER_DIR/"
cp dist/littlefs.bin "$FLASHER_DIR/"

echo ""
echo "=== dist/ ==="
ls -lh dist/
echo ""
echo "Flash layout (ESP Web Tools manifest):"
echo "  0x1000    bootloader.bin   $(du -h dist/bootloader.bin | cut -f1)"
echo "  0x8000    partitions.bin   $(du -h dist/partitions.bin | cut -f1)"
echo "  0x10000   firmware.bin     $(du -h dist/firmware.bin | cut -f1)"
echo "  0x3C0000  littlefs.bin     $(du -h dist/littlefs.bin | cut -f1)"
echo ""
echo "Done! Online flasher updated."
