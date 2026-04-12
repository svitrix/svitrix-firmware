import { useSettings } from "../../../context/SettingsContext";
import { TextField, Card } from "../../../components/ui";
import styles from "./sections.module.css";

export function NtpSection() {
  const { config, updateConfig } = useSettings();
  if (!config) return null;

  return (
    <Card title="NTP & Timezone">
      <div class={styles.stack}>
        <TextField label="NTP Server" value={config["NTP Server"] || ""} onChange={(v) => updateConfig("NTP Server", v)} placeholder="pool.ntp.org" />
        <TextField label="Timezone" value={config["Timezone"] || ""} onChange={(v) => updateConfig("Timezone", v)} placeholder="CET-1CEST,M3.5.0,M10.5.0/3" />
        <p class={styles.hint}>
          Find your timezone at{" "}
          <a href="https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv" target="_blank" rel="noopener">
            posix_tz_db
          </a>
        </p>
      </div>
    </Card>
  );
}
