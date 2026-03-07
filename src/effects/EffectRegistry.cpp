#include "EffectRegistry.h"
#include "WaveEffects.h"
#include "PatternEffects.h"
#include "ParticleEffects.h"
#include "GameEffects.h"
#include "PaletteUtils.h"
#include <ArduinoJson.h>

Effect effects[] = {
    {"Fade", Fade, EffectSettings(1, RainbowColors_p, true)},
    {"MovingLine", MovingLine, EffectSettings(1, RainbowColors_p, true)},
    {"BrickBreaker", BrickBreakerEffect, EffectSettings()},
    {"PingPong", PingPongEffect, EffectSettings(8, RainbowColors_p, true)},
    {"Radar", RadarEffect, EffectSettings(1, RainbowColors_p, true)},
    {"Checkerboard", CheckerboardEffect, EffectSettings(1, RainbowColors_p, true)},
    {"Fireworks", Fireworks, EffectSettings(1, RainbowColors_p)},
    {"PlasmaCloud", PlasmaCloudEffect, EffectSettings(3, RainbowColors_p, true)},
    {"Ripple", RippleEffect, EffectSettings(3, RainbowColors_p, true)},
    {"Snake", SnakeGame, EffectSettings(3, RainbowColors_p, true)},
    {"Pacifica", Pacifica, EffectSettings(3, OceanColors_p, true)},
    {"TheaterChase", TheaterChase, EffectSettings(3, RainbowColors_p, true)},
    {"Plasma", Plasma, EffectSettings(2, RainbowColors_p, true)},
    {"Matrix", Matrix, EffectSettings(8, ForestColors_p, false)},
    {"SwirlIn", SwirlIn, EffectSettings(5, RainbowColors_p, true)},
    {"SwirlOut", SwirlOut, EffectSettings(5, RainbowColors_p, true)},
    {"LookingEyes", LookingEyes, EffectSettings()},
    {"TwinklingStars", TwinklingStars, EffectSettings(4, OceanColors_p, true)},
    {"ColorWaves", ColorWaves, EffectSettings(5, RainbowColors_p, true)},
};

void callEffect(IPixelCanvas& canvas, int16_t x, int16_t y, uint8_t index)
{
    if (index < kNumEffects)
        effects[index].func(canvas, x, y, &effects[index].settings);
}

void resetAllEffectState()
{
    resetWaveEffectState();
    resetPatternEffectState();
    resetParticleEffectState();
    resetGameEffectState();
}

int getEffectIndex(const String& name)
{
    for (uint8_t i = 0; i < kNumEffects; i++)
        if (effects[i].name == name)
            return i;
    return -1;
}

void updateEffectSettings(uint8_t index, const String& json)
{
    if (index >= kNumEffects)
        return;

    StaticJsonDocument<2048> doc;
    if (deserializeJson(doc, json))
        return;

    auto& s = effects[index].settings;

    if (doc.containsKey("speed"))
        s.speed = doc["speed"];
    if (doc.containsKey("palette"))
        s.palette = getPalette(doc["palette"]);
    if (doc.containsKey("blend"))
        s.blend = doc["blend"].as<bool>();
}
