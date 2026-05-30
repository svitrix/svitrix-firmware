import { useState } from "preact/hooks";
import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { MqttSection, AuthSection } from "../settings/sections";
import { Button } from "../../components/ui";
import { useT } from "../../i18n";
import styles from "../settings/Settings.module.css";

function MqttContent() {
  const { settings, loading, saveInfraConfig } = useSettings();
  const [saving, setSaving] = useState(false);
  const t = useT();

  if (loading) return <p>{t.loading}</p>;
  if (!settings) return <p>{t.loading}</p>;

  async function handleSave() {
    setSaving(true);
    await saveInfraConfig(t.mqtt.mqttSaved);
    setSaving(false);
  }

  return (
    <div class={styles.page}>
      <MqttSection />
      <AuthSection />
      <Button variant="primary" onClick={handleSave} loading={saving}>
        {t.mqtt.saveButton}
      </Button>
    </div>
  );
}

export function MqttPage(_props: { path?: string }) {
  return (
    <SettingsProvider>
      <MqttContent />
    </SettingsProvider>
  );
}
