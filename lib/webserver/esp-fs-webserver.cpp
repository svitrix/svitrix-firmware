#include "esp-fs-webserver.h"

// OTA update fallback page (must work without SPA in LittleFS)
static const char update_html[] PROGMEM = R"EOF(
<!DOCTYPE html><html><head><meta charset='utf-8'>
<meta name='viewport' content='width=device-width,initial-scale=1'/>
<title>SVITRIX - Update</title>
<style>body{font-family:system-ui,-apple-system,sans-serif;background:#070b1e;color:#e8e8f0;display:flex;justify-content:center;align-items:center;min-height:100vh;margin:0}.c{background:#0a0e27;border:1px solid #1e2550;border-radius:12px;padding:40px;text-align:center;max-width:480px;width:90%}h2{color:#f0b800;margin-bottom:24px}form{margin:16px 0}input[type=file]{color:#e8e8f0;margin:8px 0}input[type=submit]{background:#f0b800;color:#070b1e;border:none;border-radius:8px;padding:12px 24px;font-size:16px;font-weight:600;cursor:pointer;margin-top:8px}input[type=submit]:hover{background:#e8a800}hr{border-color:#1e2550;margin:24px 0}</style>
</head><body><div class='c'><h2>Firmware Update</h2>
<form method='POST' action='/update' enctype='multipart/form-data'>
<input type='file' accept='.bin,.bin.gz' name='update'><br>
<input type='submit' value='Upload Firmware'>
</form></div></body></html>
)EOF";

FSWebServer::FSWebServer(fs::FS &fs, AsyncWebServer &server)
{
    m_filesystem = &fs;
    webserver = &server;
    m_basePath[0] = '\0';
}

bool FSWebServer::checkAuth(AsyncWebServerRequest *request)
{
    if (authUser.isEmpty() || m_apmode)
        return true;
    if (!request->authenticate(authUser.c_str(), authPass.c_str()))
    {
        request->requestAuthentication();
        return false;
    }
    return true;
}

void FSWebServer::run()
{
    if (m_apmode)
        m_dnsServer.processNextRequest();

    unsigned long currentMillis = millis();
    if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval) && !m_apmode)
    {
        Serial.println("Reconnecting to WiFi...");
        WiFi.disconnect();
        WiFi.reconnect();
        previousMillis = currentMillis;
    }
}

void FSWebServer::addHandler(const char *uri, WebRequestMethodComposite method, ArRequestHandlerFunction fn)
{
    webserver->on(uri, method, [this, fn](AsyncWebServerRequest *request)
                  {
        if (!checkAuth(request)) return;
        fn(request); });
}

void FSWebServer::onNotFound(ArRequestHandlerFunction fn)
{
    webserver->onNotFound(fn);
}

void FSWebServer::addHandler(const char *uri, ArRequestHandlerFunction handler)
{
    addHandler(uri, HTTP_ANY, handler);
}

void FSWebServer::addHandlerWithBody(const char *uri, WebRequestMethodComposite method, ArRequestHandlerFunction fn)
{
    webserver->on(uri, method,
                  // onRequest: called after body is fully received
                  [this, fn](AsyncWebServerRequest *request)
                  {
                      if (!checkAuth(request))
                          return;
                      fn(request);
                      if (request->_tempObject)
                      {
                          free(request->_tempObject);
                          request->_tempObject = nullptr;
                      }
                  },
                  // onUpload: not used
                  nullptr,
                  // onBody: accumulate body chunks
                  [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
                  {
                      if (index == 0)
                      {
                          request->_tempObject = malloc(total + 1);
                          if (!request->_tempObject)
                              return;
                      }
                      if (request->_tempObject)
                      {
                          memcpy((uint8_t *)request->_tempObject + index, data, len);
                          if (index + len == total)
                          {
                              ((char *)request->_tempObject)[total] = '\0';
                          }
                      }
                  });
}

// List all files saved in the selected filesystem
bool FSWebServer::checkDir(char *dirname, uint8_t levels)
{
    if (dirname[0] != '/')
        dirname[0] = '/';
    File root = m_filesystem->open(dirname, "r");
    if (!root)
    {
        DebugPrintln("- failed to open directory\n");
        return false;
    }
    if (!root.isDirectory())
    {
        DebugPrintln(" - not a directory\n");
        return false;
    }
    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            char dir[16];
            strcpy(dir, "/");
            strcat(dir, file.name());
            DebugPrintf("DIR : %s\n", dir);
            checkDir(dir, levels - 1);
        }
        else
        {
            DebugPrintf("  FILE: %s\tSIZE: %d\n", file.name(), file.size());
        }
        file = root.openNextFile();
    }
    return true;
}

