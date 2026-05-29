import { useSettings } from "../../../context/SettingsContext";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function StatsBar() {
  const { stats } = useSettings();
  const t = useT();
  if (!stats) return null;

  return (
    <div class={`card ${styles.statsBar}`}>
      <span>{t.stats.firmware}: {stats.version}</span>
      <span>{t.stats.ram}: {(stats.ram / 1024).toFixed(0)} / {(stats.ram_total / 1024).toFixed(0)} KB</span>
      <span>{t.stats.wifi}: {stats.wifi_signal} dBm</span>
      <span>{t.stats.lux}: {stats.lux}</span>
      <span>{t.stats.uptime}: {stats.uptime}</span>
      {stats.temp > 0 && <span>{t.stats.temp}: {stats.temp}&deg;</span>}
      {stats.hum > 0 && <span>{t.stats.hum}: {stats.hum}%</span>}
      <span>{t.stats.bri}: {stats.bri}</span>
    </div>
  );
}
