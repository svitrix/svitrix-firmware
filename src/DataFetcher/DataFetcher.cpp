#include "DataFetcher.h"
#include "Globals.h"
#include "IDisplayNavigation.h"
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <LittleFS.h>

extern const char *rootCACertificate;

static const char *TAG = "DataFetcher";
static const char *SOURCES_PATH = "/DATAFETCHER/sources.json";
static constexpr size_t MAX_RESPONSE_SIZE = 4096;
static constexpr uint32_t HTTP_CONNECT_TIMEOUT = 5000;
static constexpr uint32_t HTTP_READ_TIMEOUT = 10000;
static constexpr uint32_t MIN_FREE_HEAP = 40000;

DataFetcher_& DataFetcher_::getInstance()
{
    static DataFetcher_ instance;
    return instance;
}

DataFetcher_& DataFetcher = DataFetcher_::getInstance();

void DataFetcher_::setNavigation(IDisplayNavigation *n)
{
    nav_ = n;
}

bool DataFetcher_::hasNavigation() const
{
    return nav_ != nullptr;
}

void DataFetcher_::setup()
{
    LittleFS.mkdir("/DATAFETCHER");
    loadSources();
    DEBUG_PRINTF("DataFetcher: loaded %d sources", sources_.size());
}

// ---------- tick: staggered one-source-per-call ----------

void DataFetcher_::tick()
{
    if (!nav_ || sources_.empty())
        return;

    unsigned long now = millis();

    // Round-robin: check one source per tick to avoid blocking the loop
    size_t idx = nextFetchIndex_ % sources_.size();
    if (now - lastFetch_[idx] >= sources_[idx].interval * 1000UL)
    {
        if (ESP.getFreeHeap() > MIN_FREE_HEAP)
        {
            fetchAndPush(idx);
            lastFetch_[idx] = now;
        }
        else
        {
            DEBUG_PRINTLN(F("DataFetcher: low heap, skipping fetch"));
        }
    }
    nextFetchIndex_ = (idx + 1) % sources_.size();
}

// ---------- HTTP fetch + push to custom app ----------

bool DataFetcher_::fetchAndPush(size_t index)
{
    const DataSourceConfig& src = sources_[index];
    bool isHttps = src.url.startsWith("https");

    HTTPClient http;
    WiFiClientSecure secClient;
    WiFiClient plainClient;

    if (isHttps)
    {
        // Don't validate cert — DataFetcher hits arbitrary third-party APIs
        // whose CAs we can't pin in advance.
        secClient.setInsecure();
        http.begin(secClient, src.url);
    }
    else
    {
        http.begin(plainClient, src.url);
    }

    http.setConnectTimeout(HTTP_CONNECT_TIMEOUT);
    http.setTimeout(HTTP_READ_TIMEOUT);
    http.addHeader("Accept", "application/json");

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK)
    {
        DEBUG_PRINTF("DataFetcher: GET %s failed: %d", src.name.c_str(), httpCode);
        http.end();
        return false;
    }

    int contentLength = http.getSize();
    if (contentLength > (int)MAX_RESPONSE_SIZE)
    {
        DEBUG_PRINTF("DataFetcher: response too large (%d bytes)", contentLength);
        http.end();
        return false;
    }

    String body = http.getString();
    http.end();

    String value = extractJsonValue(body, src.jsonPath);
    if (value.isEmpty())
    {
        DEBUG_PRINTF("DataFetcher: path '%s' not found in response", src.jsonPath.c_str());
        return false;
    }

    String formatted = formatValue(src, value);
    String appJson = buildCustomAppJson(src, formatted);

    // Feed into the existing custom app pipeline
    nav_->parseCustomPage(src.name, appJson.c_str(), true);

    DEBUG_PRINTF("DataFetcher: %s = %s", src.name.c_str(), formatted.c_str());
    return true;
}

// ---------- JSON path extraction (dot-notation, supports array indices as numbers) ----------

String DataFetcher_::extractJsonValue(const String& json, const String& path)
{
    DynamicJsonDocument doc(2048);
    DeserializationError err = deserializeJson(doc, json);
    if (err)
        return "";

    // Walk the dot-separated path: "bpi.USD.rate_float" or "data.0.price"
    JsonVariant current = doc.as<JsonVariant>();
    int start = 0;
    int dot;

    while (start < (int)path.length())
    {
        dot = path.indexOf('.', start);
        if (dot < 0)
            dot = path.length();

        String segment = path.substring(start, dot);

        if (current.is<JsonArray>())
        {
            int idx = segment.toInt();
            current = current[idx];
        }
        else if (current.is<JsonObject>())
        {
            current = current[segment];
        }
        else
        {
            return "";
        }

        if (current.isNull())
            return "";

        start = dot + 1;
    }

    // Return as string regardless of type
    if (current.is<const char *>())
        return current.as<const char *>();
    if (current.is<float>() || current.is<double>())
    {
        char buf[32];
        snprintf(buf, sizeof(buf), "%g", current.as<double>());
        return buf;
    }
    if (current.is<long>())
        return String(current.as<long>());
    if (current.is<bool>())
        return current.as<bool>() ? "true" : "false";

    // Fallback: serialize whatever it is
    String result;
    serializeJson(current, result);
    return result;
}

// ---------- Format value with printf-style pattern ----------

String DataFetcher_::formatValue(const DataSourceConfig& src, const String& raw)
{
    if (src.displayFormat.isEmpty())
        return raw;

    // Try to interpret as a number for printf formatting
    char buf[64];
    if (src.displayFormat.indexOf('f') >= 0 || src.displayFormat.indexOf('d') >= 0 ||
        src.displayFormat.indexOf('i') >= 0 || src.displayFormat.indexOf('g') >= 0)
    {
        double val = raw.toDouble();
        snprintf(buf, sizeof(buf), src.displayFormat.c_str(), val);
    }
    else
    {
        snprintf(buf, sizeof(buf), src.displayFormat.c_str(), raw.c_str());
    }
    return buf;
}

