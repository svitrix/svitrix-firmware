#ifndef esp_fs_webserver_H
#define esp_fs_webserver_H

#include <Arduino.h>
#include <memory>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ESPmDNS.h>
#include <Update.h>

#define INCLUDE_EDIT_HTM
#ifdef INCLUDE_EDIT_HTM
// edit_htm.h removed — file manager now served from SPA in LittleFS /web/
#endif

#define INCLUDE_SETUP_HTM
#ifdef INCLUDE_SETUP_HTM
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
// setup_htm.h removed — setup page now served from SPA in LittleFS /web/
#endif

#ifndef DEBUG_ESP_PORT
#define DBG_OUTPUT_PORT Serial
#define DEBUG_MODE_WS false
#else
#define DBG_OUTPUT_PORT DEBUG_ESP_PORT
#endif

#if DEBUG_MODE_WS
#define DebugPrint(x) DBG_OUTPUT_PORT.print(x)
#define DebugPrintln(x) DBG_OUTPUT_PORT.println(x)
#define DebugPrintf(fmt, ...) DBG_OUTPUT_PORT.printf(fmt, ##__VA_ARGS__)
#define DebugPrintf_P(fmt, ...) DBG_OUTPUT_PORT.printf_P(fmt, ##__VA_ARGS__)
#else
#define DebugPrint(x)
#define DebugPrintln(x)
#define DebugPrintf(x, ...)
#define DebugPrintf_P(x, ...)
#endif

enum
{
    MSG_OK,
    CUSTOM,
    NOT_FOUND,
    BAD_REQUEST,
    ERROR
};
#define TEXT_PLAIN "text/plain"
#define FS_INIT_ERROR "FS INIT ERROR"
#define FILE_NOT_FOUND "FileNotFound"

// Helper to read accumulated POST body from request
static inline String getBody(AsyncWebServerRequest *request)
{
    if (request->_tempObject)
        return String((const char *)request->_tempObject);
    return String();
}

class FSWebServer
{

public:
    AsyncWebServer *webserver;

    FSWebServer(fs::FS &fs, AsyncWebServer &server);

    bool begin(int port = 80, const char *path = nullptr);

    void run();

    void addHandler(const char *uri, WebRequestMethodComposite method, ArRequestHandlerFunction fn);
    void addHandler(const char *uri, ArRequestHandlerFunction handler);
    void addHandlerWithBody(const char *uri, WebRequestMethodComposite method, ArRequestHandlerFunction fn);
    void onNotFound(ArRequestHandlerFunction fn);

    void setCaptiveWebage(const char *url);

    IPAddress setAPmode(const char *ssid, const char *psk);

    IPAddress startWiFi(uint32_t timeout, const char *apSSID, const char *apPsw);

    void setAuth(const String &user, const String &pass)
    {
        authUser = user;
        authPass = pass;
    }

#ifdef INCLUDE_SETUP_HTM

#define MIN_F -3.4028235E+38
#define MAX_F 3.4028235E+38

    inline bool clearOptions()
    {
        File file = m_filesystem->open("/DoNotTouch.json", "r");
        if (file)
        {
            file.close();
            m_filesystem->remove("/DoNotTouch.json");
            return true;
        }
        return false;
    }

    inline void addOptionBox(const char *boxTitle)
    {
        addOption("param-box", boxTitle, false);
    }

    inline void addHTML(const char *html, const char *id)
    {
        String elementId = "raw-html-";
        elementId += id;
        String trimmed = html;
        removeWhiteSpaces(trimmed);
        addOption(elementId.c_str(), trimmed.c_str(), false);
    }

    inline void addCSS(const char *css)
    {
        String trimmed = css;
        removeWhiteSpaces(trimmed);
        addOption("raw-css", trimmed.c_str(), false);
    }

    inline void addJavascript(const char *script)
    {
        addOption("raw-javascript", script, true, MIN_F, MAX_F, 1.0, true);
    }

    void addDropdownList(const char *label, const char **array, size_t size);

    // Only for backward-compatibility
    template <typename T>
    inline void addOption(fs::FS &fs, const char *label, T val, bool hidden = false)
    {
        addOption(label, val, hidden);
    }

    // Add custom option to config webpage (float values)
    template <typename T>
    inline void addOption(const char *label, T val, double d_min, double d_max, double step)
    {
        addOption(label, val, false, d_min, d_max, step);
    }

