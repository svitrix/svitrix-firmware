import { useSettings } from "../../context/SettingsContext";
import { resetSettings, reboot } from "../../api/client";
import { toast } from "../../components/Toast";
import { Button } from "../../components/ui";
import styles from "./sections.module.css";

export function ActionsBar() {
  const { settings, saveDisplaySettings, reload } = useSettings();
  if (!settings) return null;

  return (
    <div class={styles.actions}>
      <Button variant="primary" onClick={async () => {
        await saveDisplaySettings(settings);
      }}>
        Save All Display Settings
      </Button>
      <Button onClick={() => {
        if (confirm("Reset all settings to defaults?")) {
          resetSettings().then(() => { toast("Settings reset"); reload(); });
        }
      }}>Reset Defaults</Button>
      <Button variant="danger" onClick={() => {
        if (confirm("Reboot device?")) reboot().then(() => toast("Rebooting..."));
      }}>Reboot</Button>
    </div>
  );
}