bool FSWebServer::begin(int port, const char *path)
{
    DebugPrintln("\nList the files of webserver: ");
    if (path != nullptr)
        strcpy(m_basePath, path);

    m_fsOK = checkDir(m_basePath, 2);

    addHandler("/status", HTTP_GET, [this](AsyncWebServerRequest *r)
               { handleStatus(r); });
    addHandler("/list", HTTP_GET, [this](AsyncWebServerRequest *r)
               { handleFileList(r); });
    addHandler("/edit", HTTP_GET, [this](AsyncWebServerRequest *r)
               { handleGetEdit(r); });
    addHandler("/edit", HTTP_PUT, [this](AsyncWebServerRequest *r)
               { handleFileCreate(r); });
    addHandler("/edit", HTTP_DELETE, [this](AsyncWebServerRequest *r)
               { handleFileDelete(r); });

    // File upload: POST /edit with multipart upload
    webserver->on(
        "/edit", HTTP_POST,
        // onRequest: called after upload completes
        [this](AsyncWebServerRequest *request)
        {
            if (!checkAuth(request))
                return;
            replyOK(request);
        },
        // onUpload: called for each chunk
        [this](AsyncWebServerRequest *request, const String &filename,
               size_t index, uint8_t *data, size_t len, bool final)
        {
            if (!checkAuth(request))
                return;
            handleFileUpload(request, filename, index, data, len, final);
        });

    webserver->onNotFound([this](AsyncWebServerRequest *request)
                          {
        if (!checkAuth(request)) return;
        handleRequest(request); });

    addHandler("/favicon.ico", HTTP_GET, [this](AsyncWebServerRequest *r)
               { replyOK(r); });
    addHandler("/", HTTP_GET, [this](AsyncWebServerRequest *r)
               { handleIndex(r); });
    addHandler("/setup", HTTP_GET, [this](AsyncWebServerRequest *r)
               { handleIndex(r); }); // /setup redirects to SPA
    addHandler("/scan", HTTP_GET, [this](AsyncWebServerRequest *r)
               { handleScanNetworks(r); });
    addHandlerWithBody("/connect", HTTP_POST, [this](AsyncWebServerRequest *r)
                       { doWifiConnection(r); });
    addHandler("/restart", HTTP_GET, [this](AsyncWebServerRequest *r)
               { doRestart(r); });
    addHandler("/ipaddress", HTTP_GET, [this](AsyncWebServerRequest *r)
               { getIpAddress(r); });

    // Captive Portal redirect
    webserver->on("/redirect", HTTP_GET, [this](AsyncWebServerRequest *r)
                  { captivePortal(r); });
    webserver->on("/connecttest.txt", HTTP_GET, [this](AsyncWebServerRequest *r)
                  { captivePortal(r); });
    webserver->on("/hotspot-detect.html", HTTP_GET, [this](AsyncWebServerRequest *r)
                  { captivePortal(r); });
    webserver->on("/generate_204", HTTP_GET, [this](AsyncWebServerRequest *r)
                  { captivePortal(r); });
    webserver->on("/gen_204", HTTP_GET, [this](AsyncWebServerRequest *r)
                  { captivePortal(r); });

    // OTA update
    setupOTA();

    // CORS global headers
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS,DELETE");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Max-Age", "600");

    webserver->begin();

    return true;
}