    // Add custom option to config webpage (type of parameter will be deduced from variable itself)
    template <typename T>
    inline void addOption(const char *label, T val, bool hidden = false,
                          double d_min = MIN_F, double d_max = MAX_F, double step = 1.0, bool replace = false)
    {
        File file = m_filesystem->open("/DoNotTouch.json", "r");
        int sz = file.size() * 1.33;
        int docSize = max(sz, 2048);
        DynamicJsonDocument doc((size_t)docSize);
        if (file)
        {
            DeserializationError error = deserializeJson(doc, file);
            if (error)
            {
                DebugPrintln(F("Failed to deserialize file, may be corrupted"));
                DebugPrintln(error.c_str());
                file.close();
                return;
            }
            file.close();
        }
        else
        {
            DebugPrintln(F("File not found, will be created new configuration file"));
        }

        numOptions++;

        String key = label;
        if (hidden)
            key += "-hidden";

        if (key.equals("param-box"))
        {
            key = key + "-" + val;
        }
        if (key.equals("raw-javascript"))
        {
            key += numOptions;
        }

        if (doc.containsKey(key.c_str()) && !replace)
            return;

        if (d_min != MIN_F || d_max != MAX_F || step != 1.0)
        {
            JsonObject obj = doc.createNestedObject(key);
            obj["value"] = static_cast<T>(val);
            obj["min"] = d_min;
            obj["max"] = d_max;
            obj["step"] = step;
        }
        else
        {
            doc[key] = static_cast<T>(val);
        }

        file = m_filesystem->open("/DoNotTouch.json", "w");
        if (serializeJsonPretty(doc, file) == 0)
        {
            DebugPrintln(F("Failed to write to file"));
        }
        file.close();
    }

    template <typename T>
    bool getOptionValue(const char *label, T &var)
    {
        File file = m_filesystem->open("/DoNotTouch.json", "r");
        DynamicJsonDocument doc(file.size() * 1.33);
        if (file)
        {
            DeserializationError error = deserializeJson(doc, file);
            if (error)
            {
                DebugPrintln(F("Failed to deserialize file, may be corrupted"));
                DebugPrintln(error.c_str());
                file.close();
                return false;
            }
            file.close();
        }
        else
            return false;

        if (doc[label]["value"])
            var = doc[label]["value"].as<T>();
        else if (doc[label]["selected"])
            var = doc[label]["selected"].as<T>();
        else
            var = doc[label].as<T>();
        return true;
    }

    template <typename T>
    bool saveOptionValue(const char *label, T val)
    {
        File file = m_filesystem->open("/DoNotTouch.json", "r");
        DynamicJsonDocument doc(file.size() * 1.33);

        if (file)
        {
            DeserializationError error = deserializeJson(doc, file);
            if (error)
            {
                DebugPrintln(F("Failed to deserialize file, may be corrupted"));
                DebugPrintln(error.c_str());
                file.close();
                return false;
            }
            file.close();
        }

        if (!doc.containsKey(label))
        {
            doc[label] = val;
        }
        else
        {
            JsonVariant option = doc[label];
            option.set(val);
        }

        file = m_filesystem->open("/DoNotTouch.json", "w");
        if (!file)
            return false;
        serializeJsonPretty(doc, file);
        file.close();

        return true;
    }

#endif

private:
    int failedAttempts = 0;
    unsigned long previousMillis = 0;
    unsigned long interval = 10000;
    char m_basePath[16];
    String authUser;
    String authPass;
    DNSServer m_dnsServer;
    fs::FS *m_filesystem;
    File m_uploadFile;
    bool m_fsOK = false;
    bool m_apmode = false;
    char *m_apWebpage = (char *)"/settings";
    uint32_t m_timeout = 10000;

    bool checkDir(char *dirname, uint8_t levels);
    void doWifiConnection(AsyncWebServerRequest *request);
    void doRestart(AsyncWebServerRequest *request);
    void replyOK(AsyncWebServerRequest *request);
    void getIpAddress(AsyncWebServerRequest *request);
    void handleRequest(AsyncWebServerRequest *request);
    bool checkAuth(AsyncWebServerRequest *request);
    void setupOTA();
#ifdef INCLUDE_SETUP_HTM
    void removeWhiteSpaces(String &str);
    void handleSetup(AsyncWebServerRequest *request);
    uint8_t numOptions = 0;
#endif
    void handleIndex(AsyncWebServerRequest *request);
    bool handleFileRead(AsyncWebServerRequest *request, const String &path);
    void handleFileUpload(AsyncWebServerRequest *request, const String &filename,
                          size_t index, uint8_t *data, size_t len, bool final);
    void replyToCLient(AsyncWebServerRequest *request, int msg_type, const char *msg);
    void checkForUnsupportedPath(String &filename, String &error);
    void handleScanNetworks(AsyncWebServerRequest *request);
    const char *getContentType(const char *filename);
    void captivePortal(AsyncWebServerRequest *request);
#ifdef INCLUDE_EDIT_HTM
    void handleGetEdit(AsyncWebServerRequest *request);
    void handleFileCreate(AsyncWebServerRequest *request);
    void handleFileDelete(AsyncWebServerRequest *request);
    void handleStatus(AsyncWebServerRequest *request);
    void handleFileList(AsyncWebServerRequest *request);
#endif
};

#endif
