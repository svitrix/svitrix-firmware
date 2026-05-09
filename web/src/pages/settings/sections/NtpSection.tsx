import { useSettings } from "../../../context/SettingsContext";
import { TextField, Card, Select } from "../../../components/ui";
import styles from "./sections.module.css";

const NTP_SERVERS = [
  { value: "time.cloudflare.com", label: "Cloudflare" },
  { value: "time.google.com", label: "Google" },
  { value: "pool.ntp.org", label: "Pool NTP" },
  { value: "time.apple.com", label: "Apple" },
  { value: "time.nist.gov", label: "NIST (US)" },
];

export function NtpSection() {
  const { config, updateConfig } = useSettings();
  if (!config) return null;

  const currentServer = config["NTP Server"] || "pool.ntp.org";
  const isCustom = !NTP_SERVERS.some((s) => s.value === currentServer);

  return (
    <Card title="NTP & Timezone">
      <div class={styles.stack}>
        <Select
          label="NTP Server"
          value={isCustom ? "custom" : currentServer}
          options={[
            ...NTP_SERVERS,
            ...(isCustom ? [{ value: "custom", label: `Custom: ${currentServer}` }] : []),
          ]}
          onChange={(v) => {
            if (v !== "custom") updateConfig("NTP Server", v as string);
          }}
        />
        {isCustom && (
          <TextField
            label="Custom NTP Server"
            value={currentServer}
            onChange={(v) => updateConfig("NTP Server", v)}
          />
        )}
        <TextField
          label="Timezone"
          value={config["Timezone"] || ""}
          onChange={(v) => updateConfig("Timezone", v)}
          placeholder="CST6"
        />
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