void FSWebServer::setCaptiveWebage(const char *url)
{
    m_apWebpage = (char *)realloc(m_apWebpage, sizeof(url));
    strcpy(m_apWebpage, url);
}

IPAddress FSWebServer::setAPmode(const char *ssid, const char *psk)
{
    m_apmode = true;
    WiFi.mode(WIFI_AP_STA);
    WiFi.persistent(false);
    WiFi.softAP(ssid, psk);
    m_dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    m_dnsServer.start(53, "*", WiFi.softAPIP());
    return WiFi.softAPIP();
}

IPAddress FSWebServer::startWiFi(uint32_t timeout, const char *apSSID, const char *apPsw)
{
    IPAddress ip;
    m_timeout = timeout;
    WiFi.mode(WIFI_STA);

    const char *_ssid;
    const char *_pass;

    wifi_config_t conf;
    esp_wifi_get_config(WIFI_IF_STA, &conf);

    _ssid = reinterpret_cast<const char *>(conf.sta.ssid);
    _pass = reinterpret_cast<const char *>(conf.sta.password);

    char *my_ssid = new char[33];
    strncpy(my_ssid, _ssid, 32);
    my_ssid[32] = '\0';
    _ssid = my_ssid;

    if (strlen(_ssid) && strlen(_pass))
    {
        WiFi.begin(_ssid, _pass, 0, 0, true);
        Serial.print(F("Connecting to "));
        Serial.println(_ssid);

        uint32_t startTime = millis();
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(300);
            Serial.print(".");
            if (WiFi.status() == WL_CONNECTED)
            {
                ip = WiFi.localIP();
                WiFi.persistent(true);
                delete[] my_ssid;
                return ip;
            }
            if (millis() - startTime > m_timeout)
            {
                Serial.println(F("No connection after a while -> go in Access Point mode"));
                break;
            }
        }
    }

    if (apSSID != nullptr && apPsw != nullptr)
        setAPmode(apSSID, apPsw);
    else
        setAPmode("ESP_AP", "123456789");

    WiFi.begin();
    ip = WiFi.softAPIP();
    Serial.print(F("\nAP mode.\nServer IP address: "));
    Serial.println(ip);
    Serial.println();
    delete[] my_ssid;
    return ip;
}

////////////////////////////////  WiFi  /////////////////////////////////////////

void FSWebServer::captivePortal(AsyncWebServerRequest *request)
{
    IPAddress ip = request->client()->localIP();
    char serverLoc[sizeof("https:://255.255.255.255/") + sizeof(m_apWebpage) + 1];
    snprintf(serverLoc, sizeof(serverLoc), "http://%d.%d.%d.%d%s", ip[0], ip[1], ip[2], ip[3], m_apWebpage);
    request->redirect(String(serverLoc));
}

void FSWebServer::handleRequest(AsyncWebServerRequest *request)
{
    if (!m_fsOK)
    {
        replyToCLient(request, ERROR, PSTR(FS_INIT_ERROR));
        return;
    }
    String _url = request->url();
    if (handleFileRead(request, _url))
        return;

    // SPA fallback: serve /web/index.html for non-API routes
    if (!_url.startsWith("/api/") && handleFileRead(request, "/web/index.html"))
        return;

    replyToCLient(request, NOT_FOUND, PSTR(FILE_NOT_FOUND));
}

void FSWebServer::getIpAddress(AsyncWebServerRequest *request)
{
    request->send(200, "text/json", WiFi.localIP().toString());
}

void FSWebServer::doRestart(AsyncWebServerRequest *request)
{
    Serial.println("RESTART");
    request->send(200, "text/json", "Going to restart ESP");
    delay(500);
    ESP.restart();
}

