import { useState } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Button } from "../../../components/ui";
import { useT } from "../../../i18n";

export function InfraSaveButton() {
  const { config, saveInfraConfig } = useSettings();
  const [saving, setSaving] = useState(false);
  const t = useT();
  if (!config) return null;

  async function handleSave() {
    setSaving(true);
    await saveInfraConfig(t.settings.networkSaved);
    setSaving(false);
  }

  return (
    <Button variant="primary" onClick={handleSave} loading={saving}>
      {t.settings.saveButton}
    </Button>
  );
}
