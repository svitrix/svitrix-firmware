import { useSettings } from "../../../context/SettingsContext";
import { Toggle, TextField, Card, FormRow } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function NetworkSection() {
  const { config, updateConfig } = useSettings();
  const t = useT();
  if (!config) return null;

  return (
    <Card title={t.settings.network}>
      <div class={styles.stack}>
        <Toggle
          label={t.settings.staticIp}
          checked={!!config["Static IP"]}
          onChange={(v) => updateConfig("Static IP", v)}
        />
        {config["Static IP"] && (
          <FormRow>
            <TextField label={t.settings.localIp} value={config["Local IP"] || ""} onChange={(v) => updateConfig("Local IP", v)} />
            <TextField label={t.settings.gateway} value={config["Gateway"] || ""} onChange={(v) => updateConfig("Gateway", v)} />
            <TextField label={t.settings.subnet} value={config["Subnet"] || ""} onChange={(v) => updateConfig("Subnet", v)} />
            <TextField label={t.settings.primaryDns} value={config["Primary DNS"] || ""} onChange={(v) => updateConfig("Primary DNS", v)} />
          </FormRow>
        )}
      </div>
    </Card>
  );
}
