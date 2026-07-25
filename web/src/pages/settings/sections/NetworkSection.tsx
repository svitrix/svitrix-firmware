import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, TextField, Card, FormRow } from "../../../components/ui";
import styles from "./sections.module.css";

/** Valid IPv4 dotted-quad, each octet 0–255. Empty string is treated as "unset". */
function isValidIp(v: string): boolean {
  const parts = v.split(".");
  if (parts.length !== 4) return false;
  return parts.every((p) => /^\d{1,3}$/.test(p) && Number(p) <= 255);
}

export function NetworkSection() {
  const { config, updateConfig } = useSettings();
  const { t } = useTranslation();
  if (!config) return null;

  const ipError = (v: string | undefined) =>
    v && !isValidIp(v) ? t("settings.network.invalidIp") : undefined;

  return (
    <Card title={t("settings.network.title")}>
      <div class={styles.stack}>
        <Toggle
          label={t("settings.network.staticIp")}
          checked={!!config["Static IP"]}
          onChange={(v) => updateConfig("Static IP", v)}
        />
        {config["Static IP"] && (
          <FormRow>
            <TextField label={t("settings.network.localIp")} value={config["Local IP"] || ""} onChange={(v) => updateConfig("Local IP", v)} error={ipError(config["Local IP"])} />
            <TextField label={t("settings.network.gateway")} value={config["Gateway"] || ""} onChange={(v) => updateConfig("Gateway", v)} error={ipError(config["Gateway"])} />
            <TextField label={t("settings.network.subnet")} value={config["Subnet"] || ""} onChange={(v) => updateConfig("Subnet", v)} error={ipError(config["Subnet"])} />
            <TextField label={t("settings.network.primaryDns")} value={config["Primary DNS"] || ""} onChange={(v) => updateConfig("Primary DNS", v)} error={ipError(config["Primary DNS"])} />
          </FormRow>
        )}
      </div>
    </Card>
  );
}