void FSWebServer::doWifiConnection(AsyncWebServerRequest *request)
{
    String ssid, pass;
    bool persistent = true;
    WiFi.mode(WIFI_AP_STA);

    String body = getBody(request);

    if (request->hasParam("ssid", true))
    {
        ssid = request->getParam("ssid", true)->value();
    }

    if (request->hasParam("password", true))
    {
        pass = request->getParam("password", true)->value();
    }

    if (request->hasParam("persistent", true))
    {
        String pers = request->getParam("persistent", true)->value();
        if (pers.equals("false"))
        {
            persistent = false;
        }
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        String resp = "ESP is currently connected to a WiFi network.<br><br>"
                      "Actual connection will be closed and a new attempt will be done with <b>";
        resp += ssid;
        resp += "</b> WiFi network.";
        request->send(200, "text/plain", resp);

        delay(500);
        Serial.println("Disconnect from current WiFi network");
        WiFi.disconnect();
    }

    if (ssid.length() && pass.length())
    {
        Serial.print("\nConnecting to ");
        Serial.println(ssid);
        WiFi.begin(ssid.c_str(), pass.c_str(), 0, 0, true);

        uint32_t beginTime = millis();
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(300);
            Serial.print("*.*");
            if (millis() - beginTime > m_timeout)
                break;
        }
        if (WiFi.status() == WL_CONNECTED)
        {
            IPAddress ip = WiFi.localIP();
            Serial.print("\nConnected to Wifi! IP address: ");
            Serial.println(ip);
            request->send(200, "text/plain", ip.toString());
            m_apmode = false;
            delay(500);
            ESP.restart();
            if (persistent)
            {
                wifi_config_t stationConf;
                esp_wifi_get_config(WIFI_IF_STA, &stationConf);
                memset(&stationConf, 0, sizeof(stationConf));
                memcpy(&stationConf.sta.ssid, ssid.c_str(), ssid.length());
                memcpy(&stationConf.sta.password, pass.c_str(), pass.length());
                esp_wifi_set_config(WIFI_IF_STA, &stationConf);
            }
            else
            {
                wifi_config_t stationConf;
                esp_wifi_get_config(WIFI_IF_STA, &stationConf);
                memset(&stationConf, 0, sizeof(stationConf));
                esp_wifi_set_config(WIFI_IF_STA, &stationConf);
            }
        }
        else
            request->send(500, "text/plain", "Connection error, maybe the password is wrong?");
    }
    request->send(500, "text/plain", "Wrong credentials provided");
}

void FSWebServer::handleScanNetworks(AsyncWebServerRequest *request)
{
    String jsonList = "[";
    DebugPrint("Scanning WiFi networks...");
    int n = WiFi.scanNetworks();
    DebugPrintln(" complete.");
    if (n == 0)
    {
        DebugPrintln("no networks found");
        request->send(200, "text/json", "[]");
        return;
    }
    else
    {
        DebugPrint(n);
        DebugPrintln(" networks found:");

        for (int i = 0; i < n; ++i)
        {
            String ssid = WiFi.SSID(i);
            int rssi = WiFi.RSSI(i);
            String security = WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "none" : "enabled";
            jsonList += "{\"ssid\":\"";
            jsonList += ssid;
            jsonList += "\",\"strength\":\"";
            jsonList += rssi;
            jsonList += "\",\"security\":";
            jsonList += security == "none" ? "false" : "true";
            jsonList += ssid.equals(WiFi.SSID()) ? ",\"selected\": true" : "";
            jsonList += i < n - 1 ? "}," : "}";
        }
        jsonList += "]";
    }
    request->send(200, "text/json", jsonList);
    DebugPrintln(jsonList);
}

void FSWebServer::handleIndex(AsyncWebServerRequest *request)
{
    // SPA: serve from /web/ if available
    if (handleFileRead(request, "/web/index.html"))
        return;
    if (m_filesystem->exists("/index.htm"))
    {
        handleFileRead(request, "/index.htm");
    }
    else if (m_filesystem->exists("/index.html"))
    {
        handleFileRead(request, "/index.html");
    }
}

