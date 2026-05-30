import { useState, useEffect, useMemo } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Select, Card, FormRow, Button } from "../../../components/ui";
import { getWeatherData, forceWeatherFetch } from "../../../api/client";
import { toast } from "../../../components/Toast";
import { useT } from "../../../i18n";
import type { WeatherData } from "../../../api/types";
import styles from "./sections.module.css";

export function WeatherApiSection() {
  const { weatherConfig, updateWeatherConfig, saveWeatherConfig } = useSettings();
  const [saving, setSaving] = useState(false);
  const [fetching, setFetching] = useState(false);
  const [weatherData, setWeatherData] = useState<WeatherData | null>(null);
  const t = useT();

  const locationTypes = useMemo(() => [
    { value: 0, label: t.settings.cityName },
    { value: 1, label: t.settings.coordinates },
    { value: 2, label: t.settings.autoIp },
    { value: 3, label: t.settings.stationId },
  ], [t]);

  const updateIntervals = useMemo(() => [
    { value: 10, label: `10 ${t.intervals.minutes}` },
    { value: 15, label: `15 ${t.intervals.minutes}` },
    { value: 30, label: `30 ${t.intervals.minutes}` },
    { value: 60, label: `60 ${t.intervals.minutes}` },
  ], [t]);

  useEffect(() => {
    getWeatherData().then(setWeatherData).catch(() => {});
  }, []);

  if (!weatherConfig) return null;
  const w = weatherConfig;

  async function handleSave() {
    setSaving(true);
    await saveWeatherConfig(t.settings.weatherApiSaved);
    setSaving(false);
  }

  async function handleFetch() {
    setFetching(true);
    try {
      await forceWeatherFetch();
      await new Promise((r) => setTimeout(r, 1500));
      const data = await getWeatherData();
      setWeatherData(data);
      if (data?.valid) {
        toast(t.ok);
      } else {
        toast(t.settings.noWeatherData);
      }
    } catch {
      toast(t.error);
    }
    setFetching(false);
  }

  return (
    <Card title={t.settings.weatherApi}>
      <div class={styles.stack}>
        <div class="form-group">
          <label htmlFor="weather-api-key">{t.settings.apiKey}</label>
          <input
            id="weather-api-key"
            type="text"
            value={w.apiKey}
            onInput={(e) => updateWeatherConfig({ apiKey: (e.target as HTMLInputElement).value })}
          />
        </div>

        <Select
          label={t.settings.locationMethod}
          value={w.locationType}
          options={locationTypes}
          onChange={(v) => updateWeatherConfig({ locationType: v as number })}
        />

        {w.locationType === 0 && (
          <div class="form-group">
            <label htmlFor="weather-city">{t.settings.city}</label>
            <input
              id="weather-city"
              type="text"
              value={w.city}
              onInput={(e) => updateWeatherConfig({ city: (e.target as HTMLInputElement).value })}
            />
          </div>
        )}

        {w.locationType === 1 && (
          <FormRow>
            <div class="form-group">
              <label htmlFor="weather-lat">{t.settings.latitude}</label>
              <input
                id="weather-lat"
                type="text"
                inputMode="decimal"
                value={w.latitude}
                onBlur={(e) => {
                  const val = parseFloat((e.target as HTMLInputElement).value);
                  updateWeatherConfig({ latitude: isNaN(val) ? 0 : val });
                }}
              />
            </div>
            <div class="form-group">
              <label htmlFor="weather-lon">{t.settings.longitude}</label>
              <input
                id="weather-lon"
                type="text"
                inputMode="decimal"
                value={w.longitude}
                onBlur={(e) => {
                  const val = parseFloat((e.target as HTMLInputElement).value);
                  updateWeatherConfig({ longitude: isNaN(val) ? 0 : val });
                }}
              />
            </div>
          </FormRow>
        )}

        {w.locationType === 3 && (
          <div class="form-group">
            <label htmlFor="weather-station">{t.settings.stationId}</label>
            <input
              id="weather-station"
              type="text"
              value={w.stationId}
              onInput={(e) => updateWeatherConfig({ stationId: (e.target as HTMLInputElement).value })}
            />
          </div>
        )}

        <Select
          label={t.settings.updateInterval}
          value={w.updateInterval}
          options={updateIntervals}
          onChange={(v) => updateWeatherConfig({ updateInterval: v as number })}
        />

        <FormRow>
          <Button variant="primary" onClick={handleSave} loading={saving}>
            {t.settings.saveWeatherApi}
          </Button>
          <Button variant="default" onClick={handleFetch} loading={fetching} disabled={!w.apiKey}>
            {t.settings.fetchNow}
          </Button>
        </FormRow>

        {weatherData && weatherData.valid && (
          <div class={styles.weatherStatus}>
            <p class={styles.weatherTitle}>{t.settings.currentWeatherData}</p>
            <div class={styles.weatherTwoCol}>
              <div class={styles.weatherCol}>
                <div class={styles.weatherItem}><span>{t.settings.temperature}:</span><span>{weatherData.outdoorTemp.toFixed(1)}°</span></div>
                <div class={styles.weatherItem}><span>{t.settings.humidity}:</span><span>{weatherData.outdoorHumidity.toFixed(0)}%</span></div>
                <div class={styles.weatherItem}><span>{t.settings.pressure}:</span><span>{weatherData.pressure.toFixed(0)} mb</span></div>
              </div>
              <div class={styles.weatherCol}>
                <div class={styles.weatherItem}><span>{t.settings.condition}:</span><span>{weatherData.condition}</span></div>
                <div class={styles.weatherItem}><span>AQI:</span><span>{weatherData.aqi > 0 ? weatherData.aqi : "--"}</span></div>
                <div class={styles.weatherItem}><span>{t.settings.uvIndex}:</span><span>{weatherData.uv?.toFixed(1) ?? "--"}</span></div>
              </div>
            </div>
          </div>
        )}

        {weatherData && !weatherData.valid && w.apiKey && (
          <p class={styles.hint}>{t.settings.noWeatherData}</p>
        )}
      </div>
    </Card>
  );
}
