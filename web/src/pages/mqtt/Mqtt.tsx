import { useState } from "preact/hooks";
import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { MqttSection, AuthSection } from "../settings/sections";
import { Button } from "../../components/ui";
import styles from "../settings/Settings.module.css";

function MqttContent() {
  const { settings, loading, saveInfraConfig } = useSettings();
  const [saving, setSaving] = useState(false);

  if (loading) return <p>Loading...</p>;
  if (!settings) return <p>Loading...</p>;

  async function handleSave() {
    setSaving(true);
    await saveInfraConfig();
    setSaving(false);
  }

  return (
    <div class={styles.page}>
      <MqttSection />
      <AuthSection />
      <Button variant="primary" onClick={handleSave} loading={saving}>
        Save MQTT/Auth
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
