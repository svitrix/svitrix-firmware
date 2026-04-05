/**
 * @file DisplayRenderer.cpp
 * @brief Rendering engine implementation — text, drawing, charts, bitmaps.
 *
 * Extracted from DisplayManager_Text.cpp + DisplayManager_Drawing.cpp
 * as part of Phase 11.1 (split DisplayManager_ god-object).
 *
 * All methods access shared globals (matrix, leds, cursor, textColor)
 * via DisplayManager_internal.h — unchanged from the original code.
 */
#include "DisplayRenderer.h"
#include "DisplayManager_internal.h"
#include "Globals.h"
#include "Functions.h"
#include "ColorUtils.h"
#include <SvitrixFont.h>
#include <UnicodeFont.h>
#include "Overlays.h"
#include <TJpg_Decoder.h>
#include <ArduinoJson.h>

// ═══════════════════════════════════════════════════════════════════════
// TJpg_Decoder callback (free function, used by DisplayManager::setup)
// ═══════════════════════════════════════════════════════════════════════

/// TJpg_Decoder callback — draws a decoded JPEG tile to the matrix pixel-by-pixel.
bool jpg_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap)
{
    uint16_t bitmapIndex = 0;
    for (uint16_t row = 0; row < h; row++)
    {
        for (uint16_t col = 0; col < w; col++)
        {
            matrix->drawPixel(x + col, y + row, bitmap[bitmapIndex++]);
        }
    }
    return 0;
}

// ═══════════════════════════════════════════════════════════════════════
// Core buffer ops
// ═══════════════════════════════════════════════════════════════════════

void DisplayRenderer_::clear()
{
    matrix->clear();
}

void DisplayRenderer_::show()
{
    matrix->show();
}

// ═══════════════════════════════════════════════════════════════════════
// Text rendering (from DisplayManager_Text.cpp)
// ═══════════════════════════════════════════════════════════════════════

void DisplayRenderer_::resetTextColor()
{
    setTextColor(colorConfig.textColor);
}

/// Renders text at the given position, optionally centered horizontally on the 32px display.
/// Applies uppercase transformation based on global setting or explicit textCase.
/// @param textCase  0=use global setting, 1=force uppercase, 2=as-is.
void DisplayRenderer_::printText(int16_t x, int16_t y, const char *text, bool centered, byte textCase)
{
    if (centered)
    {
        uint16_t textWidth = getTextWidth(text, textCase);
        int16_t textX = ((32 - textWidth) / 2);
        setCursor(textX, y);
    }
    else
    {
        setCursor(x, y);
    }

    bool forceUpper = (displayConfig.uppercaseLetters && textCase == 0) || textCase == 1;
    if (forceUpper)
    {
        // Uppercase conversion: only applies to ASCII a-z in UTF-8 strings
        size_t length = strlen(text);
        char upperText[length + 1];
        for (size_t i = 0; i < length; ++i)
        {
            upperText[i] = toupper(static_cast<unsigned char>(text[i]));
        }
        upperText[length] = '\0';
        matrixPrint(upperText);
    }
    else
    {
        matrixPrint(text);
    }
}

/// Renders text with per-character rainbow colors (HSV hue cycling).
/// The hue offset auto-increments each call, creating an animated rainbow effect.
void DisplayRenderer_::HSVtext(int16_t x, int16_t y, const char *text, bool clear, byte textCase)
{
    if (clear)
        matrix->clear();
    static uint8_t hueOffset = 0;
    bool forceUpper = (displayConfig.uppercaseLetters && textCase == 0) || textCase == 1;
    uint16_t charCount = utf8Length(text);
    uint16_t xpos = 0;
    uint16_t charIdx = 0;
    const char *p = text;
    uint16_t cp;

    while ((cp = utf8NextCodepoint(p)) != 0)
    {
        if (forceUpper && cp >= 'a' && cp <= 'z')
            cp -= 32;

        uint8_t hue = map(charIdx, 0, charCount, 0, 360) + hueOffset;
        setTextColor(hsvToRgb(hue, 255, 255));
        setCursor(xpos + x, y);
        matrixPrintGlyph(cp);
        xpos += getGlyphAdvance(SvitrixFont, cp, 4);
        charIdx++;
    }
    hueOffset++;
    if (clear)
        matrix->show();
}

