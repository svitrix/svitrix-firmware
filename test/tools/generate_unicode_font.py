#!/usr/bin/env python3
"""
Generate a Unicode-aware SvitrixFont.h from the existing GFXfont-based font.

Reads the old SvitrixFont.h, extracts bitmap data and glyph metadata,
maps old single-byte indices to real Unicode codepoints, adds lowercase
Cyrillic aliases, sorts by codepoint, and outputs the new file.
"""

import re
import sys
import os

# ── Mapping from old glyph index (0x20-based) to Unicode codepoint ──

# Index 0-94: ASCII 0x20-0x7E → identity
# Index 95-128: Cyrillic block at 0x7F-0xA0
# Index 129-223: Latin-1 Supplement 0xA1-0xFF → identity

# Cyrillic uppercase mapping: old_byte → Unicode codepoint
# The font stores Cyrillic at positions 0x7F-0xA0
CYRILLIC_MAP = {
    0x7F: 0x0410,  # А
    0x80: 0x0411,  # Б
    0x81: 0x0412,  # В
    0x82: 0x0413,  # Г
    0x83: 0x0414,  # Д
    0x84: 0x0415,  # Е (also used for Ё)
    0x85: 0x0416,  # Ж
    0x86: 0x0417,  # З
    0x87: 0x0418,  # И
    0x88: 0x0419,  # Й
    0x89: 0x041A,  # К
    0x8A: 0x041B,  # Л
    0x8B: 0x041C,  # М
    0x8C: 0x041D,  # Н
    0x8D: 0x041E,  # О
    0x8E: 0x041F,  # П
    0x8F: 0x0420,  # Р
    0x90: 0x0421,  # С
    0x91: 0x0422,  # Т
    0x92: 0x0423,  # У
    0x93: 0x0424,  # Ф
    0x94: 0x0425,  # Х
    0x95: 0x0426,  # Ц
    0x96: 0x0427,  # Ч
    0x97: 0x0428,  # Ш
    0x98: 0x0429,  # Щ
    0x99: 0x042A,  # Ъ
    0x9A: 0x042B,  # Ы
    0x9B: 0x042C,  # Ь
    0x9C: 0x042D,  # Э
    0x9D: 0x042E,  # Ю
    0x9E: 0x042F,  # Я
    0x9F: 0x0490,  # Ґ
    0xA0: 0x0404,  # Є
}

