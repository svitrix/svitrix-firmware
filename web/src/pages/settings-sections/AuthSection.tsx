import { useSettings } from "../../context/SettingsContext";
import { TextField, Card, FormRow } from "../../components/ui";
import styles from "./sections.module.css";

export function AuthSection() {
  const { config, updateConfig } = useSettings();
  if (!config) return null;

  return (
    <Card title="Authentication">
      <FormRow>
        <TextField label="Username" value={config["Auth Username"] || ""} onChange={(v) => updateConfig("Auth Username", v)} />
        <TextField label="Password" value={config["Auth Password"] || ""} onChange={(v) => updateConfig("Auth Password", v)} type="password" />
      </FormRow>
      <p class={styles.hintMt}>
        Leave empty to disable HTTP authentication.
      </p>
    </Card>
  );
}
