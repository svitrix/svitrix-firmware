import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Button } from "../../../components/ui";

export function InfraSaveButton() {
  const { config, saveInfraConfig } = useSettings();
  const { t } = useTranslation();
  const [saving, setSaving] = useState(false);
  if (!config) return null;

  async function handleSave() {
    setSaving(true);
    await saveInfraConfig();
    setSaving(false);
  }

  return (
    <Button variant="primary" onClick={handleSave} loading={saving}>
      {t("settings.infra.save")}
    </Button>
  );
}
