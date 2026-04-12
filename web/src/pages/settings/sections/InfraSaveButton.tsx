import { useState } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Button } from "../../../components/ui";

export function InfraSaveButton() {
  const { config, saveInfraConfig } = useSettings();
  const [saving, setSaving] = useState(false);
  if (!config) return null;

  async function handleSave() {
    setSaving(true);
    await saveInfraConfig();
    setSaving(false);
  }

  return (
    <Button variant="primary" onClick={handleSave} loading={saving}>
      Save Network/MQTT/Time/Auth
    </Button>
  );
}
