#include "TextUtils.h"
#include <cctype>

// Pixel-width lookup for every glyph in SvitrixFont.
// Key = code point (ASCII + extended), Value = width in pixels.
// Built from the font bitmap: most glyphs are 4px, narrow ones (I, i, space) are 2px,
// wide ones (M, W) are 6px. Used by getTextWidth() to calculate scroll distances.
std::map<int, uint16_t> CharMap = {
    {32, 2}, {33, 2}, {34, 4}, {35, 4}, {36, 4}, {37, 4}, {38, 4}, {39, 2}, {40, 3}, {41, 3}, {42, 4}, {43, 4}, {44, 3}, {45, 4}, {46, 2}, {47, 4}, {48, 4}, {49, 4}, {50, 4}, {51, 4}, {52, 4}, {53, 4}, {54, 4}, {55, 4}, {56, 4}, {57, 4}, {58, 2}, {59, 3}, {60, 4}, {61, 4}, {62, 4}, {63, 4}, {64, 4}, {65, 4}, {66, 4}, {67, 4}, {68, 4}, {69, 4}, {70, 4}, {71, 4}, {72, 4}, {73, 2}, {74, 4}, {75, 4}, {76, 4}, {77, 6}, {78, 5}, {79, 4}, {80, 4}, {81, 5}, {82, 4}, {83, 4}, {84, 4}, {85, 4}, {86, 4}, {87, 6}, {88, 4}, {89, 4}, {90, 4}, {91, 4}, {92, 4}, {93, 4}, {94, 4}, {95, 4}, {96, 3}, {97, 4}, {98, 4}, {99, 4}, {100, 4}, {101, 4}, {102, 4}, {103, 4}, {104, 4}, {105, 2}, {106, 4}, {107, 4}, {108, 4}, {109, 4}, {110, 4}, {111, 4}, {112, 4}, {113, 4}, {114, 4}, {115, 4}, {116, 4}, {117, 4}, {118, 4}, {119, 4}, {120, 4}, {121, 4}, {122, 4}, {123, 4}, {124, 2}, {125, 4}, {126, 4}, {161, 2}, {162, 4}, {163, 4}, {164, 4}, {165, 4}, {166, 2}, {167, 4}, {168, 4}, {169, 4}, {170, 4}, {171, 3}, {172, 4}, {173, 3}, {174, 4}, {175, 4}, {176, 3}, {177, 4}, {178, 4}, {179, 4}, {180, 4}, {181, 4}, {182, 4}, {183, 4}, {184, 4}, {185, 2}, {186, 4}, {187, 3}, {188, 4}, {189, 4}, {190, 4}, {191, 4}, {192, 4}, {193, 4}, {194, 4}, {195, 4}, {196, 4}, {197, 4}, {198, 4}, {199, 4}, {200, 4}, {201, 4}, {202, 4}, {203, 4}, {204, 4}, {205, 4}, {206, 4}, {207, 4}, {208, 4}, {209, 4}, {210, 4}, {211, 4}, {212, 4}, {213, 4}, {214, 4}, {215, 4}, {216, 4}, {217, 4}, {218, 4}, {219, 4}, {220, 4}, {221, 4}, {222, 4}, {223, 4}, {224, 4}, {225, 4}, {226, 4}, {227, 4}, {228, 4}, {229, 4}, {230, 4}, {231, 4}, {232, 4}, {233, 4}, {234, 4}, {235, 4}, {236, 3}, {237, 3}, {238, 4}, {239, 4}, {240, 4}, {241, 4}, {242, 4}, {243, 4}, {244, 4}, {245, 4}, {246, 4}, {247, 4}, {248, 4}, {249, 4}, {250, 4}, {251, 4}, {252, 4}, {253, 4}, {254, 4}, {255, 4}, {285, 2}, {338, 4}, {339, 4}, {352, 4}, {353, 4}, {376, 4}, {381, 4}, {382, 4}, {3748, 2}, {5024, 2}, {8226, 2}, {8230, 4}, {8364, 4}, {65533, 4}};

// Sum pixel widths of all characters in a string.
// Characters found in CharMap use their exact width; characters NOT in the map
// (typically decoded Cyrillic glyphs) use hardcoded fallback widths based on
// the actual font bitmap measurements: 6px for wide Cyrillic (Ш,Щ,Ж,Ы,Ю,Э,Ъ),
// 7px for extra-wide (Ф), 5px for medium (Д,Л,И,Й), 4px default.
float getTextWidth(const char *text, byte textCase, bool uppercaseLetters)
{
    float width = 0;
    for (const char *c = text; *c != '\0'; ++c)
    {
        char current_char = *c;
        // Apply case conversion: textCase==1 forces upper, textCase==0 defers to uppercaseLetters flag.
        if ((uppercaseLetters && textCase == 0) || textCase == 1)
        {
            current_char = toupper(current_char);
        }
        if (CharMap.count(current_char) > 0)
        {
            width += CharMap[current_char];
        }
        else
        {
            // Fallback widths for decoded Cyrillic glyphs not in CharMap.
            // These byte values come from utf8ascii() remapping.
            if (current_char == 0x83 ||
                current_char == 0x85 ||
                current_char == 0x8B ||
                current_char == 0x93 ||
                current_char == 0x97 ||
                current_char == 0x9A ||
                current_char == 0x9D)
            {
                width += 6; // Wide Cyrillic: Ш, Щ, Ж, etc.
            }
            else if (current_char == 0x98)
            {
                width += 7; // Extra-wide: Ф
            }
            else if (current_char == 0x99 ||
                     current_char == 0x95 ||
                     current_char == 0x87 ||
                     current_char == 0x88)
            {
                width += 5; // Medium Cyrillic: Д, Л, И, Й
            }
            else
            {
                width += 4; // Default for any unknown glyph
            }
        }
    }
    return width;
}

