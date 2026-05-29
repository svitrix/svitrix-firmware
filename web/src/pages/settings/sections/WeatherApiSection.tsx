import { useState, useEffect } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Select, Card, FormRow, Button } from "../../../components/ui";
import { getWeatherData, forceWeatherFetch } from "../../../api/client";
import type { WeatherData } from "../../../api/types";
import styles from "./sections.module.css";

const LOCATION_TYPES = [
  { value: 0, label: "City Name" },
  { value: 1, label: "Coordinates" },
  { value: 2, label: "Auto (IP)" },
  { value: 3, label: "Station ID" },
];

const UPDATE_INTERVALS = [
  { value: 10, label: "10 minutes" },
  { value: 15, label: "15 minutes" },
  { value: 30, label: "30 minutes" },
  { value: 60, label: "60 minutes" },
];

export function WeatherApiSection() {
  const { weatherConfig, updateWeatherConfig, saveWeatherConfig } = useSettings();
  const [saving, setSaving] = useState(false);
  const [fetching, setFetching] = useState(false);
  const [weatherData, setWeatherData] = useState<WeatherData | null>(null);

  useEffect(() => {
    getWeatherData().then(setWeatherData).catch(() => {});
  }, []);

  if (!weatherConfig) return null;
  const w = weatherConfig;

  async function handleSave() {
    setSaving(true);
    await saveWeatherConfig();
    setSaving(false);
  }

  async function handleFetch() {
    setFetching(true);
    try {
      await forceWeatherFetch();
      await new Promise((r) => setTimeout(r, 1500));
      const data = await getWeatherData();
      setWeatherData(data);
    } catch {
      // ignore
    }
    setFetching(false);
  }

  return (
    <Card title="Weather API">
      <div class={styles.stack}>
        <div class="form-group">
          <label htmlFor="weather-api-key">API Key (weatherapi.com)</label>
          <input
            id="weather-api-key"
            type="text"
            value={w.apiKey}
            onInput={(e) => updateWeatherConfig({ apiKey: (e.target as HTMLInputElement).value })}
            placeholder="Enter your API key"
          />
        </div>

        <Select
          label="Location Method"
          value={w.locationType}
          options={LOCATION_TYPES}
          onChange={(v) => updateWeatherConfig({ locationType: v as number })}
        />

        {w.locationType === 0 && (
          <div class="form-group">
            <label htmlFor="weather-city">City</label>
            <input
              id="weather-city"
              type="text"
              value={w.city}
              onInput={(e) => updateWeatherConfig({ city: (e.target as HTMLInputElement).value })}
              placeholder="e.g. Mexico City"
            />
          </div>
        )}

        {w.locationType === 1 && (
          <FormRow>
            <div class="form-group">
              <label htmlFor="weather-lat">Latitude</label>
              <input
                id="weather-lat"
                type="text"
                inputMode="decimal"
                value={w.latitude}
                onBlur={(e) => {
                  const val = parseFloat((e.target as HTMLInputElement).value);
                  updateWeatherConfig({ latitude: isNaN(val) ? 0 : val });
                }}
                placeholder="e.g. 19.4326"
              />
            </div>
            <div class="form-group">
              <label htmlFor="weather-lon">Longitude</label>
              <input
                id="weather-lon"
                type="text"
                inputMode="decimal"
                value={w.longitude}
                onBlur={(e) => {
                  const val = parseFloat((e.target as HTMLInputElement).value);
                  updateWeatherConfig({ longitude: isNaN(val) ? 0 : val });
                }}
                placeholder="e.g. -99.1332"
              />
            </div>
          </FormRow>
        )}

        {w.locationType === 3 && (
          <div class="form-group">
            <label htmlFor="weather-station">Station ID</label>
            <input
              id="weather-station"
              type="text"
              value={w.stationId}
              onInput={(e) => updateWeatherConfig({ stationId: (e.target as HTMLInputElement).value })}
              placeholder="e.g. pws:KMAHANOV10"
            />
          </div>
        )}

        <Select
          label="Update Interval"
          value={w.updateInterval}
          options={UPDATE_INTERVALS}
          onChange={(v) => updateWeatherConfig({ updateInterval: v as number })}
        />

        <FormRow>
          <Button variant="primary" onClick={handleSave} loading={saving}>
            Save Weather API
          </Button>
          <Button variant="default" onClick={handleFetch} loading={fetching} disabled={!w.apiKey}>
            Fetch Now
          </Button>
        </FormRow>

        {weatherData && weatherData.valid && (
          <div class={styles.weatherStatus}>
            <p class={styles.weatherTitle}>Current Weather Data</p>
            <div class={styles.weatherTwoCol}>
              <div class={styles.weatherCol}>
                <div class={styles.weatherItem}><span>Temperature:</span><span>{weatherData.outdoorTemp.toFixed(1)}°</span></div>
                <div class={styles.weatherItem}><span>Humidity:</span><span>{weatherData.outdoorHumidity.toFixed(0)}%</span></div>
                <div class={styles.weatherItem}><span>Pressure:</span><span>{weatherData.pressure.toFixed(0)} mb</span></div>
              </div>
              <div class={styles.weatherCol}>
                <div class={styles.weatherItem}><span>Condition:</span><span>{weatherData.condition}</span></div>
                <div class={styles.weatherItem}><span>AQI:</span><span>{weatherData.aqi > 0 ? weatherData.aqi : "--"}</span></div>
                <div class={styles.weatherItem}><span>UV Index:</span><span>{weatherData.uv?.toFixed(1) ?? "--"}</span></div>
              </div>
            </div>
          </div>
        )}

        {weatherData && !weatherData.valid && w.apiKey && (
          <p class={styles.hint}>No weather data yet. Click "Fetch Now" to test your API key.</p>
        )}
      </div>
    </Card>
  );
}
