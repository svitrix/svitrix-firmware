import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { resetSettings, reboot } from "../../../api/client";
import { Button, ConfirmDialog } from "../../../components/ui";
import { RebootOverlay } from "../../../components/RebootOverlay";
import styles from "./sections.module.css";

export function ActionsBar() {
  const { t } = useTranslation();
  const { settings, reload } = useSettings();
  const [confirmReset, setConfirmReset] = useState(false);
  const [confirmReboot, setConfirmReboot] = useState(false);
  const [rebooting, setRebooting] = useState(false);
  if (!settings) return null;

  return (
    <div class={styles.actions}>
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
          // Fire the action then poll via the overlay — the old
          // resetSettings().then(reload) fetched into a rebooting device.
          resetSettings().catch(() => {});
          setRebooting(true);
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
          reboot().catch(() => {});
          setRebooting(true);
        }}
        onCancel={() => setConfirmReboot(false)}
      />

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