/*
    Read the given file from the filesystem and stream it back to the client
*/
bool FSWebServer::handleFileRead(AsyncWebServerRequest *request, const String &uri)
{
    String path = uri;

    DebugPrintln("handleFileRead: " + path);
    if (path.endsWith("/"))
    {
        path += "index.htm";
    }
    String pathWithGz = path + ".gz";

    if (m_filesystem->exists(pathWithGz) || m_filesystem->exists(path))
    {
        if (m_filesystem->exists(pathWithGz))
        {
            path += ".gz";
        }
        const char *contentType = getContentType(path.c_str());
        request->send(*m_filesystem, path, contentType);
        return true;
    }
    return false;
}

/*
    Handle a file upload request
*/
void FSWebServer::handleFileUpload(AsyncWebServerRequest *request, const String &filename,
                                    size_t index, uint8_t *data, size_t len, bool final)
{
    if (index == 0)
    {
        // First chunk: open file
        String fname = filename;
        if (!fname.startsWith("/"))
        {
            fname = "/" + fname;
        }
        String result;
        checkForUnsupportedPath(fname, result);
        if (result.length() > 0)
        {
            return;
        }

        DebugPrintf_P(PSTR("handleFileUpload Name: %s\n"), fname.c_str());
        m_uploadFile = m_filesystem->open(fname, "w");
        if (!m_uploadFile)
        {
            DebugPrintln(PSTR("CREATE FAILED"));
            return;
        }
        DebugPrintf_P(PSTR("Upload: START, filename: %s\n"), fname.c_str());
    }

    // Write chunk
    if (m_uploadFile && len > 0)
    {
        size_t bytesWritten = m_uploadFile.write(data, len);
        if (bytesWritten != len)
        {
            DebugPrintln(PSTR("WRITE FAILED"));
        }
    }
    DebugPrintf_P(PSTR("Upload: WRITE, Bytes: %d\n"), len);

    if (final)
    {
        // Last chunk: close file
        if (m_uploadFile)
        {
            m_uploadFile.close();
        }
        DebugPrintf_P(PSTR("Upload: END, Size: %d\n"), index + len);
    }
}

void FSWebServer::replyOK(AsyncWebServerRequest *request)
{
    replyToCLient(request, OK, "");
}

void FSWebServer::replyToCLient(AsyncWebServerRequest *request, int msg_type = 0, const char *msg = "")
{
    switch (msg_type)
    {
    case OK:
        request->send(200, FPSTR(TEXT_PLAIN), "");
        break;
    case CUSTOM:
        request->send(200, FPSTR(TEXT_PLAIN), msg);
        break;
    case NOT_FOUND:
        if (request->method() == HTTP_OPTIONS)
        {
            request->send(204);
        }
        else
        {
            request->send(404, FPSTR(TEXT_PLAIN), msg);
        }
        break;
    case BAD_REQUEST:
        request->send(400, FPSTR(TEXT_PLAIN), msg);
        break;
    case ERROR:
        request->send(500, FPSTR(TEXT_PLAIN), msg);
        break;
    }
}

void FSWebServer::checkForUnsupportedPath(String &filename, String &error)
{
    if (!filename.startsWith("/"))
    {
        error += PSTR(" !! NO_LEADING_SLASH !! ");
    }
    if (filename.indexOf("//") != -1)
    {
        error += PSTR(" !! DOUBLE_SLASH !! ");
    }
    if (filename.endsWith("/"))
    {
        error += PSTR(" ! TRAILING_SLASH ! ");
    }
    DebugPrintln(filename);
    DebugPrintln(error);
}

const char *FSWebServer::getContentType(const char *filename)
{
    if (strstr(filename, ".htm"))
        return PSTR("text/html");
    else if (strstr(filename, ".html"))
        return PSTR("text/html");
    else if (strstr(filename, ".css"))
        return PSTR("text/css");
    else if (strstr(filename, ".sass"))
        return PSTR("text/css");
    else if (strstr(filename, ".js"))
        return PSTR("application/javascript");
    else if (strstr(filename, ".png"))
        return PSTR("image/png");
    else if (strstr(filename, ".svg"))
        return PSTR("image/svg+xml");
    else if (strstr(filename, ".gif"))
        return PSTR("image/gif");
    else if (strstr(filename, ".jpg"))
        return PSTR("image/jpeg");
    else if (strstr(filename, ".ico"))
        return PSTR("image/x-icon");
    else if (strstr(filename, ".xml"))
        return PSTR("text/xml");
    else if (strstr(filename, ".pdf"))
        return PSTR("application/x-pdf");
    else if (strstr(filename, ".zip"))
        return PSTR("application/x-zip");
    else if (strstr(filename, ".gz"))
        return PSTR("application/x-gzip");
    return PSTR("text/plain");
}