// Stateful UTF-8 decoder. Remembers the previous leading byte in `c1`.
// UTF-8 multi-byte sequences: the first byte (0xC0-0xDF for 2-byte, 0xE0-0xEF for 3-byte)
// is stored in c1, and the next call with the continuation byte performs the mapping.
//
// The output is a single-byte glyph index for the SvitrixFont, NOT a Unicode code point.
// Note: switch cases intentionally fall through (no break) for C4→C5 and D0→D1→D2 blocks —
// this is by design since some continuation bytes overlap between prefix groups.
static byte c1;
byte utf8ascii(byte ascii)
{
    // ASCII range: pass through directly, reset state.
    if (ascii < 128)
    {
        c1 = 0;
        return (ascii);
    }
    byte last = c1;
    c1 = ascii;
    switch (last)
    {

    // Latin supplement (¡ through ÿ): pass continuation byte as-is.
    case 0xC2:
        return (ascii);
        break;

    // Latin Extended-A (À through ÿ): OR with 0xC0 to shift into 0xC0-0xFF range.
    // Special cases: ó→'o', Ó→'O' (map to ASCII equivalent).
    case 0xC3:
        if (ascii == 0xB3) return 0x6F; // ó → o
        if (ascii == 0x93) return 0x4F; // Ó → O
        return (ascii | 0xC0);
        break;

    // Polish characters (C4 prefix): map to nearest ASCII equivalent.
    // ą→a, Ą→A, ć→c, Ć→C, ę→e, Ę→E
    case 0xC4:
        if (ascii == 0x85) return 0x61; // ą → a
        if (ascii == 0x84) return 0x41; // Ą → A
        if (ascii == 0x87) return 0x63; // ć → c
        if (ascii == 0x86) return 0x43; // Ć → C
        if (ascii == 0x99) return 0x65; // ę → e
        if (ascii == 0x98) return 0x45; // Ę → E
        // Fall through to C5 for shared continuation bytes.

    // Polish characters (C5 prefix): ł→l, Ł→L, ń→N, Ń→N, Ś→S, ź/Ź/ż/Ż, ś→s
    case 0xC5:
        if (ascii == 0x82) return 0x6C; // ł → l
        if (ascii == 0x81) return 0x4C; // Ł → L
        if (ascii == 0x84) return 0x6E; // ń → n
        if (ascii == 0x83) return 0x4E; // Ń → N
        if (ascii == 0x9A) return 0x53; // Ś → S
        if (ascii == 0xBC) return 0x7A; // ż → z
        if (ascii == 0xBB) return 0x5A; // Ż → Z
        if (ascii == 0xBA) return 0x7A; // ź → z
        if (ascii == 0xB9) return 0x5A; // Ź → Z
        if (ascii == 0x9B) return 0x73; // ś → s
        // Fall through.

    // Euro sign: UTF-8 is E2 82 AC → mapped to glyph 0xB6.
    case 0x82:
        if (ascii == 0xAC)
            return (0xB6);
        // Fall through.

    // Cyrillic uppercase block (D0 prefix):
    //   Ё→0x84, Є→0xA0, І→0xA1, Ї→0xEF
    //   А-П (0x90-0xAF): subtract 17 → 0x7F-0x9E
    //   Р-Я (0xB0-0xBF): subtract 49 → 0x81-0x90
    case 0xD0:
        if (ascii == 0x81)
            return 0x84;  // Ё
        if (ascii == 0x84)
            return 0xA0;  // Є
        if (ascii == 0x86)
            return 0xA1;  // І
        if (ascii == 0x87)
            return 0xEF;  // Ї

        if (ascii >= 0x90 && ascii <= 0xAF)
            return (ascii)-17; // А-П

        if (ascii >= 0xB0 && ascii <= 0xBF)
            return (ascii)-49; // Р-Я
        // Fall through.

    // Cyrillic lowercase block (D1 prefix):
    //   ё→0x84, є→0xA0, і→0xA1, ї→0xEF
    //   а-п (0x80-0x8F): add 15 → 0x8F-0x9E
    case 0xD1:
        if (ascii == 0x91)
            return 0x84;  // ё
        if (ascii == 0x94)
            return 0xA0;  // є
        if (ascii == 0x96)
            return 0xA1;  // і
        if (ascii == 0x97)
            return 0xEF;  // ї

        if (ascii >= 0x80 && ascii <= 0x8F)
            return (ascii) + 15; // а-п
        // Fall through.

    // Cyrillic extended (D2 prefix): Ґ/ґ → 0x9F
    case 0xD2:
        if (ascii == 0x90)
            return 0x9F;  // Ґ

        if (ascii == 0x91)
            return 0x9F;  // ґ
    }
    return (0); // Unrecognized continuation byte → drop it.
}

// Batch-convert an entire String from UTF-8 to single-byte font encoding.
// Feeds each byte through the stateful utf8ascii(byte) decoder.
// Zero results (continuation bytes that don't produce output) are skipped.
String utf8ascii(String s)
{
    String r = "";
    char c;
    for (unsigned int i = 0; i < s.length(); i++)
    {
        c = utf8ascii(s.charAt(i));
        if (c != 0)
            r += c;
    }
    return r;
}
