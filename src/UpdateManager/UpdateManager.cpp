#include <UpdateManager.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "cert.h"
#include "IDisplayRenderer.h"
#include <Ticker.h>
#include "Globals.h"
#include <cassert>

static IDisplayRenderer *display_ = nullptr;

void UpdateManager_::setDisplay(IDisplayRenderer *d)
{
    assert(d);
    display_ = d;
}
bool UpdateManager_::hasDisplay() const
{
    return display_ != nullptr;
}


// The getter for the instantiated singleton instance
UpdateManager_& UpdateManager_::getInstance()
{
    static UpdateManager_ instance;
    return instance;
}

// Initialize the global shared instance
UpdateManager_& UpdateManager = UpdateManager.getInstance();

void update_started()
{
}

void update_finished()
{
}

void update_progress(int cur, int total)
{
    display_->clear();
    int progress = (cur * 100) / total;
    char progressStr[5];
    snprintf(progressStr, 5, "%d%%", progress);
    display_->resetTextColor();
    display_->printText(0, 6, progressStr, true, false);
    display_->drawProgressBar(0, 7, progress, 0x00FF00, 0xFFFFFF);
    display_->show();
}

void update_error(int err)
{
    display_->clear();
    display_->printText(0, 6, "FAIL", true, true);
    display_->show();
}

void UpdateManager_::updateFirmware()
{
    if (systemConfig.updateFirmwareUrl.isEmpty())
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("OTA firmware URL not configured"));
        return;
    }

    if (!WiFi.isConnected())
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("OTA update skipped: no WiFi"));
        return;
    }

    WiFiClientSecure client;
    client.setCACert(rootCACertificate);
    client.setTimeout(5);

    httpUpdate.onStart(update_started);
    httpUpdate.onEnd(update_finished);
    httpUpdate.onProgress(update_progress);
    httpUpdate.onError(update_error);

    t_httpUpdate_return ret = httpUpdate.update(client, systemConfig.updateFirmwareUrl.c_str());
    switch (ret)
    {
    case HTTP_UPDATE_FAILED:
        if (systemConfig.debugMode)
            DEBUG_PRINTF("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

    case HTTP_UPDATE_NO_UPDATES:
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("HTTP_UPDATE_NO_UPDATES"));
        break;

    case HTTP_UPDATE_OK:
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("HTTP_UPDATE_OK"));
        break;
    }
}

bool UpdateManager_::checkUpdate(bool withScreen)
{
    if (systemConfig.updateVersionUrl.isEmpty())
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("OTA version URL not configured"));
        systemConfig.updateAvailable = false;
        return false;
    }

    if (withScreen)
    {
        display_->clear();
        display_->resetTextColor();
        display_->printText(0, 6, "CHECK", true, true);
        display_->show();
    }

    if (!WiFi.isConnected())
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Update check skipped: no WiFi"));
        systemConfig.updateAvailable = false;
        return false;
    }

    String payload;
    int httpCode = -1;
    String fwurl = systemConfig.updateVersionUrl + "?" + String(rand());
    if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Check firmwareversion"));

    static WiFiClientSecure client;

    client.setCACert(rootCACertificate);
    client.setTimeout(5);
    HTTPClient https;
    https.setConnectTimeout(3000);
    https.setTimeout(5000);

    if (https.begin(client, fwurl))
    {
        httpCode = https.GET();
        if (httpCode == HTTP_CODE_OK) // if version received
        {
            payload = https.getString(); // save received version
        }
        else
        {
            if (systemConfig.debugMode)
                DEBUG_PRINTLN(F("Error in downloading version file"));
            if (withScreen)
            {
                display_->clear();
                display_->resetTextColor();
                display_->printText(0, 6, "ERR CNCT", true, true);
                display_->show();
                delay(1000);
            }
        }
        https.end();
    }

    if (httpCode == HTTP_CODE_OK) // if version received
    {
        payload.trim();
        if (payload.equals(VERSION))
        {
            systemConfig.updateAvailable = false;
            if (systemConfig.debugMode)
                DEBUG_PRINTF("\nDevice already on latest firmware version: %s\n", VERSION);
            if (withScreen)
            {
                display_->clear();
                display_->resetTextColor();
                display_->printText(0, 6, "NO UP :(", true, true);
                display_->show();
                delay(1000);
            }
            return 0;
        }
        else
        {
            if (systemConfig.debugMode)
                DEBUG_PRINTLN(F("New firmwareversion found!"));
            systemConfig.updateAvailable = true;
            return 1;
        }
    }
    systemConfig.updateAvailable = false;
    return 0;
}

void checkUpdateNoReturn()
{
    Serial.println("Check Update");
    UpdateManager.checkUpdate(false);
}

void UpdateManager_::setup()
{
}