// OTA firmware update handler
void FSWebServer::setupOTA()
{
    // Serve OTA upload form
    webserver->on("/update", HTTP_GET, [this](AsyncWebServerRequest *request)
                  {
        if (!checkAuth(request)) return;
        request->send_P(200, "text/html", update_html); });

    // Handle firmware upload
    webserver->on(
        "/update", HTTP_POST,
        // onRequest: called after upload finishes
        [this](AsyncWebServerRequest *request)
        {
            AsyncWebServerResponse *response;
            if (Update.hasError())
            {
                response = request->beginResponse(500, "text/html",
                                                   "<META http-equiv=\"refresh\" content=\"5;URL=/update\">Update Failed! Rebooting...");
            }
            else
            {
                response = request->beginResponse(200, "text/html",
                                                   "<META http-equiv=\"refresh\" content=\"15;URL=/\">Update Success! Rebooting...");
            }
            response->addHeader("Connection", "close");
            request->send(response);
            request->onDisconnect([]()
                                  { ESP.restart(); });
        },
        // onUpload: called for each chunk
        [this](AsyncWebServerRequest *request, const String &filename,
               size_t index, uint8_t *data, size_t len, bool final)
        {
            if (index == 0)
            {
                if (!checkAuth(request))
                    return;
                Serial.printf("OTA Update: %s\n", filename.c_str());
                if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000))
                {
                    Update.printError(Serial);
                }
            }
            if (!Update.hasError())
            {
                if (Update.write(data, len) != len)
                {
                    Update.printError(Serial);
                }
            }
            if (final)
            {
                if (Update.end(true))
                {
                    Serial.printf("OTA Update Success: %uB\n", index + len);
                }
                else
                {
                    Update.printError(Serial);
                }
            }
        });
}

// File manager routes (list, create, delete, status, edit)

void FSWebServer::handleFileList(AsyncWebServerRequest *request)
{
    if (!request->hasParam("dir"))
    {
        return replyToCLient(request, BAD_REQUEST, "DIR ARG MISSING");
    }

    String path = request->getParam("dir")->value();
    DebugPrintln("handleFileList: " + path);
    if (path != "/" && !m_filesystem->exists(path))
    {
        return replyToCLient(request, BAD_REQUEST, "BAD PATH");
    }

    File root = m_filesystem->open(path, "r");
    path = String();
    String output;
    output.reserve(256);
    output = "[";
    if (root.isDirectory())
    {
        File file = root.openNextFile();
        while (file)
        {
            String filename = file.name();
            if (filename.lastIndexOf("/") > -1)
            {
                filename.remove(0, filename.lastIndexOf("/") + 1);
            }
            if (output != "[")
            {
                output += ',';
            }
            output += "{\"type\":\"";
            output += (file.isDirectory()) ? "dir" : "file";
            output += "\",\"size\":\"";
            output += file.size();
            output += "\",\"name\":\"";
            output += filename;
            output += "\"}";
            file = root.openNextFile();
        }
    }
    output += "]";
    request->send(200, "text/json", output);
}

