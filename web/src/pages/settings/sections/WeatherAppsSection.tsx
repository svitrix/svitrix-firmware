import { useState } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Card, Button } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function WeatherAppsSection() {
  const { weatherConfig, settings, updateWeatherConfig, updateSettings, saveWeatherConfig } = useSettings();
  const [saving, setSaving] = useState(false);
  const t = useT();

  if (!weatherConfig || !settings) return null;
  const w = weatherConfig;

  async function handleSave() {
    setSaving(true);
    await saveWeatherConfig(t.apps.weatherAppsSaved);
    setSaving(false);
  }

  return (
    <Card title={t.apps.weatherApps}>
      <div class={styles.stack}>
        <div class={styles.appRow}>
          <Toggle label={t.apps.outdoorTemp} checked={w.showOutdoorTemp} onChange={(v) => updateWeatherConfig({ showOutdoorTemp: v })} />
          <ColorField label="" value={w.outdoorTempColor} onChange={(v) => updateWeatherConfig({ outdoorTempColor: v })} />
          <Toggle label={t.apps.celsius} checked={settings.CEL} onChange={(v) => updateSettings({ CEL: v })} />
          <div style={{ width: "515px", flexShrink: 0 }}>
            <Slider label="" min={1} max={60} value={w.outdoorTempDuration || 7} onChange={(v) => updateWeatherConfig({ outdoorTempDuration: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label={t.apps.outdoorHum} checked={w.showOutdoorHumidity} onChange={(v) => updateWeatherConfig({ showOutdoorHumidity: v })} />
          <ColorField label="" value={w.outdoorHumColor} onChange={(v) => updateWeatherConfig({ outdoorHumColor: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={w.outdoorHumDuration || 7} onChange={(v) => updateWeatherConfig({ outdoorHumDuration: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label={t.apps.pressure} checked={w.showPressure} onChange={(v) => updateWeatherConfig({ showPressure: v })} />
          <ColorField label="" value={w.pressureColor} onChange={(v) => updateWeatherConfig({ pressureColor: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={w.pressureDuration || 7} onChange={(v) => updateWeatherConfig({ pressureDuration: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label={t.apps.airQuality} checked={w.showAirQuality} onChange={(v) => updateWeatherConfig({ showAirQuality: v })} />
          <Toggle label={t.apps.autoColor} checked={w.aqiColor === 0} onChange={(v) => updateWeatherConfig({ aqiColor: v ? 0 : 0xFFFFFF })} />
          {w.aqiColor !== 0 && <ColorField label="" value={w.aqiColor} onChange={(v) => updateWeatherConfig({ aqiColor: v })} />}
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={w.aqiDuration || 7} onChange={(v) => updateWeatherConfig({ aqiDuration: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label={t.apps.uvIndex} checked={w.showUV} onChange={(v) => updateWeatherConfig({ showUV: v })} />
          <Toggle label={t.apps.autoColor} checked={w.uvColor === 0} onChange={(v) => updateWeatherConfig({ uvColor: v ? 0 : 0xFFFFFF })} />
          {w.uvColor !== 0 && <ColorField label="" value={w.uvColor} onChange={(v) => updateWeatherConfig({ uvColor: v })} />}
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={w.uvDuration || 7} onChange={(v) => updateWeatherConfig({ uvDuration: v })} unit="s" />
          </div>
        </div>

        <p class={styles.hint}>{t.apps.autoColorHint}</p>

        <Button variant="primary" onClick={handleSave} loading={saving}>
          {t.apps.saveWeatherApps}
        </Button>
      </div>
    </Card>
  );
}
