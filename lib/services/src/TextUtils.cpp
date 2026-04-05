#include "TextUtils.h"

static const UniFont *activeFont = nullptr;

void setTextFont(const UniFont *font)
{
    activeFont = font;
}

float getTextWidth(const char *text, byte textCase, bool uppercaseLetters)
{
    if (!activeFont)
        return 0;
    return getUnicodeTextWidth(*activeFont, text, textCase, uppercaseLetters);
}