# Unicode names for known codepoints
UNICODE_NAMES = {
    0x0020: 'space', 0x0021: 'exclam', 0x0022: 'quotedbl', 0x0023: 'numbersign',
    0x0024: 'dollar', 0x0025: 'percent', 0x0026: 'ampersand', 0x0027: 'quotesingle',
    0x0028: 'parenleft', 0x0029: 'parenright', 0x002A: 'asterisk', 0x002B: 'plus',
    0x002C: 'comma', 0x002D: 'hyphen', 0x002E: 'period', 0x002F: 'slash',
    0x003A: 'colon', 0x003B: 'semicolon', 0x003C: 'less', 0x003D: 'equal',
    0x003E: 'greater', 0x003F: 'question', 0x0040: 'at',
    0x005B: 'bracketleft', 0x005C: 'backslash', 0x005D: 'bracketright',
    0x005E: 'asciicircum', 0x005F: 'underscore', 0x0060: 'grave',
    0x007B: 'braceleft', 0x007C: 'bar', 0x007D: 'braceright', 0x007E: 'asciitilde',
    # Cyrillic
    0x0410: 'А', 0x0411: 'Б', 0x0412: 'В', 0x0413: 'Г', 0x0414: 'Д',
    0x0415: 'Е', 0x0416: 'Ж', 0x0417: 'З', 0x0418: 'И', 0x0419: 'Й',
    0x041A: 'К', 0x041B: 'Л', 0x041C: 'М', 0x041D: 'Н', 0x041E: 'О',
    0x041F: 'П', 0x0420: 'Р', 0x0421: 'С', 0x0422: 'Т', 0x0423: 'У',
    0x0424: 'Ф', 0x0425: 'Х', 0x0426: 'Ц', 0x0427: 'Ч', 0x0428: 'Ш',
    0x0429: 'Щ', 0x042A: 'Ъ', 0x042B: 'Ы', 0x042C: 'Ь', 0x042D: 'Э',
    0x042E: 'Ю', 0x042F: 'Я',
    0x0430: 'а', 0x0431: 'б', 0x0432: 'в', 0x0433: 'г', 0x0434: 'д',
    0x0435: 'е', 0x0436: 'ж', 0x0437: 'з', 0x0438: 'и', 0x0439: 'й',
    0x043A: 'к', 0x043B: 'л', 0x043C: 'м', 0x043D: 'н', 0x043E: 'о',
    0x043F: 'п', 0x0440: 'р', 0x0441: 'с', 0x0442: 'т', 0x0443: 'у',
    0x0444: 'ф', 0x0445: 'х', 0x0446: 'ц', 0x0447: 'ч', 0x0448: 'ш',
    0x0449: 'щ', 0x044A: 'ъ', 0x044B: 'ы', 0x044C: 'ь', 0x044D: 'э',
    0x044E: 'ю', 0x044F: 'я',
    0x0490: 'Ґ', 0x0491: 'ґ',
    0x0404: 'Є', 0x0454: 'є',
    0x0406: 'І', 0x0456: 'і',
    0x0407: 'Ї', 0x0457: 'ї',
    0x0401: 'Ё', 0x0451: 'ё',
    # Latin Extended-A (aliases to base ASCII)
    0x0100: 'Ā', 0x0101: 'ā', 0x0102: 'Ă', 0x0103: 'ă',
    0x0104: 'Ą', 0x0105: 'ą', 0x0106: 'Ć', 0x0107: 'ć',
    0x010C: 'Č', 0x010D: 'č', 0x010E: 'Ď', 0x010F: 'ď',
    0x0110: 'Đ', 0x0111: 'đ', 0x0112: 'Ē', 0x0113: 'ē',
    0x0118: 'Ę', 0x0119: 'ę', 0x011A: 'Ě', 0x011B: 'ě',
    0x011E: 'Ğ', 0x011F: 'ğ', 0x0122: 'Ģ', 0x0123: 'ģ',
    0x012A: 'Ī', 0x012B: 'ī', 0x0130: 'İ', 0x0131: 'ı',
    0x0136: 'Ķ', 0x0137: 'ķ', 0x013B: 'Ļ', 0x013C: 'ļ',
    0x0141: 'Ł', 0x0142: 'ł', 0x0143: 'Ń', 0x0144: 'ń',
    0x0145: 'Ņ', 0x0146: 'ņ', 0x0147: 'Ň', 0x0148: 'ň',
    0x014C: 'Ō', 0x014D: 'ō', 0x0150: 'Ő', 0x0151: 'ő',
    0x0152: 'Œ', 0x0153: 'œ', 0x0158: 'Ř', 0x0159: 'ř',
    0x015A: 'Ś', 0x015B: 'ś', 0x015E: 'Ş', 0x015F: 'ş',
    0x0160: 'Š', 0x0161: 'š', 0x0164: 'Ť', 0x0165: 'ť',
    0x016A: 'Ū', 0x016B: 'ū', 0x016E: 'Ů', 0x016F: 'ů',
    0x0170: 'Ű', 0x0171: 'ű', 0x0179: 'Ź', 0x017A: 'ź',
    0x017B: 'Ż', 0x017C: 'ż', 0x017D: 'Ž', 0x017E: 'ž',
    0x0218: 'Ș', 0x0219: 'ș', 0x021A: 'Ț', 0x021B: 'ț',
    # Latin-1 Supplement
    0x00A1: 'exclamdown', 0x00A2: 'cent', 0x00A3: 'sterling', 0x00A4: 'currency',
    0x00A5: 'yen', 0x00A6: 'brokenbar', 0x00A7: 'section', 0x00A8: 'dieresis',
    0x00A9: 'copyright', 0x00AA: 'ordfeminine', 0x00AB: 'guillemotleft',
    0x00AC: 'logicalnot', 0x00AD: 'softhyphen', 0x00AE: 'registered',
    0x00AF: 'macron', 0x00B0: 'degree', 0x00B1: 'plusminus',
    0x00B2: 'twosuperior', 0x00B3: 'threesuperior', 0x00B4: 'acute',
    0x00B5: 'mu', 0x00B6: 'paragraph', 0x00B7: 'periodcentered',
    0x00B8: 'cedilla', 0x00B9: 'onesuperior', 0x00BA: 'ordmasculine',
    0x00BB: 'guillemotright', 0x00BC: 'onequarter', 0x00BD: 'onehalf',
    0x00BE: 'threequarters', 0x00BF: 'questiondown',
    0x00C0: 'Agrave', 0x00C1: 'Aacute', 0x00C2: 'Acircumflex', 0x00C3: 'Atilde',
    0x00C4: 'Adieresis', 0x00C5: 'Aring', 0x00C6: 'AE', 0x00C7: 'Ccedilla',
    0x00C8: 'Egrave', 0x00C9: 'Eacute', 0x00CA: 'Ecircumflex', 0x00CB: 'Edieresis',
    0x00CC: 'Igrave', 0x00CD: 'Iacute', 0x00CE: 'Icircumflex', 0x00CF: 'Idieresis',
    0x00D0: 'Eth', 0x00D1: 'Ntilde', 0x00D2: 'Ograve', 0x00D3: 'Oacute',
    0x00D4: 'Ocircumflex', 0x00D5: 'Otilde', 0x00D6: 'Odieresis', 0x00D7: 'multiply',
    0x00D8: 'Oslash', 0x00D9: 'Ugrave', 0x00DA: 'Uacute', 0x00DB: 'Ucircumflex',
    0x00DC: 'Udieresis', 0x00DD: 'Yacute', 0x00DE: 'Thorn', 0x00DF: 'germandbls',
    0x00E0: 'agrave', 0x00E1: 'aacute', 0x00E2: 'acircumflex', 0x00E3: 'atilde',
    0x00E4: 'adieresis', 0x00E5: 'aring', 0x00E6: 'ae', 0x00E7: 'ccedilla',
    0x00E8: 'egrave', 0x00E9: 'eacute', 0x00EA: 'ecircumflex', 0x00EB: 'edieresis',
    0x00EC: 'igrave', 0x00ED: 'iacute', 0x00EE: 'icircumflex', 0x00EF: 'idieresis',
    0x00F0: 'eth', 0x00F1: 'ntilde', 0x00F2: 'ograve', 0x00F3: 'oacute',
    0x00F4: 'ocircumflex', 0x00F5: 'otilde', 0x00F6: 'odieresis', 0x00F7: 'divide',
    0x00F8: 'oslash', 0x00F9: 'ugrave', 0x00FA: 'uacute', 0x00FB: 'ucircumflex',
    0x00FC: 'udieresis', 0x00FD: 'yacute', 0x00FE: 'thorn', 0x00FF: 'ydieresis',
}


