import { useSettings } from "../../context/SettingsContext";
import { TextField, Card, FormRow } from "../../components/ui";

export function AuthSection() {
  const { config, updateConfig } = useSettings();
  if (!config) return null;

  return (
    <Card title="Authentication">
      <FormRow>
        <TextField label="Username" value={config["Auth Username"] || ""} onChange={(v) => updateConfig("Auth Username", v)} />
        <TextField label="Password" value={config["Auth Password"] || ""} onChange={(v) => updateConfig("Auth Password", v)} type="password" />
      </FormRow>
      <p style={{ fontSize: 12, color: "var(--text-dim)", marginTop: 8 }}>
        Leave empty to disable HTTP authentication.
      </p>
    </Card>
  );
}
