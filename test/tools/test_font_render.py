#!/usr/bin/env python3
"""
Visual test for SvitrixFont.h — renders every glyph as ASCII art.
Usage: python3 test/tools/test_font_render.py
"""

import os
import re
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
PROJECT_ROOT = os.path.join(SCRIPT_DIR, "..", "..")
FONT_FILE = os.path.join(PROJECT_ROOT, "src", "SvitrixFont.h")

def parse_font(path):
    with open(path, "r") as f:
        text = f.read()

    # Parse bitmap bytes
    bitmaps_match = re.search(
        r"SvitrixBitmaps\[\]\s*PROGMEM\s*=\s*\{(.*?)\};", text, re.DOTALL
    )
    if not bitmaps_match:
        sys.exit("Could not parse SvitrixBitmaps")

    raw = bitmaps_match.group(1)
    # Strip comments to avoid picking up hex values from them (e.g. /*0x41 A*/)
    raw_no_comments = re.sub(r"/\*.*?\*/", "", raw, flags=re.DOTALL)
    bitmap_bytes = []
    for m in re.finditer(r"0x([0-9A-Fa-f]{2})", raw_no_comments):
        bitmap_bytes.append(int(m.group(1), 16))

    # Parse glyph descriptors + comments
    glyphs_match = re.search(
        r"SvitrixFontGlyphs\[\]\s*PROGMEM\s*=\s*\{(.*?)\};", text, re.DOTALL
    )
    if not glyphs_match:
        sys.exit("Could not parse SvitrixFontGlyphs")

    raw_glyphs = glyphs_match.group(1)
    glyphs = []
    for m in re.finditer(
        r"\{\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*(-?\d+)\s*,\s*(-?\d+)\s*\}"
        r"\s*,?\s*/\*.*?0x([0-9A-Fa-f]+)\s+(.*?)\s*\*/",
        raw_glyphs,
    ):
        offset = int(m.group(1))
        width = int(m.group(2))
        height = int(m.group(3))
        advance = int(m.group(4))
        x_off = int(m.group(5))
        y_off = int(m.group(6))
        code = int(m.group(7), 16)
        name = m.group(8).strip()
        glyphs.append({
            "offset": offset,
            "width": width,
            "height": height,
            "advance": advance,
            "x_off": x_off,
            "y_off": y_off,
            "code": code,
            "name": name,
        })

    return bitmap_bytes, glyphs


def render_glyph(bitmap_bytes, glyph):
    """Render a single glyph as a list of strings using block characters.
    Trims to the actual visible width (advance - 1 spacing)."""
    offset = glyph["offset"]
    width = glyph["width"]
    height = glyph["height"]
    advance = glyph["advance"]

    # Show only meaningful columns (advance width gives total including spacing)
    show_cols = max(advance - 1, 1)

    rows = []
    bit_index = 0
    for row in range(height):
        line = ""
        for col in range(width):
            byte_idx = offset + (bit_index // 8)
            bit_pos = 7 - (bit_index % 8)
            if byte_idx < len(bitmap_bytes):
                pixel = (bitmap_bytes[byte_idx] >> bit_pos) & 1
            else:
                pixel = 0
            if col < show_cols:
                line += "\u2588" if pixel else "\u00b7"  # █ or ·
            bit_index += 1
        rows.append(line)
    return rows


def print_glyphs_grid(bitmap_bytes, glyphs, title, cols=8):
    """Print glyphs in a grid layout."""
    print(f"\n{'=' * 60}")
    print(f"  {title}")
    print(f"{'=' * 60}")

    cell_w = 14  # width of each cell in terminal chars

    for start in range(0, len(glyphs), cols):
        batch = glyphs[start : start + cols]

        # Header line: code + name
        header = ""
        for g in batch:
            label = f"0x{g['code']:02X} {g['name']}"
            header += label.ljust(cell_w)
        print(f"\n{header}")
        print("-" * (cell_w * len(batch)))

        # Render all glyphs in batch
        rendered = [render_glyph(bitmap_bytes, g) for g in batch]
        max_h = max(len(r) for r in rendered) if rendered else 0

        for row in range(max_h):
            line = ""
            for i, r in enumerate(rendered):
                if row < len(r):
                    cell = r[row]
                else:
                    cell = ""
                line += cell.ljust(cell_w)
            print(line)

        # Advance info
        adv_line = ""
        for g in batch:
            adv_line += f"w={g['advance']}".ljust(cell_w)
        print(adv_line)


def main():
    bitmap_bytes, glyphs = parse_font(FONT_FILE)
    print(f"Parsed {len(bitmap_bytes)} bitmap bytes, {len(glyphs)} glyphs\n")

    # Categorize glyphs
    ascii_printable = [g for g in glyphs if 0x20 <= g["code"] <= 0x7E]
    cyrillic = [g for g in glyphs if 0x7F <= g["code"] <= 0xA0]
    latin_ext = [g for g in glyphs if 0xA1 <= g["code"] <= 0xFF]

    print_glyphs_grid(bitmap_bytes, ascii_printable, "ASCII (0x20 - 0x7E)", cols=10)
    print_glyphs_grid(bitmap_bytes, cyrillic, "Cyrillic (А-Я, Ґ, Є)", cols=8)
    print_glyphs_grid(bitmap_bytes, latin_ext, "Latin Extended (0xA1 - 0xFF)", cols=8)

    # Summary
    print(f"\n{'=' * 60}")
    print(f"  SUMMARY")
    print(f"{'=' * 60}")
    print(f"  Total glyphs:    {len(glyphs)}")
    print(f"  ASCII:           {len(ascii_printable)}")
    print(f"  Cyrillic:        {len(cyrillic)}")
    print(f"  Latin Extended:  {len(latin_ext)}")
    print(f"  Bitmap bytes:    {len(bitmap_bytes)}")


if __name__ == "__main__":
    main()
