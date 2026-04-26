#pragma once

#include <Arduino.h>
#include <vector>
#include "DataFetcherConfig.h"

class IDisplayNavigation;

class DataFetcher_
{
  private:
    DataFetcher_() = default;
    DataFetcher_(const DataFetcher_&) = delete;
    DataFetcher_& operator=(const DataFetcher_&) = delete;
    DataFetcher_(DataFetcher_&&) = delete;
    DataFetcher_& operator=(DataFetcher_&&) = delete;

    IDisplayNavigation *nav_ = nullptr;
    std::vector<DataSourceConfig> sources_;
    std::vector<unsigned long> lastFetch_;
    size_t nextFetchIndex_ = 0;

    bool fetchAndPush(size_t index);
    String extractJsonValue(const String& json, const String& path);
    static String buildCustomAppJson(const DataSourceConfig& src, const String& value);
    static String formatValue(const DataSourceConfig& src, const String& raw);

  public:
    static DataFetcher_& getInstance();

    void setNavigation(IDisplayNavigation *n);
    bool hasNavigation() const;

    void setup();
    void tick();

    bool addSource(const char *json);
    bool removeSource(const String& name);
    String getSourcesAsJson();
    void forceFetch(const String& name);
    void loadSources();
    void saveSources();
};

extern DataFetcher_& DataFetcher;