/// Renders text with a linear color gradient interpolated between color1 and color2.
/// Each character gets a color based on its position in the string (0.0 to 1.0).
void DisplayRenderer_::GradientText(int16_t x, int16_t y, const char *text, int color1, int color2, bool clear, byte textCase)
{
    if (clear)
        matrix->clear();

    bool forceUpper = (displayConfig.uppercaseLetters && textCase == 0) || textCase == 1;
    uint16_t charCount = utf8Length(text);
    uint16_t xpos = 0;
    uint16_t charIdx = 0;
    const char *p = text;
    uint16_t cp;

    while ((cp = utf8NextCodepoint(p)) != 0)
    {
        if (forceUpper && cp >= 'a' && cp <= 'z')
            cp -= 32;

        float t = (charCount > 1) ? static_cast<float>(charIdx) / (charCount - 1) : 0.0f;
        uint32_t TC = interpolateColor(color1, color2, t);
        setTextColor(TC);

        setCursor(xpos + x, y);
        matrixPrintGlyph(cp);
        xpos += getGlyphAdvance(SvitrixFont, cp, 4);
        charIdx++;
    }

    if (clear)
        matrix->show();
}

/// Renders text with the appropriate color mode: rainbow, gradient, or solid.
/// Also applies fade and blink effects for solid color mode via TextEffect().
void DisplayRenderer_::renderColoredText(int16_t x, int16_t y, const char *text,
                                         bool rainbow, int gradient0, int gradient1,
                                         uint32_t color, uint32_t fade, uint32_t blink, byte textCase)
{
    if (rainbow)
    {
        HSVtext(x, y, text, false, textCase);
    }
    else if (gradient0 > -1 && gradient1 > -1)
    {
        GradientText(x, y, text, gradient0, gradient1, false, textCase);
    }
    else
    {
        setTextColor(TextEffect(color, fade, blink));
        printText(x, y, text, false, textCase);
    }
}

/// Core glyph renderer — draws a single character from SvitrixFont bitmap at the current cursor.
/// Renders a single Unicode glyph at the current cursor position.
/// Advances cursor_x by the glyph's xAdvance after drawing.
void DisplayRenderer_::matrixPrintGlyph(uint16_t codepoint)
{
    const UniGlyph *glyph = findGlyph(SvitrixFont, codepoint);
    if (!glyph)
    {
        // Fallback: try '?' glyph
        glyph = findGlyph(SvitrixFont, '?');
        if (!glyph)
            return;
    }

    const uint8_t *bitmap = SvitrixFont.bitmap;
    uint16_t bo = UNI_READ_WORD(&glyph->bitmapOffset);
    uint8_t w = UNI_READ_BYTE(&glyph->width);
    uint8_t h = UNI_READ_BYTE(&glyph->height);
    int8_t xo = static_cast<int8_t>(UNI_READ_BYTE(&glyph->xOffset));
    int8_t yo = static_cast<int8_t>(UNI_READ_BYTE(&glyph->yOffset));

    uint8_t xx, yy, bits = 0, bit = 0;
    for (yy = 0; yy < h; yy++)
    {
        for (xx = 0; xx < w; xx++)
        {
            if (!(bit++ & 7))
            {
                bits = UNI_READ_BYTE(&bitmap[bo++]);
            }
            if (bits & 0x80)
            {
                matrix->drawPixel(cursor_x + xo + xx, cursor_y + yo + yy, textColor);
            }
            bits <<= 1;
        }
    }

    cursor_x += UNI_READ_BYTE(&glyph->xAdvance);
}

void DisplayRenderer_::matrixPrint(const char *str)
{
    const char *p = str;
    uint16_t cp;
    while ((cp = utf8NextCodepoint(p)) != 0)
    {
        if (cp == '\n')
        {
            cursor_y += SvitrixFont.yAdvance;
            cursor_x = 0;
        }
        else if (cp != '\r')
        {
            matrixPrintGlyph(cp);
        }
    }
}

void DisplayRenderer_::matrixPrint(double number, uint8_t digits)
{
    String output;

    if (isnan(number))
    {
        output = "nan";
    }
    else if (isinf(number))
    {
        output = "inf";
    }
    else if (number > 4294967040.0)
    {
        output = "ovf";
    }
    else if (number < -4294967040.0)
    {
        output = "ovf";
    }
    else
    {
        if (number < 0.0)
        {
            output += '-';
            number = -number;
        }
        double rounding = 0.5;
        for (uint8_t i = 0; i < digits; ++i)
        {
            rounding /= 10.0;
        }
        number += rounding;

        unsigned long int_part = static_cast<unsigned long>(number);
        output += String(int_part);

        if (digits > 0)
        {
            output += '.';
        }

        double remainder = number - static_cast<double>(int_part);
        while (digits-- > 0)
        {
            remainder *= 10.0;
            int toPrint = static_cast<int>(remainder);
            output += String(toPrint);
            remainder -= toPrint;
        }
    }

    matrixPrint(output);
}