// ---------- Build custom app JSON ----------

String DataFetcher_::buildCustomAppJson(const DataSourceConfig& src, const String& value)
{
    StaticJsonDocument<512> doc;
    doc["text"] = value;
    doc["lifetime"] = 0; // No expiry — DataFetcher manages updates
    doc["noScroll"] = false;

    if (!src.icon.isEmpty())
        doc["icon"] = src.icon;
    if (!src.textColor.isEmpty())
        doc["color"] = src.textColor;

    String result;
    serializeJson(doc, result);
    return result;
}

// ---------- Source management ----------

bool DataFetcher_::addSource(const char *json)
{
    DynamicJsonDocument doc(1024);
    if (deserializeJson(doc, json))
        return false;

    if (!doc.containsKey("name") || !doc.containsKey("url") || !doc.containsKey("jsonPath"))
        return false;

    DataSourceConfig cfg;
    cfg.name = doc["name"].as<String>();
    cfg.url = doc["url"].as<String>();
    cfg.jsonPath = doc["jsonPath"].as<String>();
    cfg.displayFormat = doc["displayFormat"] | "";
    cfg.icon = doc["icon"] | "";
    cfg.textColor = doc["color"] | "";
    cfg.interval = doc["interval"] | DataSourceConfig::DEFAULT_INTERVAL;

    if (cfg.interval < DataSourceConfig::MIN_INTERVAL)
        cfg.interval = DataSourceConfig::MIN_INTERVAL;

    // Update existing or add new
    for (size_t i = 0; i < sources_.size(); i++)
    {
        if (sources_[i].name == cfg.name)
        {
            sources_[i] = cfg;
            saveSources();
            return true;
        }
    }

    if (sources_.size() >= DataSourceConfig::MAX_SOURCES)
    {
        DEBUG_PRINTLN(F("DataFetcher: max sources reached"));
        return false;
    }

    sources_.push_back(cfg);
    lastFetch_.push_back(0);
    saveSources();
    return true;
}

bool DataFetcher_::removeSource(const String& name)
{
    for (size_t i = 0; i < sources_.size(); i++)
    {
        if (sources_[i].name == name)
        {
            // Remove the custom app from display
            if (nav_)
                nav_->parseCustomPage(name, "{}", false);

            sources_.erase(sources_.begin() + i);
            lastFetch_.erase(lastFetch_.begin() + i);
            if (nextFetchIndex_ >= sources_.size() && !sources_.empty())
                nextFetchIndex_ = 0;

            saveSources();
            return true;
        }
    }
    return false;
}

String DataFetcher_::getSourcesAsJson()
{
    DynamicJsonDocument doc(2048);
    JsonArray arr = doc.to<JsonArray>();

    for (const auto& src : sources_)
    {
        JsonObject obj = arr.createNestedObject();
        obj["name"] = src.name;
        obj["url"] = src.url;
        obj["jsonPath"] = src.jsonPath;
        obj["displayFormat"] = src.displayFormat;
        obj["icon"] = src.icon;
        obj["color"] = src.textColor;
        obj["interval"] = src.interval;
    }

    String result;
    serializeJson(doc, result);
    return result;
}

void DataFetcher_::forceFetch(const String& name)
{
    for (size_t i = 0; i < sources_.size(); i++)
    {
        if (sources_[i].name == name)
        {
            fetchAndPush(i);
            lastFetch_[i] = millis();
            return;
        }
    }
}

// ---------- LittleFS persistence ----------

void DataFetcher_::loadSources()
{
    File file = LittleFS.open(SOURCES_PATH, "r");
    if (!file)
        return;

    DynamicJsonDocument doc(2048);
    if (deserializeJson(doc, file))
    {
        file.close();
        return;
    }
    file.close();

    JsonArray arr = doc.as<JsonArray>();
    sources_.clear();
    lastFetch_.clear();

    for (JsonObject obj : arr)
    {
        DataSourceConfig cfg;
        cfg.name = obj["name"].as<String>();
        cfg.url = obj["url"].as<String>();
        cfg.jsonPath = obj["jsonPath"].as<String>();
        cfg.displayFormat = obj["displayFormat"] | "";
        cfg.icon = obj["icon"] | "";
        cfg.textColor = obj["color"] | "";
        cfg.interval = obj["interval"] | 300;

        if (cfg.interval < DataSourceConfig::MIN_INTERVAL)
            cfg.interval = DataSourceConfig::MIN_INTERVAL;

        sources_.push_back(cfg);
        lastFetch_.push_back(0); // Fetch on first tick
    }
}

void DataFetcher_::saveSources()
{
    DynamicJsonDocument doc(2048);
    JsonArray arr = doc.to<JsonArray>();

    for (const auto& src : sources_)
    {
        JsonObject obj = arr.createNestedObject();
        obj["name"] = src.name;
        obj["url"] = src.url;
        obj["jsonPath"] = src.jsonPath;
        obj["displayFormat"] = src.displayFormat;
        obj["icon"] = src.icon;
        obj["color"] = src.textColor;
        obj["interval"] = src.interval;
    }

    File file = LittleFS.open(SOURCES_PATH, "w");
    if (!file)
    {
        DEBUG_PRINTLN(F("DataFetcher: failed to open file for writing"));
        return;
    }
    serializeJson(doc, file);
    file.close();
}