def get_name(cp):
    """Get a display name for a codepoint."""
    if cp in UNICODE_NAMES:
        return UNICODE_NAMES[cp]
    if 0x30 <= cp <= 0x39:
        return chr(cp)
    if 0x41 <= cp <= 0x5A or 0x61 <= cp <= 0x7A:
        return chr(cp)
    return f'U+{cp:04X}'


def parse_old_glyphs(font_path):
    """Parse glyph entries from SvitrixFont.h (supports both old GFXglyph and new UniGlyph format)."""
    with open(font_path, 'r') as f:
        content = f.read()

    # Try new UniGlyph format first: {codepoint, bitmapOffset, width, height, xAdvance, xOffset, yOffset}
    if 'SvitrixGlyphs[]' in content:
        glyph_pattern = re.compile(
            r'\{0x([0-9A-Fa-f]+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+)\}',
        )
        start = content.find('SvitrixGlyphs[]')
        end = content.find('};', start)
        glyph_section = content[start:end]

        glyphs = []
        for m in glyph_pattern.finditer(glyph_section):
            glyphs.append({
                'bitmapOffset': int(m.group(2)),
                'width': int(m.group(3)),
                'height': int(m.group(4)),
                'xAdvance': int(m.group(5)),
                'xOffset': int(m.group(6)),
                'yOffset': int(m.group(7)),
                '_codepoint': int(m.group(1), 16),  # Track original codepoint
            })
        return glyphs

    # Fallback: old GFXglyph format: {bitmapOffset, width, height, xAdvance, xOffset, yOffset}
    glyph_pattern = re.compile(
        r'\{(\d+),\s*(\d+),\s*(\d+),\s*(\d+),\s*(-?\d+),\s*(-?\d+)\}',
    )
    start = content.find('SvitrixFontGlyphs[]')
    end = content.find('};', start)
    glyph_section = content[start:end]

    glyphs = []
    for m in glyph_pattern.finditer(glyph_section):
        glyphs.append({
            'bitmapOffset': int(m.group(1)),
            'width': int(m.group(2)),
            'height': int(m.group(3)),
            'xAdvance': int(m.group(4)),
            'xOffset': int(m.group(5)),
            'yOffset': int(m.group(6)),
        })

    return glyphs


