import { useState } from "preact/hooks";
import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { resetSettings, reboot, eraseWifi } from "../../api/client";
import { toast } from "../../components/Toast";
import { Button, Card } from "../../components/ui";
import { useT } from "../../i18n";
import styles from "../settings/Settings.module.css";
import sectionStyles from "../settings/sections/sections.module.css";

function SystemContent() {
  const { settings, saveDisplaySettings, saveInfraConfig, saveWeatherConfig, loading } = useSettings();
  const [saving, setSaving] = useState(false);
  const t = useT();

  if (loading) return <p>{t.loading}</p>;
  if (!settings) return <p>{t.loading}</p>;

  async function handleSaveAll() {
    if (!settings) return;
    setSaving(true);
    await Promise.all([
      saveDisplaySettings(settings, ""),
      saveInfraConfig(""),
      saveWeatherConfig(""),
    ]);
    setSaving(false);
    toast(t.system.allSaved);
  }

  return (
    <div class={styles.page}>
      <Card title={t.system.title}>
        <div class={sectionStyles.stack}>
          <p class={sectionStyles.hint}>
            {t.system.hint}
          </p>
          <div class={sectionStyles.actions}>
            <Button variant="primary" onClick={handleSaveAll} loading={saving}>
              {t.system.saveAll}
            </Button>
            <Button
              onClick={() => {
                if (confirm(t.system.confirmReset)) {
                  resetSettings().then(() => {
                    toast(t.system.settingsReset);
                  });
                }
              }}
            >
              {t.system.resetDefaults}
            </Button>
            <Button
              variant="danger"
              onClick={() => {
                if (confirm(t.system.confirmEraseWifi)) {
                  eraseWifi().then(() => {
                    toast(t.system.wifiErased);
                  });
                }
              }}
            >
              {t.system.eraseWifi}
            </Button>
            <Button
              variant="danger"
              onClick={() => {
                if (confirm(t.system.confirmReboot)) {
                  reboot().then(() => toast(t.system.rebooting));
                }
              }}
            >
              {t.system.reboot}
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
