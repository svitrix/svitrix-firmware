#include "ServerManager.h"
#include "Globals.h"
#include <esp-fs-webserver.h>
#include <ArduinoJson.h>
#include <Update.h>
#include <ESPmDNS.h>
#include <LittleFS.h>
#include <WiFi.h>
#include "IDisplayRenderer.h"
#include "IDisplayControl.h"
#include "IDisplayNavigation.h"
#include "IDisplayNotifier.h"
#include <cassert>
#include "ISound.h"
#include "IPower.h"
#include "IUpdater.h"
#include "DataFetcher/DataFetcher.h"
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <EEPROM.h>

WiFiUDP udp;

unsigned int localUdpPort = 4210;
char incomingPacket[255];

// Pufferdefinition
constexpr int BUFFER_SIZE = 64;
char dataBuffer[BUFFER_SIZE];
int bufferIndex = 0;

// Aktueller verbundener Client
WiFiClient currentClient = WiFiClient();
AsyncWebServer server(80);
FSWebServer mws(LittleFS, server);

// Erstelle eine Server-Instanz
WiFiServer TCPserver(8080);

// The getter for the instantiated singleton instance
ServerManager_& ServerManager_::getInstance()
{
    static ServerManager_ instance;
    return instance;
}

// Initialize the global shared instance
ServerManager_& ServerManager = ServerManager.getInstance();

static IDisplayRenderer *smRenderer_ = nullptr;
static IDisplayControl *smControl_ = nullptr;
static IDisplayNavigation *smNav_ = nullptr;
static IDisplayNotifier *smNotify_ = nullptr;
static ISound *smSound_ = nullptr;
static IPower *smPower_ = nullptr;
static IUpdater *smUpdater_ = nullptr;

void ServerManager_::setDisplay(IDisplayRenderer *r, IDisplayControl *c, IDisplayNavigation *n, IDisplayNotifier *nt)
{
    assert(r && c && n && nt);
    smRenderer_ = r;
    smControl_ = c;
    smNav_ = n;
    smNotify_ = nt;
}

bool ServerManager_::hasDisplay() const
{
    return smRenderer_ && smControl_ && smNav_ && smNotify_;
}

void ServerManager_::setServices(ISound *s, IPower *p, IUpdater *u)
{
    assert(s && p && u);
    smSound_ = s;
    smPower_ = p;
    smUpdater_ = u;
}

bool ServerManager_::hasServices() const
{
    return smSound_ && smPower_ && smUpdater_;
}

void versionHandler(AsyncWebServerRequest *request)
{
    request->send(200, "text/plain", VERSION);
}

void ServerManager_::erase()
{
    smRenderer_->HSVtext(0, 6, "RESET", true, 0);
    wifi_config_t conf;
    memset(&conf, 0, sizeof(conf)); // Set all the bytes in the structure to 0
    esp_wifi_set_config(WIFI_IF_STA, &conf);
    LittleFS.format();
    delay(200);
    formatSettings();
    delay(200);
}

void saveHandler(AsyncWebServerRequest *request)
{
    ServerManager.loadSettings();
    request->send(200);
}

