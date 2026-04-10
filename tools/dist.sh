#!/bin/bash
# Build firmware + SPA and collect all artifacts into dist/
set -e

cd "$(dirname "$0")/.."

echo "=== Building SPA ==="
cd web && npm run build && cd ..

echo "=== Building firmware ==="
pio run -e ulanzi

echo "=== Collecting artifacts ==="
mkdir -p dist
cp .pio/build/ulanzi/firmware.bin dist/
cp data/web/app.js.gz dist/
cp data/web/style.css.gz dist/
cp data/web/index.html.gz dist/

echo ""
echo "=== dist/ ==="
ls -lh dist/
echo ""
echo "Done! All artifacts in dist/"