def old_index_to_unicode(index):
    """Map old glyph index (0-based, starting at 0x20) to Unicode codepoint."""
    old_byte = index + 0x20

    # ASCII range: identity
    if 0x20 <= old_byte <= 0x7E:
        return old_byte

    # Cyrillic block
    if old_byte in CYRILLIC_MAP:
        return CYRILLIC_MAP[old_byte]

    # Latin-1 Supplement
    if 0xA1 <= old_byte <= 0xFF:
        return old_byte

    return None


def build_unicode_glyphs(old_glyphs):
    """Build Unicode glyph list from old glyphs, adding Cyrillic and Latin Extended aliases."""
    result = []

    # Check if glyphs already have codepoints (new format)
    has_codepoints = '_codepoint' in old_glyphs[0] if old_glyphs else False

    if has_codepoints:
        # New format: filter to only original glyphs (ASCII + Latin-1 + Cyrillic uppercase)
        # Skip previously generated aliases — we'll regenerate them
        for g in old_glyphs:
            cp = g['_codepoint']
            # Keep only base glyphs (not aliases we added before)
            is_ascii = 0x0020 <= cp <= 0x007E
            is_latin1 = 0x00A1 <= cp <= 0x00FF
            is_cyr_upper = 0x0410 <= cp <= 0x042F
            is_cyr_special = cp in (0x0490, 0x0404)
            if is_ascii or is_latin1 or is_cyr_upper or is_cyr_special:
                result.append({
                    'codepoint': cp,
                    'bitmapOffset': g['bitmapOffset'],
                    'width': g['width'],
                    'height': g['height'],
                    'xAdvance': g['xAdvance'],
                    'xOffset': g['xOffset'],
                    'yOffset': g['yOffset'],
                })
    else:
        # Old format: map by index
        for i, g in enumerate(old_glyphs):
            cp = old_index_to_unicode(i)
            if cp is not None:
                result.append({
                    'codepoint': cp,
                    'bitmapOffset': g['bitmapOffset'],
                    'width': g['width'],
                    'height': g['height'],
                    'xAdvance': g['xAdvance'],
                    'xOffset': g['xOffset'],
                    'yOffset': g['yOffset'],
                })

    # Add lowercase Cyrillic aliases (а-я → same bitmaps as А-Я)
    # Build a map of uppercase codepoint → glyph data
    upper_map = {g['codepoint']: g for g in result if 0x0410 <= g['codepoint'] <= 0x042F}

    for upper_cp in range(0x0410, 0x0430):  # А-Я
        lower_cp = upper_cp + 0x20  # а-я
        if upper_cp in upper_map:
            g = upper_map[upper_cp]
            result.append({
                'codepoint': lower_cp,
                'bitmapOffset': g['bitmapOffset'],
                'width': g['width'],
                'height': g['height'],
                'xAdvance': g['xAdvance'],
                'xOffset': g['xOffset'],
                'yOffset': g['yOffset'],
            })

    # Add lowercase Ukrainian special aliases
    # Ґ(0x0490) → ґ(0x0491)
    special_aliases = [
        (0x0490, 0x0491),  # Ґ → ґ
        (0x0404, 0x0454),  # Є → є
    ]
    cp_map = {g['codepoint']: g for g in result}
    for upper_cp, lower_cp in special_aliases:
        if upper_cp in cp_map and lower_cp not in cp_map:
            g = cp_map[upper_cp]
            result.append({
                'codepoint': lower_cp,
                'bitmapOffset': g['bitmapOffset'],
                'width': g['width'],
                'height': g['height'],
                'xAdvance': g['xAdvance'],
                'xOffset': g['xOffset'],
                'yOffset': g['yOffset'],
            })

    # Add Ё(0x0401)/ё(0x0451) → alias to Е(0x0415) glyph
    if 0x0415 in cp_map:
        g = cp_map[0x0415]
        for cp in [0x0401, 0x0451]:
            if cp not in cp_map:
                result.append({
                    'codepoint': cp,
                    'bitmapOffset': g['bitmapOffset'],
                    'width': g['width'],
                    'height': g['height'],
                    'xAdvance': g['xAdvance'],
                    'xOffset': g['xOffset'],
                    'yOffset': g['yOffset'],
                })

    # Add І(0x0406)/і(0x0456) → alias to I(0x0049) glyph (Latin I)
    if 0x0049 in cp_map:
        g = cp_map[0x0049]
        for cp in [0x0406, 0x0456]:
            if cp not in cp_map:
                result.append({
                    'codepoint': cp,
                    'bitmapOffset': g['bitmapOffset'],
                    'width': g['width'],
                    'height': g['height'],
                    'xAdvance': g['xAdvance'],
                    'xOffset': g['xOffset'],
                    'yOffset': g['yOffset'],
                })

    # Add Ї(0x0407)/ї(0x0457) — the old font had a glyph at 0xEF for this
    idieresis_cp = 0x00EF  # ï (idieresis) in Latin-1
    if idieresis_cp in cp_map:
        g = cp_map[idieresis_cp]
        for cp in [0x0407, 0x0457]:
            if cp not in cp_map:
                result.append({
                    'codepoint': cp,
                    'bitmapOffset': g['bitmapOffset'],
                    'width': g['width'],
                    'height': g['height'],
                    'xAdvance': g['xAdvance'],
                    'xOffset': g['xOffset'],
                    'yOffset': g['yOffset'],
                })

    # ── Latin Extended aliases ──────────────────────────────────────
    # On a 3×5 pixel grid, diacritical marks can't be displayed.
    # Map accented/modified Latin letters to their ASCII base glyph.
    # Format: (new_codepoint, base_ascii_codepoint)
    LATIN_EXTENDED_ALIASES = [
        # Croatian / Serbian Latin / Slovenian
        (0x010C, ord('C')),  # Č
        (0x010D, ord('c')),  # č
        (0x0106, ord('C')),  # Ć
        (0x0107, ord('c')),  # ć
        (0x0110, ord('D')),  # Đ
        (0x0111, ord('d')),  # đ
        (0x0160, ord('S')),  # Š
        (0x0161, ord('s')),  # š
        (0x017D, ord('Z')),  # Ž
        (0x017E, ord('z')),  # ž
        # Czech / Slovak
        (0x0158, ord('R')),  # Ř
        (0x0159, ord('r')),  # ř
        (0x0147, ord('N')),  # Ň
        (0x0148, ord('n')),  # ň
        (0x0164, ord('T')),  # Ť
        (0x0165, ord('t')),  # ť
        (0x010E, ord('D')),  # Ď
        (0x010F, ord('d')),  # ď
        (0x016E, ord('U')),  # Ů
        (0x016F, ord('u')),  # ů
        (0x011A, ord('E')),  # Ě
        (0x011B, ord('e')),  # ě
        # Hungarian
        (0x0150, ord('O')),  # Ő
        (0x0151, ord('o')),  # ő
        (0x0170, ord('U')),  # Ű
        (0x0171, ord('u')),  # ű
        # Romanian
        (0x0218, ord('S')),  # Ș
        (0x0219, ord('s')),  # ș
        (0x021A, ord('T')),  # Ț
        (0x021B, ord('t')),  # ț
        (0x0102, ord('A')),  # Ă
        (0x0103, ord('a')),  # ă
        # Turkish
        (0x011E, ord('G')),  # Ğ
        (0x011F, ord('g')),  # ğ
        (0x0130, ord('I')),  # İ
        (0x0131, ord('i')),  # ı (dotless i)
        (0x015E, ord('S')),  # Ş
        (0x015F, ord('s')),  # ş
        # Polish (now proper aliases instead of utf8ascii hack)
        (0x0104, ord('A')),  # Ą
        (0x0105, ord('a')),  # ą
        (0x0106, ord('C')),  # Ć (already above, dedup by cp_map check)
        (0x0107, ord('c')),  # ć
        (0x0118, ord('E')),  # Ę
        (0x0119, ord('e')),  # ę
        (0x0141, ord('L')),  # Ł
        (0x0142, ord('l')),  # ł
        (0x0143, ord('N')),  # Ń
        (0x0144, ord('n')),  # ń
        (0x015A, ord('S')),  # Ś
        (0x015B, ord('s')),  # ś
        (0x0179, ord('Z')),  # Ź
        (0x017A, ord('z')),  # ź
        (0x017B, ord('Z')),  # Ż
        (0x017C, ord('z')),  # ż
        # Latvian / Lithuanian
        (0x0100, ord('A')),  # Ā
        (0x0101, ord('a')),  # ā
        (0x0112, ord('E')),  # Ē
        (0x0113, ord('e')),  # ē
        (0x012A, ord('I')),  # Ī
        (0x012B, ord('i')),  # ī
        (0x016A, ord('U')),  # Ū
        (0x016B, ord('u')),  # ū
        (0x0122, ord('G')),  # Ģ
        (0x0123, ord('g')),  # ģ
        (0x0136, ord('K')),  # Ķ
        (0x0137, ord('k')),  # ķ
        (0x013B, ord('L')),  # Ļ
        (0x013C, ord('l')),  # ļ
        (0x0145, ord('N')),  # Ņ
        (0x0146, ord('n')),  # ņ
        (0x010C, ord('C')),  # Č (already above)
        (0x010D, ord('c')),  # č
        (0x0160, ord('S')),  # Š (already above)
        (0x0161, ord('s')),  # š
        (0x017D, ord('Z')),  # Ž (already above)
        (0x017E, ord('z')),  # ž
        # Estonian
        (0x014C, ord('O')),  # Ō
        (0x014D, ord('o')),  # ō
        # Scandinavian (beyond Latin-1 which is already covered)
        (0x0152, ord('O')),  # Œ → O (ligature approximation)
        (0x0153, ord('o')),  # œ → o
    ]

    cp_map = {g['codepoint']: g for g in result}  # Refresh after Cyrillic aliases
    for new_cp, base_cp in LATIN_EXTENDED_ALIASES:
        if new_cp not in cp_map and base_cp in cp_map:
            g = cp_map[base_cp]
            result.append({
                'codepoint': new_cp,
                'bitmapOffset': g['bitmapOffset'],
                'width': g['width'],
                'height': g['height'],
                'xAdvance': g['xAdvance'],
                'xOffset': g['xOffset'],
                'yOffset': g['yOffset'],
            })
            cp_map[new_cp] = result[-1]  # Prevent duplicates

    # Sort by codepoint
    result.sort(key=lambda g: g['codepoint'])

    return result