void DisplayRenderer_::matrixPrint(String str)
{
    matrixPrint(str.c_str());
}

void DisplayRenderer_::matrixPrint(char *str)
{
    matrixPrint(static_cast<const char *>(str));
}

void DisplayRenderer_::matrixPrint(char str[], size_t length)
{
    char temp[length + 1];
    strncpy(temp, str, length);
    temp[length] = '\0';
    matrixPrint(temp);
}

void DisplayRenderer_::setCursor(int16_t x, int16_t y)
{
    cursor_x = x;
    cursor_y = y;
}

void DisplayRenderer_::setTextColor(uint32_t color)
{
    textColor = color;
}

// ═══════════════════════════════════════════════════════════════════════
// Drawing primitives (from DisplayManager_Drawing.cpp)
// ═══════════════════════════════════════════════════════════════════════

void DisplayRenderer_::drawBMP(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h)
{
    matrix->drawRGBBitmap(x, y, bitmap, w, h);
}

void DisplayRenderer_::drawJPG(uint16_t x, uint16_t y, fs::File jpgFile)
{
    TJpgDec.drawFsJpg(x, y, jpgFile);
}

void DisplayRenderer_::drawJPG(int32_t x, int32_t y, const uint8_t jpeg_data[], uint32_t data_size)
{
    TJpgDec.drawJpg(x, y, jpeg_data, data_size);
}

/// Draws a single-pixel-high progress bar spanning from x to the right edge of the matrix.
/// @param progress  Percentage (0-100) of the bar to fill with pColor over pbColor background.
void DisplayRenderer_::drawProgressBar(int16_t x, int16_t y, int progress, uint32_t pColor, uint32_t pbColor)
{
    int available_length = 32 - x;
    int leds_for_progress = (progress * available_length) / 100;
    drawLine(x, y, x + available_length - 1, y, pbColor);
    if (leds_for_progress > 0)
        drawLine(x, y, x + leds_for_progress - 1, y, pColor);
}

/// Draws centered dot indicators at the bottom row (y=7) showing current page position.
/// Active dot uses the given color; inactive dots use dim gray (0x666666).
void DisplayRenderer_::drawMenuIndicator(int cur, int total, uint32_t color)
{
    int menuItemWidth = 1;
    int totalWidth = total * menuItemWidth + (total - 1);
    int leftMargin = (MATRIX_WIDTH - totalWidth) / 2;
    int pixelSpacing = 1;
    for (int i = 0; i < total; i++)
    {
        int x = leftMargin + i * (menuItemWidth + pixelSpacing);
        if (i == cur)
        {
            drawLine(x, 7, x + menuItemWidth - 1, 7, color);
        }
        else
        {
            drawLine(x, 7, x + menuItemWidth - 1, 7, 0x666666);
        }
    }
}

/// Draws a vertical bar chart. Bars are auto-sized to fill available width (23px with icon, 32px without).
void DisplayRenderer_::drawBarChart(int16_t x, int16_t y, const int data[], byte dataSize, bool withIcon, uint32_t color, uint32_t barBG)
{
    int availableWidth = withIcon ? (32 - 9) : 32;
    int gap = 1;
    int totalGapsWidth = (dataSize - 1) * gap;
    int barWidth = (availableWidth - totalGapsWidth) / dataSize;
    int startX = withIcon ? 9 : 0;

    for (int i = 0; i < dataSize; i++)
    {
        int x1 = x + startX + i * (barWidth + gap);
        int barHeight = data[i];
        int y1 = (barHeight > 0) ? (8 - barHeight) : 8;

        if (barBG > 0)
        {
            drawFilledRect(x1, y, barWidth, 8, barBG);
        }

        if (barHeight > 0)
        {
            drawFilledRect(x1, y1 + y, barWidth, barHeight, color);
        }
    }
}