void addHandler()
{
    mws.addHandlerWithBody("/api/power", HTTP_POST, [](AsyncWebServerRequest *request)
                           { String body = getBody(request); smControl_->powerStateParse(body.c_str()); request->send(200, "text/plain", "OK"); });
    mws.addHandlerWithBody(
        "/api/sleep", HTTP_POST, [](AsyncWebServerRequest *request)
        {
            String body = getBody(request);
            request->send(200, "text/plain", "OK");
            smControl_->setPower(false);
            smPower_->sleepParser(body.c_str()); });
    mws.addHandler("/api/loop", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(200, "application/json", smNav_->getAppsAsJson()); });
    mws.addHandler("/api/effects", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(200, "application/json", smNav_->getEffectNames()); });
    mws.addHandler("/api/transitions", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(200, "application/json", smNav_->getTransitionNames()); });
    mws.addHandler("/api/reboot", HTTP_ANY, [](AsyncWebServerRequest *request)
                   { request->send(200, "text/plain", "OK"); delay(200); ESP.restart(); });
    mws.addHandlerWithBody("/api/rtttl", HTTP_POST, [](AsyncWebServerRequest *request)
                           { String body = getBody(request); request->send(200, "text/plain", "OK"); smSound_->playRTTTLString(body.c_str()); });
    mws.addHandlerWithBody("/api/sound", HTTP_POST, [](AsyncWebServerRequest *request)
                           { String body = getBody(request);
                           if (smSound_->parseSound(body.c_str())){
                            request->send(200, "text/plain", "OK");
                           }else{
                            request->send(404, "text/plain", "FileNotFound");
                           }; });

    mws.addHandlerWithBody("/api/moodlight", HTTP_POST, [](AsyncWebServerRequest *request)
                           {
                            String body = getBody(request);
                            if (smControl_->moodlight(body.c_str()))
                            {
                                request->send(200, "text/plain", "OK");
                            }
                            else
                            {
                                request->send(500, "text/plain", "ErrorParsingJson");
                            } });
    mws.addHandlerWithBody("/api/notify", HTTP_POST, [](AsyncWebServerRequest *request)
                           {
                               String body = getBody(request);
                               if (smNotify_->generateNotification(1, body.c_str()))
                               {
                                request->send(200, "text/plain", "OK");
                               }else{
                                request->send(500, "text/plain", "ErrorParsingJson");
                               } });
    mws.addHandler("/api/nextapp", HTTP_ANY, [](AsyncWebServerRequest *request)
                   { smNav_->nextApp(); request->send(200, "text/plain", "OK"); });
    // HTML pages now served from SPA in LittleFS /web/
    mws.addHandler("/api/previousapp", HTTP_POST, [](AsyncWebServerRequest *request)
                   { smNav_->previousApp(); request->send(200, "text/plain", "OK"); });
    mws.addHandler("/api/notify/dismiss", HTTP_ANY, [](AsyncWebServerRequest *request)
                   { smNotify_->dismissNotify(); request->send(200, "text/plain", "OK"); });
    mws.addHandlerWithBody("/api/apps", HTTP_POST, [](AsyncWebServerRequest *request)
                           { String body = getBody(request); smNav_->updateAppVector(body.c_str()); request->send(200, "text/plain", "OK"); });
    mws.addHandlerWithBody(
        "/api/switch", HTTP_POST, [](AsyncWebServerRequest *request)
        {
        String body = getBody(request);
        if (smNav_->switchToApp(body.c_str()))
        {
            request->send(200, "text/plain", "OK");
        }
        else
        {
            request->send(500, "text/plain", "FAILED");
        } });
    mws.addHandler("/api/apps", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(200, "application/json", smNav_->getAppsWithIcon()); });
    mws.addHandlerWithBody("/api/settings", HTTP_POST, [](AsyncWebServerRequest *request)
                           { String body = getBody(request); smControl_->setNewSettings(body.c_str()); request->send(200, "text/plain", "OK"); });
    mws.addHandler("/api/erase", HTTP_ANY, [](AsyncWebServerRequest *request)
                   { ServerManager.erase(); request->send(200, "text/plain", "OK"); delay(200); ESP.restart(); });
    mws.addHandler("/api/resetSettings", HTTP_ANY, [](AsyncWebServerRequest *request)
                   { formatSettings(); request->send(200, "text/plain", "OK"); delay(200); ESP.restart(); });
    mws.addHandlerWithBody("/api/reorder", HTTP_POST, [](AsyncWebServerRequest *request)
                           { String body = getBody(request); smNav_->reorderApps(body.c_str()); request->send(200, "text/plain", "OK"); });
    mws.addHandler("/api/settings", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(200, "application/json", smControl_->getSettings()); });
    mws.addHandlerWithBody("/api/custom", HTTP_POST, [](AsyncWebServerRequest *request)
                           {
                            String name;
                            if (request->hasParam("name"))
                                name = request->getParam("name")->value();
                            String body = getBody(request);
                            if (smNav_->parseCustomPage(name, body.c_str(), false)){
                                request->send(200, "text/plain", "OK");
                            }else{
                                request->send(500, "text/plain", "ErrorParsingJson");
                            } });
    // NOTE: more specific routes must be registered BEFORE the parent route,
    // because ESPAsyncWebServer's canHandle() also matches by prefix ("/api/datafetcher/...")
    mws.addHandler("/api/datafetcher/fetch", HTTP_POST, [](AsyncWebServerRequest *request)
                   {
                    if (request->hasParam("name")) {
                        DataFetcher.forceFetch(request->getParam("name")->value());
                        request->send(200, "text/plain", "OK");
                    } else {
                        request->send(400, "text/plain", "MissingName");
                    } });
    mws.addHandler("/api/datafetcher", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(200, "application/json", DataFetcher.getSourcesAsJson()); });
    mws.addHandlerWithBody("/api/datafetcher", HTTP_POST, [](AsyncWebServerRequest *request)
                           {
                            String body = getBody(request);
                            if (DataFetcher.addSource(body.c_str()))
                                request->send(200, "text/plain", "OK");
                            else
                                request->send(400, "text/plain", "Error"); });
    mws.addHandler("/api/datafetcher", HTTP_DELETE, [](AsyncWebServerRequest *request)
                   {
                    if (request->hasParam("name")) {
                        String name = request->getParam("name")->value();
                        if (DataFetcher.removeSource(name))
                            request->send(200, "text/plain", "OK");
                        else
                            request->send(404, "text/plain", "NotFound");
                    } else {
                        request->send(400, "text/plain", "MissingName");
                    } });
    mws.addHandler("/api/stats", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(200, "application/json", smControl_->getStats()); });
    mws.addHandler("/api/screen", HTTP_GET, [](AsyncWebServerRequest *request)
                   { request->send(200, "application/json", smControl_->ledsAsJson()); });
    mws.addHandlerWithBody("/api/indicator1", HTTP_POST, [](AsyncWebServerRequest *request)
                           {
                            String body = getBody(request);
                            if (smNotify_->indicatorParser(1, body.c_str())){
                             request->send(200, "text/plain", "OK");
                            }else{
                                 request->send(500, "text/plain", "ErrorParsingJson");
                            } });
    mws.addHandlerWithBody("/api/indicator2", HTTP_POST, [](AsyncWebServerRequest *request)
                           {
                            String body = getBody(request);
                            if (smNotify_->indicatorParser(2, body.c_str())){
                             request->send(200, "text/plain", "OK");
                            }else{
                                 request->send(500, "text/plain", "ErrorParsingJson");
                            } });
    mws.addHandlerWithBody("/api/indicator3", HTTP_POST, [](AsyncWebServerRequest *request)
                           {
                            String body = getBody(request);
                            if (smNotify_->indicatorParser(3, body.c_str())){
                             request->send(200, "text/plain", "OK");
                            }else{
                                 request->send(500, "text/plain", "ErrorParsingJson");
                            } });
    mws.addHandlerWithBody("/api/doupdate", HTTP_POST, [](AsyncWebServerRequest *request)
                           {
                            String body = getBody(request);
                            if (smUpdater_->checkUpdate(true)){
                                request->send(200, "text/plain", "OK");
                                smUpdater_->updateFirmware();
                            }else{
                                request->send(404, "text/plain", "NoUpdateFound");
                            } });
    mws.addHandlerWithBody("/api/r2d2", HTTP_POST, [](AsyncWebServerRequest *request)
                           { String body = getBody(request); smSound_->r2d2(body.c_str()); request->send(200, "text/plain", "OK"); });
}

