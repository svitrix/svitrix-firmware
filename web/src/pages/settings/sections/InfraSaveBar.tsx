import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Button } from "../../../components/ui";
import { RebootOverlay } from "../../../components/RebootOverlay";
import styles from "./sections.module.css";

export function InfraSaveBar() {
  const { config, saveInfraConfig, reload } = useSettings();
  const { t } = useTranslation();
  const [saving, setSaving] = useState(false);
  const [rebooting, setRebooting] = useState(false);
  if (!config) return null;

  async function handleSave() {
    setSaving(true);
    try {
      await saveInfraConfig();
      // Infra /save restarts networking — poll until the device returns.
      setRebooting(true);
    } catch {
      // save failed (saveInfraConfig already toasted) — don't open the reboot overlay
    } finally {
      setSaving(false);
    }
  }

  return (
    <div class={styles.infraBar}>
      <span class={styles.restartBadge}>{t("settings.infra.requiresRestart")}</span>
      <p class={styles.infraHint}>{t("settings.infra.restartHint")}</p>
      <Button variant="primary" onClick={handleSave} loading={saving}>
        {t("settings.infra.save")}
      </Button>

      <RebootOverlay
        open={rebooting}
        onBack={() => {
          setRebooting(false);
          reload();
        }}
      />
    </div>
  );
}
