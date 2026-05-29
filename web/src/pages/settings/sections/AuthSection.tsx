import { useSettings } from "../../../context/SettingsContext";
import { TextField, Card, FormRow } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function AuthSection() {
  const { config, updateConfig } = useSettings();
  const t = useT();
  if (!config) return null;

  return (
    <Card title={t.mqtt.auth}>
      <FormRow>
        <TextField label={t.mqtt.authUser} value={config["Auth Username"] || ""} onChange={(v) => updateConfig("Auth Username", v)} />
        <TextField label={t.mqtt.authPass} value={config["Auth Password"] || ""} onChange={(v) => updateConfig("Auth Password", v)} type="password" />
      </FormRow>
      <p class={styles.hintMt}>
        {t.mqtt.authHint}
      </p>
    </Card>
  );
}
