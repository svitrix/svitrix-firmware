import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, TextField, Card, FormRow } from "../../../components/ui";
import styles from "./sections.module.css";

export function NetworkSection() {
  const { config, updateConfig } = useSettings();
  const { t } = useTranslation();
  if (!config) return null;

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
            <TextField label={t("settings.network.localIp")} value={config["Local IP"] || ""} onChange={(v) => updateConfig("Local IP", v)} />
            <TextField label={t("settings.network.gateway")} value={config["Gateway"] || ""} onChange={(v) => updateConfig("Gateway", v)} />
            <TextField label={t("settings.network.subnet")} value={config["Subnet"] || ""} onChange={(v) => updateConfig("Subnet", v)} />
            <TextField label={t("settings.network.primaryDns")} value={config["Primary DNS"] || ""} onChange={(v) => updateConfig("Primary DNS", v)} />
          </FormRow>
        )}
      </div>
    </Card>
  );
}
