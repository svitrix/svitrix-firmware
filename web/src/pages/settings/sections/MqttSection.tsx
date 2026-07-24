import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, TextField, Card, FormRow } from "../../../components/ui";
import styles from "./sections.module.css";

export function MqttSection() {
  const { config, updateConfig } = useSettings();
  const { t } = useTranslation();
  if (!config) return null;

  return (
    <Card title={t("settings.mqtt.title")}>
      <div class={styles.stack}>
        <FormRow>
          <TextField label={t("settings.mqtt.broker")} value={config["Broker"] || ""} onChange={(v) => updateConfig("Broker", v)} />
          <TextField label={t("settings.mqtt.port")} value={config["Port"] || 1883} onChange={(v) => updateConfig("Port", parseInt(v as string) || 1883)} type="number" />
        </FormRow>
        <FormRow>
          <TextField label={t("settings.mqtt.username")} value={config["Username"] || ""} onChange={(v) => updateConfig("Username", v)} />
          <TextField label={t("settings.mqtt.password")} value={config["Password"] || ""} onChange={(v) => updateConfig("Password", v)} type="password" />
        </FormRow>
        <TextField label={t("settings.mqtt.prefix")} value={config["Prefix"] || ""} onChange={(v) => updateConfig("Prefix", v)} placeholder="svitrix" />
        <Toggle
          label={t("settings.mqtt.haDiscovery")}
          checked={!!config["Homeassistant Discovery"]}
          onChange={(v) => updateConfig("Homeassistant Discovery", v)}
        />
      </div>
    </Card>
  );
}
