#include "PaletteUtils.h"
#include <LittleFS.h>

CRGBPalette16 loadPaletteFromLittleFS(const String& paletteName)
{
    File file = LittleFS.open("/PALETTES/" + paletteName + ".txt", "r");
    if (!file)
        return RainbowColors_p;

    CRGBPalette16 palette = RainbowColors_p;
    for (int i = 0; i < 16 && file.available(); i++)
    {
        String line = file.readStringUntil('\n');
        line.trim();
        palette[i] = CRGB(static_cast<uint32_t>(strtol(line.c_str(), nullptr, 16)));
    }
    file.close();
    return palette;
}

CRGBPalette16 getPalette(const JsonVariant& paletteVariant)
{
    if (paletteVariant.is<String>())
    {
        String name = paletteVariant.as<String>();

        struct PaletteEntry
        {
            const char *name;
            CRGBPalette16 palette;
        };
        static const PaletteEntry builtins[] = {
            {"Cloud", CloudColors_p},
            {"Lava", LavaColors_p},
            {"Ocean", OceanColors_p},
            {"Forest", ForestColors_p},
            {"Stripe", RainbowStripeColors_p},
            {"Party", PartyColors_p},
            {"Heat", HeatColors_p},
        };

        for (const auto& entry : builtins)
            if (name == entry.name)
                return entry.palette;

        return loadPaletteFromLittleFS(name);
    }

    if (paletteVariant.is<JsonArray>())
    {
        CRGBPalette16 palette;
        JsonArray array = paletteVariant.as<JsonArray>();
        for (int i = 0; i < 16 && i < static_cast<int>(array.size()); i++)
            palette[i] = CRGB(static_cast<uint32_t>(strtol(array[i].as<String>().c_str(), nullptr, 16)));
        return palette;
    }

    return RainbowColors_p;
}
