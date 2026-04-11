#include "AppContent.h"
#include "DisplayManager.h"
#include "Functions.h"
#include "Globals.h"
#include "GifPlayer.h"
#include "effects/NeoMatrixCanvas.h"
#include "effects/WeatherOverlay.h"

int renderAppIcon(AppContentBase& app, GifPlayer *gifPlayer,
                  int16_t x, int16_t y, bool noScrolling, bool hasIcon,
                  uint8_t *currentFrame)
{
    int iconWidth = 0;

    if (hasIcon)
    {
        if (app.pushIcon > 0 && !noScrolling)
        {
            if (app.iconPosition < 0 && app.iconWasPushed == false && app.scrollposition > 8)
            {
                app.iconPosition += movementFactor;
            }
            if (app.scrollposition < (9 - app.textOffset) && !app.iconWasPushed)
            {
                app.iconPosition = app.scrollposition - 9 + app.textOffset;

                if (app.iconPosition <= -9 - app.textOffset)
                {
                    app.iconWasPushed = true;
                }
            }
        }

        if (app.isGif)
        {
            if (currentFrame)
            {
                iconWidth = gifPlayer->playGif(x + app.iconPosition + app.iconOffset, y, &app.icon, *currentFrame);
                *currentFrame = gifPlayer->getFrame();
            }
            else
            {
                iconWidth = gifPlayer->playGif(x + app.iconPosition + app.iconOffset, y, &app.icon);
            }
        }
        else
        {
            iconWidth = 8;
            if (!app.jpegDataBuffer.empty())
            {
                DisplayManager.drawJPG(x + app.iconPosition + app.iconOffset, y, app.jpegDataBuffer.data(), app.jpegDataBuffer.size());
            }
            else
            {
                DisplayManager.drawJPG(x + app.iconPosition + app.iconOffset, y, app.icon);
            }
        }

        if (!noScrolling)
        {
            int16_t lineY1 = (app.progress > -1) ? (6 + y) : (7 + y);
            DisplayManager.drawLine(iconWidth + x + app.iconPosition + app.iconOffset, 0 + y,
                                    iconWidth + x + app.iconPosition, lineY1, app.background);
        }
    }

    if (app.drawInstructions.length() > 0)
    {
        DisplayManager.processDrawInstructions(x, y, app.drawInstructions);
    }

    if (app.progress > -1)
    {
        DisplayManager.drawProgressBar((hasIcon ? 8 : 0), 7 + y, app.progress, app.pColor, app.pbColor);
    }

    if (app.barSize > 0)
    {
        DisplayManager.drawBarChart(x, y, app.barData, app.barSize, hasIcon, app.color, app.barBG);
    }

    if (app.lineSize > 0)
    {
        DisplayManager.drawLineChart(x, y, app.lineData, app.lineSize, hasIcon, app.color);
    }

    return iconWidth;
}

void updateScrollAnimation(AppContentBase& app, bool hasIcon,
                           float delayThreshold, bool canScroll)
{
    if ((app.scrollDelay > delayThreshold) || ((hasIcon ? app.textOffset + 9 : app.textOffset) > 31))
    {
        if (canScroll && !app.noScrolling)
        {
            if (app.scrollSpeed == -1)
            {
                app.scrollposition -= movementFactor * ((float)appConfig.scrollSpeed / 100);
            }
            else
            {
                app.scrollposition -= movementFactor * (app.scrollSpeed / 100);
            }
        }
    }
    else
    {
        ++app.scrollDelay;
        if (hasIcon)
        {
            if (app.iconWasPushed && app.pushIcon == 1)
            {
                app.scrollposition = 0 + app.textOffset;
            }
            else
            {
                app.scrollposition = 9 + app.textOffset;
            }
        }
        else
        {
            app.scrollposition = 0 + app.textOffset;
        }
    }
}

int16_t calculateTextX(const AppContentBase& app,
                       uint16_t textWidth, bool hasIcon)
{
    if (app.center)
    {
        return hasIcon ? ((24 - textWidth) / 2) + 9 : ((32 - textWidth) / 2);
    }
    return hasIcon ? 9 : 0;
}

void renderAppText(AppContentBase& app, int16_t x, int16_t y,
                   uint16_t textWidth, uint16_t availableWidth,
                   const String& resolvedText,
                   const std::vector<String>& resolvedFragments,
                   bool hsvOnZeroColor)
{
    bool hasIcon = app.icon || !app.jpegDataBuffer.empty();
    bool isNoScrolling = (textWidth <= availableWidth);
    int16_t textX = calculateTextX(app, textWidth, hasIcon);

    if (isNoScrolling)
    {
        if (!resolvedFragments.empty())
        {
            int16_t fragmentX = textX + app.textOffset;
            for (size_t i = 0; i < resolvedFragments.size(); ++i)
            {
                uint32_t fragColor = (i < app.colors.size()) ? app.colors[i] : app.color;
                if (hsvOnZeroColor && fragColor == 0)
                {
                    DisplayManager.HSVtext(x + fragmentX, y + 6, resolvedFragments[i].c_str(), false, app.textCase);
                }
                else
                {
                    DisplayManager.setTextColor(TextEffect(fragColor, app.fade, app.blink));
                    DisplayManager.printText(x + fragmentX, y + 6, resolvedFragments[i].c_str(), false, app.textCase);
                }
                fragmentX += getTextWidth(resolvedFragments[i].c_str(), app.textCase);
            }
        }
        else
        {
            DisplayManager.renderColoredText(x + textX + app.textOffset, y + 6, resolvedText.c_str(),
                                             app.rainbow, app.gradient[0], app.gradient[1],
                                             app.color, app.fade, app.blink, app.textCase);
        }
    }
    else
    {
        if (!resolvedFragments.empty())
        {
            int16_t fragmentX = app.scrollposition;
            for (size_t i = 0; i < resolvedFragments.size(); ++i)
            {
                uint32_t fragColor = (i < app.colors.size()) ? app.colors[i] : app.color;
                if (hsvOnZeroColor && fragColor == 0)
                {
                    DisplayManager.HSVtext(x + fragmentX, y + 6, resolvedFragments[i].c_str(), false, app.textCase);
                }
                else
                {
                    DisplayManager.setTextColor(TextEffect(fragColor, app.fade, app.blink));
                    DisplayManager.printText(x + fragmentX, y + 6, resolvedFragments[i].c_str(), false, app.textCase);
                }
                fragmentX += getTextWidth(resolvedFragments[i].c_str(), app.textCase);
            }
        }
        else
        {
            DisplayManager.renderColoredText(x + app.scrollposition + app.textOffset, y + 6, resolvedText.c_str(),
                                             app.rainbow, app.gradient[0], app.gradient[1],
                                             app.color, app.fade, app.blink, app.textCase);
        }
    }
}

void renderAppOverlay(const AppContentBase& app,
                      FastLED_NeoMatrix *matrix, int16_t x, int16_t y)
{
    if (app.overlay != NONE)
    {
        NeoMatrixCanvas canvas(matrix);
        EffectOverlay(canvas, x, y, app.overlay);
    }
}
