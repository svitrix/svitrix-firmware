import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { DisplaySection, NightModeSection, NotifySection } from "../settings/sections";
import { useT } from "../../i18n";
import styles from "../settings/Settings.module.css";

function DisplayContent() {
  const { settings, loading } = useSettings();
  const t = useT();

  if (loading) return <p>{t.loading}</p>;
  if (!settings) return <p>{t.loading}</p>;

  return (
    <div class={styles.page}>
      <DisplaySection />
      <NightModeSection />
      <NotifySection />
    </div>
  );
}

export function DisplayPage(_props: { path?: string }) {
  return (
    <SettingsProvider>
      <DisplayContent />
    </SettingsProvider>
  );
}