/// Draws a line chart connecting data points with straight line segments.
void DisplayRenderer_::drawLineChart(int16_t x, int16_t y, const int data[], byte dataSize, bool withIcon, uint32_t color)
{
    int availableWidth = withIcon ? (32 - 9) : 32;
    int startX = withIcon ? 9 : 0;
    float xStep = static_cast<float>(availableWidth) / static_cast<float>(dataSize - 1);
    int lastX = x + startX;
    int lastY = y + 8 - data[0];
    for (int i = 1; i < dataSize; i++)
    {
        int x1 = x + startX + static_cast<int>(xStep * i);
        int y1 = y + 8 - data[i];
        drawLine(lastX, lastY, x1, y1, color);
        lastX = x1;
        lastY = y1;
    }
}

/// Interprets a JSON array of draw commands and renders them on the matrix.
/// Supported commands: dp (pixel), dl (line), dr (rect), df (filled rect),
/// dc (circle), dfc (filled circle), dt (text), db (bitmap).
void DisplayRenderer_::processDrawInstructions(int16_t xOffset, int16_t yOffset, String& drawInstructions)
{
    static DynamicJsonDocument doc(8192);
    doc.clear();
    DeserializationError error = deserializeJson(doc, drawInstructions);

    if (error)
    {
        return;
    }

    if (!doc.is<JsonArray>())
    {
        return;
    }

    JsonArray instructions = doc.as<JsonArray>();
    for (JsonObject instruction : instructions)
    {
        for (auto kvp : instruction)
        {
            String command = kvp.key().c_str();

            JsonArray params = kvp.value().as<JsonArray>();
            if (command == "dp")
            {
                int x = params[0].as<int>();
                int y = params[1].as<int>();
                auto color1 = params[2];
                uint32_t color = getColorFromJsonVariant(color1, colorConfig.textColor);
                matrix->drawPixel(x + xOffset, y + yOffset, color);
            }
            else if (command == "dl")
            {
                int x0 = params[0].as<int>();
                int y0 = params[1].as<int>();
                int x1 = params[2].as<int>();
                int y1 = params[3].as<int>();
                auto color2 = params[4];
                uint32_t color = getColorFromJsonVariant(color2, colorConfig.textColor);
                drawLine(x0 + xOffset, y0 + yOffset, x1 + xOffset, y1 + yOffset, color);
            }
            else if (command == "dr")
            {
                int x = params[0].as<int>();
                int y = params[1].as<int>();
                int w = params[2].as<int>();
                int h = params[3].as<int>();
                auto color3 = params[4];
                uint32_t color = getColorFromJsonVariant(color3, colorConfig.textColor);
                drawRect(x + xOffset, y + yOffset, w, h, color);
            }
            else if (command == "df")
            {
                int x = params[0].as<int>();
                int y = params[1].as<int>();
                int w = params[2].as<int>();
                int h = params[3].as<int>();
                auto color4 = params[4];
                uint32_t color = getColorFromJsonVariant(color4, colorConfig.textColor);
                drawFilledRect(x + xOffset, y + yOffset, w, h, color);
            }
            else if (command == "dc")
            {
                int x = params[0].as<int>();
                int y = params[1].as<int>();
                int r = params[2].as<int>();
                auto color5 = params[3];
                uint32_t color = getColorFromJsonVariant(color5, colorConfig.textColor);
                drawCircle(x + xOffset, y + yOffset, r, color);
            }
            else if (command == "dfc")
            {
                double x = params[0].as<double>();
                double y = params[1].as<double>();
                double r = params[2].as<double>();
                auto color6 = params[3];
                uint32_t color = getColorFromJsonVariant(color6, colorConfig.textColor);
                fillCircle(x + xOffset, y + yOffset, r, color);
            }
            else if (command == "dt")
            {
                int x = params[0].as<int>();
                int y = params[1].as<int>();
                String text = params[2].as<String>();
                auto color7 = params[3];
                uint32_t color = getColorFromJsonVariant(color7, colorConfig.textColor);
                setTextColor(color);
                setCursor(x + xOffset, y + yOffset + 5);
                matrixPrint(text.c_str());
            }
            else if (command == "db")
            {
                int x = params[0].as<int>();
                int y = params[1].as<int>();
                int width = params[2].as<int>();
                int height = params[3].as<int>();
                if (width <= 0 || height <= 0 || width > 32 || height > 8)
                    continue;
                std::vector<uint32_t> bitmap(width * height);
                JsonArray colorArray = params[4].as<JsonArray>();
                size_t i = 0;
                for (const auto& color : colorArray)
                {
                    if (i >= bitmap.size())
                        break;
                    bitmap[i] = color.as<uint32_t>();
                    i++;
                }
                size_t bitmapIndex = 0;
                for (int row = 0; row < height; ++row)
                {
                    for (int col = 0; col < width; ++col)
                    {
                        matrix->drawPixel(x + col + xOffset, y + row + yOffset, bitmap[bitmapIndex++]);
                    }
                }
            }
        }
    }
}

