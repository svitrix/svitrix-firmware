import { useSettings } from "../../../context/SettingsContext";
import { TextField, Card, Select } from "../../../components/ui";
import { useT } from "../../../i18n";
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
  const t = useT();
  if (!config) return null;

  const currentServer = config["NTP Server"] || "time.cloudflare.com";
  const isCustom = !NTP_SERVERS.some((s) => s.value === currentServer);

  return (
    <Card title={t.settings.ntpTimezone}>
      <div class={styles.stack}>
        <Select
          label={t.settings.ntpServer}
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
            label={t.settings.customNtpServer}
            value={currentServer}
            onChange={(v) => updateConfig("NTP Server", v)}
          />
        )}
        <TextField
          label={t.settings.timezone}
          value={config["Timezone"] || ""}
          onChange={(v) => updateConfig("Timezone", v)}
          placeholder="CST6"
        />
        <p class={styles.hint}>
          {t.settings.timezoneHint}{" "}
          <a href="https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv" target="_blank" rel="noopener">
            posix_tz_db
          </a>
        </p>
      </div>
    </Card>
  );
}
