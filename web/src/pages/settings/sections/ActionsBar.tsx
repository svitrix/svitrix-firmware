import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { resetSettings, reboot } from "../../../api/client";
import { toast } from "../../../components/Toast";
import { Button, ConfirmDialog } from "../../../components/ui";
import styles from "./sections.module.css";

export function ActionsBar() {
  const { t } = useTranslation();
  const { settings, saveDisplaySettings, reload } = useSettings();
  const [confirmReset, setConfirmReset] = useState(false);
  const [confirmReboot, setConfirmReboot] = useState(false);
  if (!settings) return null;

  return (
    <div class={styles.actions}>
      <Button variant="primary" onClick={async () => {
        await saveDisplaySettings(settings);
      }}>
        {t("settingsDisplay.actions.saveAll")}
      </Button>
      <Button onClick={() => setConfirmReset(true)}>
        {t("settingsDisplay.actions.resetDefaults")}
      </Button>
      <Button variant="danger" onClick={() => setConfirmReboot(true)}>
        {t("settingsDisplay.actions.reboot")}
      </Button>

      <ConfirmDialog
        open={confirmReset}
        title={t("settingsDisplay.actions.resetTitle")}
        body={t("settingsDisplay.actions.resetConfirm")}
        confirmLabel={t("settingsDisplay.actions.resetDefaults")}
        danger
        onConfirm={() => {
          setConfirmReset(false);
          resetSettings().then(() => { toast(t("settingsDisplay.actions.settingsReset")); reload(); });
        }}
        onCancel={() => setConfirmReset(false)}
      />

      <ConfirmDialog
        open={confirmReboot}
        title={t("settingsDisplay.actions.rebootTitle")}
        body={t("settingsDisplay.actions.rebootConfirm")}
        confirmLabel={t("settingsDisplay.actions.reboot")}
        danger
        onConfirm={() => {
          setConfirmReboot(false);
          reboot().then(() => toast(t("settingsDisplay.actions.rebooting")));
        }}
        onCancel={() => setConfirmReboot(false)}
      />
    </div>
  );
}
