import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { resetSettings, reboot } from "../../../api/client";
import { toast } from "../../../components/Toast";
import { Button } from "../../../components/ui";
import styles from "./sections.module.css";

export function ActionsBar() {
  const { t } = useTranslation();
  const { settings, saveDisplaySettings, reload } = useSettings();
  if (!settings) return null;

  return (
    <div class={styles.actions}>
      <Button variant="primary" onClick={async () => {
        await saveDisplaySettings(settings);
      }}>
        {t("settingsDisplay.actions.saveAll")}
      </Button>
      <Button onClick={() => {
        if (confirm(t("settingsDisplay.actions.resetConfirm"))) {
          resetSettings().then(() => { toast(t("settingsDisplay.actions.settingsReset")); reload(); });
        }
      }}>{t("settingsDisplay.actions.resetDefaults")}</Button>
      <Button variant="danger" onClick={() => {
        if (confirm(t("settingsDisplay.actions.rebootConfirm"))) reboot().then(() => toast(t("settingsDisplay.actions.rebooting")));
      }}>{t("settingsDisplay.actions.reboot")}</Button>
    </div>
  );
}
