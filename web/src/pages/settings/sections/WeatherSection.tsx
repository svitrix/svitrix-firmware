import { useState, useEffect } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Select, Card, FormRow, Button, Slider, ColorField } from "../../../components/ui";
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

export function WeatherSection() {
  const { weatherConfig, settings, updateWeatherConfig, updateSettings, saveWeatherConfig } = useSettings();
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

        <p class={styles.hint}>Weather Apps</p>

        <div class={styles.appRow}>
          <Toggle label="Outdoor Temp" checked={w.showOutdoorTemp} onChange={(v) => updateWeatherConfig({ showOutdoorTemp: v })} />
          <ColorField label="" value={w.outdoorTempColor} onChange={(v) => updateWeatherConfig({ outdoorTempColor: v })} />
          {settings && (
            <Toggle label="Celsius" checked={settings.CEL} onChange={(v) => updateSettings({ CEL: v })} />
          )}
          <div style={{ width: "515px", flexShrink: 0 }}>
            <Slider label="" min={1} max={60} value={w.outdoorTempDuration || 7} onChange={(v) => updateWeatherConfig({ outdoorTempDuration: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label="Outdoor Hum" checked={w.showOutdoorHumidity} onChange={(v) => updateWeatherConfig({ showOutdoorHumidity: v })} />
          <ColorField label="" value={w.outdoorHumColor} onChange={(v) => updateWeatherConfig({ outdoorHumColor: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={w.outdoorHumDuration || 7} onChange={(v) => updateWeatherConfig({ outdoorHumDuration: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label="Pressure" checked={w.showPressure} onChange={(v) => updateWeatherConfig({ showPressure: v })} />
          <ColorField label="" value={w.pressureColor} onChange={(v) => updateWeatherConfig({ pressureColor: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={w.pressureDuration || 7} onChange={(v) => updateWeatherConfig({ pressureDuration: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label="Air Quality" checked={w.showAirQuality} onChange={(v) => updateWeatherConfig({ showAirQuality: v })} />
          <ColorField label={w.aqiColor === 0 ? "Auto" : ""} value={w.aqiColor} onChange={(v) => updateWeatherConfig({ aqiColor: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={w.aqiDuration || 7} onChange={(v) => updateWeatherConfig({ aqiDuration: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label="UV Index" checked={w.showUV} onChange={(v) => updateWeatherConfig({ showUV: v })} />
          <ColorField label={w.uvColor === 0 ? "Auto" : ""} value={w.uvColor} onChange={(v) => updateWeatherConfig({ uvColor: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={w.uvDuration || 7} onChange={(v) => updateWeatherConfig({ uvDuration: v })} unit="s" />
          </div>
        </div>

        <p class={styles.hint}>
          Air Quality and UV use automatic colors based on level (green→yellow→orange→red). Set a custom color to override.
        </p>

        <FormRow>
          <Button variant="primary" onClick={handleSave} loading={saving}>
            Save Weather
          </Button>
          <Button variant="default" onClick={handleFetch} loading={fetching} disabled={!w.apiKey}>
            Fetch Now
          </Button>
        </FormRow>

        {weatherData && weatherData.valid && (
          <div class={styles.weatherStatus}>
            <p class={styles.hint}>Current Weather Data</p>
            <div class={styles.weatherGrid}>
              <span>Temperature:</span>
              <span>{weatherData.outdoorTemp.toFixed(1)}°</span>
              <span>Humidity:</span>
              <span>{weatherData.outdoorHumidity.toFixed(0)}%</span>
              <span>Pressure:</span>
              <span>{weatherData.pressure.toFixed(0)} mb</span>
              <span>Condition:</span>
              <span>{weatherData.condition}</span>
              {weatherData.aqi > 0 && (
                <>
                  <span>AQI:</span>
                  <span>{weatherData.aqi}</span>
                </>
              )}
              <span>UV Index:</span>
              <span>{weatherData.uv?.toFixed(1) ?? "--"}</span>
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
