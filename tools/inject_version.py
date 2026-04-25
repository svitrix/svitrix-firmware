"""
PlatformIO pre-build script: reads firmware version from the `version` file
in the project root and injects it as -DVERSION build flag.

Single source of truth: the `version` file. The release workflow updates it
from the git tag and commits it back to main, so both local and CI builds
produce firmware reporting the correct version.

If `version` is missing or empty, no flag is added — Globals.h falls back to
"dev".

Add to platformio.ini:
    extra_scripts = pre:tools/inject_version.py
"""

import os

Import("env")

version_file = os.path.join(env["PROJECT_DIR"], "version")

if os.path.isfile(version_file):
    with open(version_file, "r", encoding="utf-8") as f:
        version = f.read().strip()
    if version:
        env.Append(CPPDEFINES=[("VERSION", env.StringifyMacro(version))])
        print(f"Firmware version: {version}")