void ServerManager_::setup()
{
    esp_wifi_set_max_tx_power(80); // 82 * 0.25 dBm = 20.5 dBm
    esp_wifi_set_ps(WIFI_PS_NONE); // Power Saving deaktivieren
    IPAddress ip, gw, sn, dns1, dns2;
    if (!ip.fromString(networkConfig.ip) || !gw.fromString(networkConfig.gateway) ||
        !sn.fromString(networkConfig.subnet) || !dns1.fromString(networkConfig.primaryDns) ||
        !dns2.fromString(networkConfig.secondaryDns))
        networkConfig.isStatic = false;
    if (networkConfig.isStatic)
    {
        WiFi.config(ip, gw, sn, dns1, dns2);
    }
    WiFi.setHostname(systemConfig.hostname.c_str()); // define hostname
    myIP = mws.startWiFi(systemConfig.apTimeout * 1000, systemConfig.hostname.c_str(), "12345678");
    isConnected = !(myIP == IPAddress(192, 168, 4, 1));
    if (systemConfig.debugMode)
        DEBUG_PRINTF("My IP: %d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);
    mws.setAuth(authConfig.user, authConfig.pass);
    if (isConnected)
    {
        initConfigDefaults();
        mws.addHandlerWithBody("/save", HTTP_POST, saveHandler);
        addHandler();
        udp.begin(localUdpPort);
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Webserver loaded"));
    }
    mws.addHandler("/version", HTTP_GET, versionHandler);
    mws.begin(systemConfig.webPort);

    if (!MDNS.begin(systemConfig.hostname))
    {
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Error starting mDNS"));
    }
    else
    {
        MDNS.addService("http", "tcp", 80);
        MDNS.addService("svitrix", "tcp", 80);
        MDNS.addServiceTxt("svitrix", "tcp", "id", systemConfig.deviceId);
        MDNS.addServiceTxt("svitrix", "tcp", "name", systemConfig.hostname.c_str());
        MDNS.addServiceTxt("svitrix", "tcp", "type", "svitrix");
    }

    configTzTime(timeConfig.ntpTz.c_str(), timeConfig.ntpServer.c_str());
    tm timeInfo;
    getLocalTime(&timeInfo);
    TCPserver.begin();
    TCPserver.setNoDelay(true);
}

void ServerManager_::tick()
{
    mws.run();

    if (!systemConfig.apMode)
    {
        int packetSize = udp.parsePacket();
        if (packetSize)
        {
            int len = udp.read(incomingPacket, 255);
            if (len > 0)
            {
                incomingPacket[len] = 0;
            }
            if (strcmp(incomingPacket, "FIND_SVITRIX") == 0)
            {
                udp.beginPacket(udp.remoteIP(), 4211);
                if (systemConfig.webPort != 80)
                {
                    char buffer[128];
                    snprintf(buffer, sizeof(buffer), "%s:%d",
                             systemConfig.hostname.c_str(), systemConfig.webPort);
                    udp.printf("%s", buffer);
                }
                else
                {
                    udp.printf("%s", systemConfig.hostname.c_str());
                }

                udp.endPacket();
            }
        }
    }

    if (!currentClient || !currentClient.connected())
    {
        if (TCPserver.hasClient())
        {
            if (currentClient)
            {
                currentClient.stop();
                Serial.println("Vorheriger Client getrennt, um neuen Client zu akzeptieren.");
            }
            currentClient = TCPserver.available();
            Serial.println("Neuer Client verbunden.");
        }
    }

    if (currentClient && currentClient.connected())
    {
        while (currentClient.available())
        {
            char incomingByte = currentClient.read();
            if (incomingByte == '\n')
            {
                dataBuffer[bufferIndex] = '\0';
                bufferIndex = 0;
            }
            else if (incomingByte != '\r')
            {
                if (bufferIndex < BUFFER_SIZE - 1)
                {
                    dataBuffer[bufferIndex++] = incomingByte;
                }
                else
                {
                    bufferIndex = 0;
                }
            }
        }
    }
}

void ServerManager_::sendTCP(String message)
{
    if (currentClient && currentClient.connected())
    {
        currentClient.print(message);
    }
}

void ServerManager_::initConfigDefaults()
{
    if (LittleFS.exists("/DoNotTouch.json"))
        return; // Config already exists, don't overwrite user settings

    DynamicJsonDocument doc(2048);
    doc["Static IP"] = networkConfig.isStatic;
    doc["Local IP"] = networkConfig.ip;
    doc["Gateway"] = networkConfig.gateway;
    doc["Subnet"] = networkConfig.subnet;
    doc["Primary DNS"] = networkConfig.primaryDns;
    doc["Secondary DNS"] = networkConfig.secondaryDns;
    doc["Broker"] = mqttConfig.host;
    doc["Port"] = mqttConfig.port;
    doc["Username"] = mqttConfig.user;
    doc["Password"] = mqttConfig.pass;
    doc["Prefix"] = mqttConfig.prefix;
    doc["Homeassistant Discovery"] = haConfig.discovery;
    doc["NTP Server"] = timeConfig.ntpServer;
    doc["Timezone"] = timeConfig.ntpTz;
    doc["Auth Username"] = authConfig.user;
    doc["Auth Password"] = authConfig.pass;

    File file = LittleFS.open("/DoNotTouch.json", "w");
    serializeJsonPretty(doc, file);
    file.close();
}

void ServerManager_::loadSettings()
{
    if (LittleFS.exists("/DoNotTouch.json"))
    {
        File file = LittleFS.open("/DoNotTouch.json", "r");
        DynamicJsonDocument doc(file.size() * 1.33);
        DeserializationError error = deserializeJson(doc, file);
        if (error)
        {
            file.close();
            return;
        }

        timeConfig.ntpServer = doc["NTP Server"].as<String>();
        timeConfig.ntpTz = doc["Timezone"].as<String>();
        mqttConfig.host = doc["Broker"].as<String>();
        mqttConfig.port = doc["Port"].as<uint16_t>();
        mqttConfig.user = doc["Username"].as<String>();
        mqttConfig.pass = doc["Password"].as<String>();
        mqttConfig.prefix = doc["Prefix"].as<String>();
        mqttConfig.prefix.trim();
        networkConfig.isStatic = doc["Static IP"];
        haConfig.discovery = doc["Homeassistant Discovery"];
        networkConfig.ip = doc["Local IP"].as<String>();
        networkConfig.gateway = doc["Gateway"].as<String>();
        networkConfig.subnet = doc["Subnet"].as<String>();
        networkConfig.primaryDns = doc["Primary DNS"].as<String>();
        networkConfig.secondaryDns = doc["Secondary DNS"].as<String>();
        if (doc["Auth Username"].is<String>())
            authConfig.user = doc["Auth Username"].as<String>();
        if (doc["Auth Password"].is<String>())
            authConfig.pass = doc["Auth Password"].as<String>();

        file.close();
        smControl_->applyAllSettings();
        if (systemConfig.debugMode)
            DEBUG_PRINTLN(F("Webserver configuration loaded"));
        doc.clear();
        return;
    }
    else if (systemConfig.debugMode)
        DEBUG_PRINTLN(F("Webserver configuration file not exist"));
    return;
}

void ServerManager_::sendButton(byte btn, bool state)
{
    if (systemConfig.buttonCallback == "")
        return;
    static bool btn0State, btn1State, btn2State;
    String payload;
    switch (btn)
    {
    case 0:
        if (btn0State != state)
        {
            btn0State = state;
            payload = "button=left&state=" + String(state) + "&uid=" + systemConfig.deviceId;
        }
        break;
    case 1:
        if (btn1State != state)
        {
            btn1State = state;
            payload = "button=middle&state=" + String(state) + "&uid=" + systemConfig.deviceId;
        }
        break;
    case 2:
        if (btn2State != state)
        {
            btn2State = state;
            payload = "button=right&state=" + String(state) + "&uid=" + systemConfig.deviceId;
        }
        break;
    default:
        return;
    }
    if (!payload.isEmpty())
    {
        HTTPClient http;
        http.setConnectTimeout(200);
        http.setTimeout(300);
        http.begin(systemConfig.buttonCallback);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        http.POST(payload);
        http.end();
    }
}
