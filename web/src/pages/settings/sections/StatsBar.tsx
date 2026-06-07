import { useSettings } from "../../../context/SettingsContext";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

function formatUptime(seconds: number): string {
  const d = Math.floor(seconds / 86400);
  const h = Math.floor((seconds % 86400) / 3600);
  const m = Math.floor((seconds % 3600) / 60);
  const s = seconds % 60;
  const parts: string[] = [];
  if (d > 0) parts.push(`${d}d`);
  if (h > 0) parts.push(`${h}h`);
  if (m > 0) parts.push(`${m}m`);
  if (s > 0 || parts.length === 0) parts.push(`${s}s`);
  return parts.join(" ");
}

export function StatsBar() {
  const { stats } = useSettings();
  const t = useT();
  if (!stats) return null;

  const uptimeSeconds = parseInt(stats.uptime, 10) || 0;

  return (
    <div class={`card ${styles.statsBar}`}>
      <span>{t.stats.firmware}: {stats.version}</span>
      <span>{t.stats.ram}: {(stats.ram / 1024).toFixed(0)} / {(stats.ram_total / 1024).toFixed(0)} KB</span>
      <span>{t.stats.wifi}: {stats.wifi_signal} dBm</span>
      <span>{t.stats.lux}: {stats.lux}</span>
      <span>{t.stats.uptime}: {formatUptime(uptimeSeconds)}</span>
      {stats.temp > 0 && <span>{t.stats.temp}: {stats.temp}&deg;</span>}
      {stats.hum > 0 && <span>{t.stats.hum}: {stats.hum}%</span>}
      <span>{t.stats.bri}: {stats.bri}</span>
    </div>
  );
}
