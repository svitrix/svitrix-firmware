"""
PlatformIO pre-build script: builds the SPA in web/ before firmware compilation.

Runs `npm run build` in web/ directory, which outputs gzipped files to data/web/.
These files are then uploaded to LittleFS via `pio run -t uploadfs`.

Add to platformio.ini:
    extra_scripts = pre:tools/build_web.py
"""

import subprocess
import sys
import os

Import("env")

web_dir = os.path.join(env["PROJECT_DIR"], "web")
data_dir = os.path.join(env["PROJECT_DIR"], "data", "web")


def build_web(*args, **kwargs):
    if not os.path.exists(os.path.join(web_dir, "package.json")):
        print("WARNING: web/package.json not found, skipping SPA build")
        return

    # Check if node_modules exists
    if not os.path.exists(os.path.join(web_dir, "node_modules")):
        print("Installing web dependencies...")
        result = subprocess.run(
            ["npm", "install"],
            cwd=web_dir,
            capture_output=True,
            text=True,
        )
        if result.returncode != 0:
            print(f"npm install failed:\n{result.stderr}")
            sys.exit(1)

    # Check if rebuild is needed (any src file newer than output)
    src_dir = os.path.join(web_dir, "src")
    app_js = os.path.join(data_dir, "app.js.gz")

    needs_build = not os.path.exists(app_js)
    if not needs_build and os.path.exists(src_dir):
        out_mtime = os.path.getmtime(app_js)
        for root, dirs, files in os.walk(src_dir):
            for f in files:
                if os.path.getmtime(os.path.join(root, f)) > out_mtime:
                    needs_build = True
                    break
            if needs_build:
                break

    if not needs_build:
        print("SPA is up to date, skipping build")
        return

    print("Building SPA...")
    result = subprocess.run(
        ["npm", "run", "build"],
        cwd=web_dir,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print(f"SPA build failed:\n{result.stderr}")
        sys.exit(1)

    print("SPA built successfully")


# Run before firmware build
build_web()
