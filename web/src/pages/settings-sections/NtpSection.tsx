import { useSettings } from "../../context/SettingsContext";
import { TextField, Card } from "../../components/ui";

export function NtpSection() {
  const { config, updateConfig } = useSettings();
  if (!config) return null;

  return (
    <Card title="NTP & Timezone">
      <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
        <TextField label="NTP Server" value={config["NTP Server"] || ""} onChange={(v) => updateConfig("NTP Server", v)} placeholder="pool.ntp.org" />
        <TextField label="Timezone" value={config["Timezone"] || ""} onChange={(v) => updateConfig("Timezone", v)} placeholder="CET-1CEST,M3.5.0,M10.5.0/3" />
        <p style={{ fontSize: 12, color: "var(--text-dim)" }}>
          Find your timezone at{" "}
          <a href="https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv" target="_blank" rel="noopener">
            posix_tz_db
          </a>
        </p>
      </div>
    </Card>
  );
}
