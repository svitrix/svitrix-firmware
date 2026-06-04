import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Card } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function WeatherAppsSection() {
  const { weatherConfig, settings, instantSave, autoSaveWeather, instantSaveWeather } = useSettings();
  const t = useT();

  if (!weatherConfig || !settings) return null;
  const w = weatherConfig;

  return (
    <Card title={t.apps.weatherApps}>
      <div class={styles.stack}>
        <div class={styles.appRow}>
          <Toggle label={t.apps.outdoorTemp} checked={w.showOutdoorTemp} onChange={(v) => instantSaveWeather({ showOutdoorTemp: v })} />
          <ColorField label="" value={w.outdoorTempColor} onChange={(v) => autoSaveWeather({ outdoorTempColor: v })} />
          <Toggle label={t.apps.celsius} checked={settings.CEL} onChange={(v) => instantSave({ CEL: v })} />
          <div style={{ width: "515px", flexShrink: 0 }}>
            <Slider label="" min={1} max={60} value={w.outdoorTempDuration || 7} onChange={(v) => autoSaveWeather({ outdoorTempDuration: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label={t.apps.outdoorHum} checked={w.showOutdoorHumidity} onChange={(v) => instantSaveWeather({ showOutdoorHumidity: v })} />
          <ColorField label="" value={w.outdoorHumColor} onChange={(v) => autoSaveWeather({ outdoorHumColor: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={w.outdoorHumDuration || 7} onChange={(v) => autoSaveWeather({ outdoorHumDuration: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label={t.apps.pressure} checked={w.showPressure} onChange={(v) => instantSaveWeather({ showPressure: v })} />
          <ColorField label="" value={w.pressureColor} onChange={(v) => autoSaveWeather({ pressureColor: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={w.pressureDuration || 7} onChange={(v) => autoSaveWeather({ pressureDuration: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label={t.apps.airQuality} checked={w.showAirQuality} onChange={(v) => instantSaveWeather({ showAirQuality: v })} />
          <Toggle label={t.apps.autoColor} checked={w.aqiColor === 0} onChange={(v) => instantSaveWeather({ aqiColor: v ? 0 : 0xFFFFFF })} />
          {w.aqiColor !== 0 && <ColorField label="" value={w.aqiColor} onChange={(v) => autoSaveWeather({ aqiColor: v })} />}
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={w.aqiDuration || 7} onChange={(v) => autoSaveWeather({ aqiDuration: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label={t.apps.uvIndex} checked={w.showUV} onChange={(v) => instantSaveWeather({ showUV: v })} />
          <Toggle label={t.apps.autoColor} checked={w.uvColor === 0} onChange={(v) => instantSaveWeather({ uvColor: v ? 0 : 0xFFFFFF })} />
          {w.uvColor !== 0 && <ColorField label="" value={w.uvColor} onChange={(v) => autoSaveWeather({ uvColor: v })} />}
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={w.uvDuration || 7} onChange={(v) => autoSaveWeather({ uvDuration: v })} unit="s" />
          </div>
        </div>

        <p class={styles.hint}>{t.apps.autoColorHint}</p>
      </div>
    </Card>
  );
}