def extract_bitmap_section(font_path):
    """Extract the raw bitmap array source code from the old file."""
    with open(font_path, 'r') as f:
        content = f.read()

    # Find bitmap array
    start = content.find('const uint8_t SvitrixBitmaps[] PROGMEM = {')
    end = content.find('};', start) + 2
    return content[start:end]


def extract_license(font_path):
    """Extract the license header from the old file."""
    with open(font_path, 'r') as f:
        content = f.read()
    end = content.find('// SvitrixFont Version')
    return content[:end].rstrip()


def generate_font_header(font_path, output_path):
    """Generate the new SvitrixFont.h."""
    old_glyphs = parse_old_glyphs(font_path)
    print(f"Parsed {len(old_glyphs)} old glyphs")

    unicode_glyphs = build_unicode_glyphs(old_glyphs)
    print(f"Generated {len(unicode_glyphs)} Unicode glyphs (with aliases)")

    license_text = extract_license(font_path)
    bitmap_section = extract_bitmap_section(font_path)

    lines = []
    lines.append(license_text)
    lines.append('')
    lines.append('// SvitrixFont Version 20260405 — Unicode sparse glyph table')
    lines.append('')
    lines.append('#pragma once')
    lines.append('#include "UnicodeFont.h"')
    lines.append('')

    # Bitmap array (unchanged)
    lines.append(bitmap_section)
    lines.append('')

    # Unicode glyph table (compact: 5 bytes per glyph)
    # packed byte: height[7:5] | xAdvance[4:2] | yOffIdx[1:0]
    # yOffset index: {-5:0, -4:1, -3:2, -1:3}
    yoff_to_idx = {-5: 0, -4: 1, -3: 2, -1: 3}

    lines.append('/* {codepoint, bitmapOffset, packed: h[7:5]|xAdv[4:2]|yOff[1:0]} */')
    lines.append(f'const UniGlyph SvitrixGlyphs[] PROGMEM = {{')

    for i, g in enumerate(unicode_glyphs):
        name = get_name(g['codepoint'])
        h = g['height']
        xa = g['xAdvance']
        yo = g['yOffset']
        assert 0 <= h <= 7, f"height {h} out of range for glyph U+{g['codepoint']:04X}"
        assert 0 <= xa <= 7, f"xAdvance {xa} out of range for glyph U+{g['codepoint']:04X}"
        assert yo in yoff_to_idx, f"yOffset {yo} not in lookup table for glyph U+{g['codepoint']:04X}"
        packed = (h << 5) | (xa << 2) | yoff_to_idx[yo]
        line = (
            f"    {{0x{g['codepoint']:04X}, {g['bitmapOffset']}, 0x{packed:02X}}}, "
            f"/*[{i}] {name} h={h} xa={xa} yo={yo} */"
        )
        lines.append(line)

    lines.append('};')
    lines.append('')

    # Font struct
    lines.append('const UniFont SvitrixFont PROGMEM = {')
    lines.append('    (const uint8_t *)SvitrixBitmaps,')
    lines.append('    (const UniGlyph *)SvitrixGlyphs,')
    lines.append(f'    {len(unicode_glyphs)}, // glyphCount')
    lines.append('    6, // yAdvance')
    lines.append('    8, // bitmapWidth')
    lines.append('    0, // xOffset')
    lines.append('};')
    lines.append('')

    with open(output_path, 'w') as f:
        f.write('\n'.join(lines))

    print(f"Written to {output_path}")

    # Print stats
    ascii_count = sum(1 for g in unicode_glyphs if 0x0020 <= g['codepoint'] <= 0x007E)
    latin1_count = sum(1 for g in unicode_glyphs if 0x00A1 <= g['codepoint'] <= 0x00FF)
    latin_ext_count = sum(1 for g in unicode_glyphs if 0x0100 <= g['codepoint'] <= 0x024F)
    cyr_count = sum(1 for g in unicode_glyphs if 0x0400 <= g['codepoint'] <= 0x04FF)
    print(f"  ASCII: {ascii_count}, Latin-1: {latin1_count}, Latin Ext: {latin_ext_count}, Cyrillic: {cyr_count}")
    print(f"  Total: {len(unicode_glyphs)} glyphs × 5 bytes = {len(unicode_glyphs) * 5} bytes")


if __name__ == '__main__':
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.join(script_dir, '..', '..')
    font_path = os.path.join(project_root, 'src', 'SvitrixFont.h')
    output_path = os.path.join(project_root, 'src', 'SvitrixFont.h')

    if len(sys.argv) > 1:
        font_path = sys.argv[1]
    if len(sys.argv) > 2:
        output_path = sys.argv[2]

    # Safety: backup before overwriting
    if os.path.abspath(font_path) == os.path.abspath(output_path):
        backup_path = font_path + '.bak'
        import shutil
        shutil.copy2(font_path, backup_path)
        print(f"Backed up to {backup_path}")

    generate_font_header(font_path, output_path)
