import { useSettings } from "../../../context/SettingsContext";
import { Toggle, TextField, Card, FormRow } from "../../../components/ui";
import styles from "./sections.module.css";

export function MqttSection() {
  const { config, updateConfig } = useSettings();
  if (!config) return null;

  return (
    <Card title="MQTT">
      <div class={styles.stack}>
        <FormRow>
          <TextField label="Broker" value={config["Broker"] || ""} onChange={(v) => updateConfig("Broker", v)} />
          <TextField label="Port" value={config["Port"] || 1883} onChange={(v) => updateConfig("Port", parseInt(v as string) || 1883)} type="number" />
        </FormRow>
        <FormRow>
          <TextField label="Username" value={config["Username"] || ""} onChange={(v) => updateConfig("Username", v)} />
          <TextField label="Password" value={config["Password"] || ""} onChange={(v) => updateConfig("Password", v)} type="password" />
        </FormRow>
        <TextField label="Prefix" value={config["Prefix"] || ""} onChange={(v) => updateConfig("Prefix", v)} placeholder="svitrix" />
        <Toggle
          label="Home Assistant Discovery"
          checked={!!config["Homeassistant Discovery"]}
          onChange={(v) => updateConfig("Homeassistant Discovery", v)}
        />
      </div>
    </Card>
  );
}
