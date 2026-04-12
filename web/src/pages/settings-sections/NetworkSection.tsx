import { useSettings } from "../../context/SettingsContext";
import { Toggle, TextField, Card, FormRow } from "../../components/ui";
import styles from "./sections.module.css";

export function NetworkSection() {
  const { config, updateConfig } = useSettings();
  if (!config) return null;

  return (
    <Card title="Network">
      <div class={styles.stack}>
        <Toggle
          label="Static IP"
          checked={!!config["Static IP"]}
          onChange={(v) => updateConfig("Static IP", v)}
        />
        {config["Static IP"] && (
          <FormRow>
            <TextField label="Local IP" value={config["Local IP"] || ""} onChange={(v) => updateConfig("Local IP", v)} />
            <TextField label="Gateway" value={config["Gateway"] || ""} onChange={(v) => updateConfig("Gateway", v)} />
            <TextField label="Subnet" value={config["Subnet"] || ""} onChange={(v) => updateConfig("Subnet", v)} />
            <TextField label="Primary DNS" value={config["Primary DNS"] || ""} onChange={(v) => updateConfig("Primary DNS", v)} />
          </FormRow>
        )}
      </div>
    </Card>
  );
}
