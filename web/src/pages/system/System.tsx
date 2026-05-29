import { useState } from "preact/hooks";
import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { resetSettings, reboot, eraseWifi } from "../../api/client";
import { toast } from "../../components/Toast";
import { Button, Card } from "../../components/ui";
import styles from "../settings/Settings.module.css";
import sectionStyles from "../settings/sections/sections.module.css";

function SystemContent() {
  const { settings, saveDisplaySettings, saveInfraConfig, saveWeatherConfig, loading } = useSettings();
  const [saving, setSaving] = useState(false);

  if (loading) return <p>Loading...</p>;
  if (!settings) return <p>Loading...</p>;

  async function handleSaveAll() {
    if (!settings) return;
    setSaving(true);
    await Promise.all([
      saveDisplaySettings(settings),
      saveInfraConfig(),
      saveWeatherConfig(),
    ]);
    setSaving(false);
    toast("All settings saved");
  }

  return (
    <div class={styles.page}>
      <Card title="System">
        <div class={sectionStyles.stack}>
          <p class={sectionStyles.hint}>
            Save, reset, or reboot the device.
          </p>
          <div class={sectionStyles.actions}>
            <Button variant="primary" onClick={handleSaveAll} loading={saving}>
              Save All Settings
            </Button>
            <Button
              onClick={() => {
                if (confirm("Reset all settings to defaults? Device will restart.")) {
                  resetSettings().then(() => {
                    toast("Settings reset, rebooting...");
                  });
                }
              }}
            >
              Reset Defaults
            </Button>
            <Button
              variant="danger"
              onClick={() => {
                if (confirm("Erase WiFi credentials? Device will restart and enter AP mode.")) {
                  eraseWifi().then(() => {
                    toast("WiFi erased, rebooting...");
                  });
                }
              }}
            >
              Erase WiFi
            </Button>
            <Button
              variant="danger"
              onClick={() => {
                if (confirm("Reboot device?")) {
                  reboot().then(() => toast("Rebooting..."));
                }
              }}
            >
              Reboot
            </Button>
          </div>
        </div>
      </Card>
    </div>
  );
}

export function SystemPage(_props: { path?: string }) {
  return (
    <SettingsProvider>
      <SystemContent />
    </SettingsProvider>
  );
}
