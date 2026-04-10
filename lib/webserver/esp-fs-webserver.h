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
    void handleIndex(AsyncWebServerRequest *request);
    bool handleFileRead(AsyncWebServerRequest *request, const String &path);
    void handleFileUpload(AsyncWebServerRequest *request, const String &filename,
                          size_t index, uint8_t *data, size_t len, bool final);
    void replyToCLient(AsyncWebServerRequest *request, int msg_type, const char *msg);
    void checkForUnsupportedPath(String &filename, String &error);
    void handleScanNetworks(AsyncWebServerRequest *request);
    const char *getContentType(const char *filename);
    void captivePortal(AsyncWebServerRequest *request);
    void handleGetEdit(AsyncWebServerRequest *request);
    void handleFileCreate(AsyncWebServerRequest *request);
    void handleFileDelete(AsyncWebServerRequest *request);
    void handleStatus(AsyncWebServerRequest *request);
    void handleFileList(AsyncWebServerRequest *request);
};

#endif
