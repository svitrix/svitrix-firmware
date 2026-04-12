#include "LayoutEngine.h"

LayoutMetrics LayoutEngine::computeLayout(
    IconLayout layout,
    uint16_t textWidth,
    int16_t iconWidth,
    int16_t matrixWidth)
{
    LayoutMetrics m = {};
    m.iconY = 0;

    switch (layout)
    {
    case IconLayout::Right:
        m.hasIcon = true;
        m.iconOnRight = true;
        m.iconX = matrixWidth - iconWidth;
        m.textStartX = 0;
        m.textAvailableWidth = matrixWidth - iconWidth;
        m.textCenterX = (m.textAvailableWidth - textWidth) / 2;
        break;

    case IconLayout::None:
        m.hasIcon = false;
        m.iconOnRight = false;
        m.iconX = -1;
        m.textStartX = 0;
        m.textAvailableWidth = matrixWidth;
        m.textCenterX = (matrixWidth - textWidth) / 2;
        break;

    case IconLayout::Left:
    default:
        m.hasIcon = true;
        m.iconOnRight = false;
        m.iconX = 0;
        m.textStartX = iconWidth + 1;
        m.textAvailableWidth = matrixWidth - iconWidth;
        m.textCenterX = ((m.textAvailableWidth - textWidth) / 2) + iconWidth + 1;
        break;
    }

    return m;
}

String layoutToString(IconLayout layout)
{
    switch (layout)
    {
    case IconLayout::Right: return "right";
    case IconLayout::None:  return "none";
    case IconLayout::Left:
    default:                return "left";
    }
}

IconLayout layoutFromString(const String& str)
{
    if (str == "right") return IconLayout::Right;
    if (str == "none")  return IconLayout::None;
    return IconLayout::Left;
}
