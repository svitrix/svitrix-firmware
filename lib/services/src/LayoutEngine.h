#pragma once

#include <cstdint>

#ifdef UNIT_TEST
#include "Arduino.h"
#else
#include <Arduino.h>
#endif

enum class IconLayout : uint8_t {
    Left  = 0,
    Right = 1,
    None  = 2
};

struct LayoutMetrics {
    int16_t  iconX;
    int16_t  iconY;
    int16_t  textStartX;
    int16_t  textCenterX;
    uint16_t textAvailableWidth;
    bool     hasIcon;
    bool     iconOnRight;
};

class LayoutEngine {
public:
    static LayoutMetrics computeLayout(
        IconLayout layout,
        uint16_t textWidth,
        int16_t iconWidth = 8,
        int16_t matrixWidth = 32
    );
};

String layoutToString(IconLayout layout);
IconLayout layoutFromString(const String& str);