void FSWebServer::handleFileCreate(AsyncWebServerRequest *request)
{
    String path;
    if (request->hasParam("path", true))
        path = request->getParam("path", true)->value();
    else if (request->hasParam("path"))
        path = request->getParam("path")->value();

    if (path.isEmpty())
    {
        replyToCLient(request, BAD_REQUEST, PSTR("PATH ARG MISSING"));
        return;
    }
    if (path == "/")
    {
        replyToCLient(request, BAD_REQUEST, PSTR("BAD PATH"));
        return;
    }

    String src;
    if (request->hasParam("src", true))
        src = request->getParam("src", true)->value();
    else if (request->hasParam("src"))
        src = request->getParam("src")->value();

    if (src.isEmpty())
    {
        DebugPrintf_P(PSTR("handleFileCreate: %s\n"), path.c_str());
        if (path.endsWith("/"))
        {
            path.remove(path.length() - 1);
            if (!m_filesystem->mkdir(path))
            {
                replyToCLient(request, ERROR, PSTR("MKDIR FAILED"));
                return;
            }
        }
        else
        {
            File file = m_filesystem->open(path, "w");
            if (file)
            {
                file.write(0);
                file.close();
            }
            else
            {
                replyToCLient(request, ERROR, PSTR("CREATE FAILED"));
                return;
            }
        }
        replyToCLient(request, CUSTOM, path.c_str());
    }
    else
    {
        if (src == "/")
        {
            replyToCLient(request, BAD_REQUEST, PSTR("BAD SRC"));
            return;
        }
        if (!m_filesystem->exists(src))
        {
            replyToCLient(request, BAD_REQUEST, PSTR("BSRC FILE NOT FOUND"));
            return;
        }

        DebugPrintf_P(PSTR("handleFileCreate: %s from %s\n"), path.c_str(), src.c_str());
        if (path.endsWith("/"))
        {
            path.remove(path.length() - 1);
        }
        if (src.endsWith("/"))
        {
            src.remove(src.length() - 1);
        }
        if (!m_filesystem->rename(src, path))
        {
            replyToCLient(request, ERROR, PSTR("RENAME FAILED"));
            return;
        }
        replyOK(request);
    }
}

void FSWebServer::handleFileDelete(AsyncWebServerRequest *request)
{
    String path;
    if (request->params() > 0)
        path = request->getParam(0)->value();

    if (path.isEmpty() || path == "/")
    {
        replyToCLient(request, BAD_REQUEST, PSTR("BAD PATH"));
        return;
    }

    DebugPrintf_P(PSTR("handleFileDelete: %s\n"), path.c_str());
    if (!m_filesystem->exists(path))
    {
        replyToCLient(request, NOT_FOUND, PSTR(FILE_NOT_FOUND));
        return;
    }
    File root = m_filesystem->open(path, "r");
    if (!root.isDirectory())
    {
        root.close();
        m_filesystem->remove(path);
        replyOK(request);
    }
    else
    {
        m_filesystem->rmdir(path);
        replyOK(request);
    }
}

void FSWebServer::handleGetEdit(AsyncWebServerRequest *request)
{
    // File manager now served from SPA — redirect to /files
    if (handleFileRead(request, "/web/index.html"))
        return;
    replyToCLient(request, NOT_FOUND, PSTR("SPA not found in /web/"));
}

void FSWebServer::handleStatus(AsyncWebServerRequest *request)
{
    DebugPrintln(PSTR("handleStatus"));

    size_t totalBytes = 1024;
    size_t usedBytes = 0;

    totalBytes = LittleFS.totalBytes();
    usedBytes = LittleFS.usedBytes();

    String json;
    json.reserve(256);
    json = "{\"type\":\"Filesystem\", \"isOk\":";
    if (m_fsOK)
    {
        uint32_t ip = (WiFi.status() == WL_CONNECTED) ? (uint32_t)WiFi.localIP() : (uint32_t)WiFi.softAPIP();
        json += PSTR("\"true\", \"totalBytes\":\"");
        json += totalBytes;
        json += PSTR("\", \"usedBytes\":\"");
        json += usedBytes;
        json += PSTR("\", \"mode\":\"");
        json += WiFi.status() == WL_CONNECTED ? "Station" : "Access Point";
        json += PSTR("\", \"ip\":\"");
        json += ip;
        json += PSTR("\", \"ssid\":\"");
        json += WiFi.SSID();
        json += "\"";
    }
    else
    {
        json += "\"false\"";
    }

    json += "}";
    request->send(200, "application/json", json);
}