// ── Drawing primitives ──────────────────────────────────────────────

void DisplayRenderer_::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color)
{
    for (int16_t i = x; i < x + w; i++)
    {
        matrix->drawPixel(i, y, color);
        matrix->drawPixel(i, y + h - 1, color);
    }
    for (int16_t i = y; i < y + h; i++)
    {
        matrix->drawPixel(x, i, color);
        matrix->drawPixel(x + w - 1, i, color);
    }
}

void DisplayRenderer_::drawFastVLine(int16_t x, int16_t y, int16_t h, uint32_t color)
{
    drawLine(x, y, x, y + h - 1, color);
}

void DisplayRenderer_::drawFilledRect(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color)
{
    for (int16_t i = x; i < x + w; i++)
    {
        drawFastVLine(i, y, h, color);
    }
}

/// Draws a line between two points using Bresenham's algorithm.
void DisplayRenderer_::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint32_t color)
{
    int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy, e2;

    while (true)
    {
        matrix->drawPixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void DisplayRenderer_::drawRGBBitmap(int16_t x, int16_t y, const uint32_t *bitmap, int16_t w, int16_t h)
{
    for (int16_t i = 0; i < w; i++)
    {
        for (int16_t j = 0; j < h; j++)
        {
            uint32_t pixelColor = bitmap[j * w + i];
            matrix->drawPixel(x + i, y + j, pixelColor);
        }
    }
}

void DisplayRenderer_::drawPixel(int16_t x0, int16_t y0, uint32_t color)
{
    matrix->drawPixel(x0, y0, color);
}

/// Draws a circle outline using the midpoint circle algorithm.
void DisplayRenderer_::drawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color)
{
    int16_t x = r;
    int16_t y = 0;
    int16_t p = 1 - r;

    if (r == 0)
    {
        matrix->drawPixel(x0, y0, color);
        return;
    }

    matrix->drawPixel(x0 + r, y0, color);
    matrix->drawPixel(x0 - r, y0, color);
    matrix->drawPixel(x0, y0 + r, color);
    matrix->drawPixel(x0, y0 - r, color);
    while (x > y)
    {
        y++;

        if (p <= 0)
            p = p + 2 * y + 1;
        else
        {
            x--;
            p = p + 2 * y - 2 * x + 1;
        }

        if (x < y)
            break;

        matrix->drawPixel(x0 + x, y0 - y, color);
        matrix->drawPixel(x0 - x, y0 - y, color);
        matrix->drawPixel(x0 + x, y0 + y, color);
        matrix->drawPixel(x0 - x, y0 + y, color);

        if (x != y)
        {
            matrix->drawPixel(x0 + y, y0 - x, color);
            matrix->drawPixel(x0 - y, y0 - x, color);
            matrix->drawPixel(x0 + y, y0 + x, color);
            matrix->drawPixel(x0 - y, y0 + x, color);
        }
    }
}

/// Draws a filled circle using horizontal scan lines from the midpoint algorithm.
void DisplayRenderer_::fillCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color)
{
    matrix->drawPixel(x0, y0, color);
    drawLine(x0 - r, y0, x0 + r, y0, color);
    int16_t x = r;
    int16_t y = 0;
    int16_t p = 1 - r;
    while (x > y)
    {
        y++;

        if (p <= 0)
            p = p + 2 * y + 1;
        else
        {
            x--;
            p = p + 2 * y - 2 * x + 1;
        }

        if (x < y)
            break;

        drawLine(x0 - x, y0 - y, x0 + x, y0 - y, color);
        drawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);

        if (x != y)
        {
            drawLine(x0 - y, y0 - x, x0 + y, y0 - x, color);
            drawLine(x0 - y, y0 + x, x0 + y, y0 + x, color);
        }
    }
}

CRGB DisplayRenderer_::getPixelColor(int16_t x, int16_t y)
{
    int index = matrix->XY(x, y);
    return leds[index];
}
