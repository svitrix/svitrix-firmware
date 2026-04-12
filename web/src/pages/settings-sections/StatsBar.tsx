import { useSettings } from "../../context/SettingsContext";

export function StatsBar() {
  const { stats } = useSettings();
  if (!stats) return null;

  return (
    <div class="card" style={{ display: "flex", gap: 16, flexWrap: "wrap", fontSize: 13 }}>
      <span>FW: {stats.version}</span>
      <span>RAM: {(stats.ram / 1024).toFixed(0)} KB</span>
      <span>WiFi: {stats.wifi_signal} dBm</span>
      <span>Lux: {stats.lux}</span>
      <span>Uptime: {stats.uptime}</span>
      {stats.temp > 0 && <span>Temp: {stats.temp}&deg;</span>}
      {stats.hum > 0 && <span>Hum: {stats.hum}%</span>}
      <span>Bri: {stats.bri}</span>
    </div>
  );
}
