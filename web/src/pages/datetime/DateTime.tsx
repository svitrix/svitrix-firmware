import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { TimeDateSection } from "../settings/sections";
import { useT } from "../../i18n";
import styles from "../settings/Settings.module.css";

function DateTimeContent() {
  const { settings, loading } = useSettings();
  const t = useT();

  if (loading) return <p>{t.loading}</p>;
  if (!settings) return <p>{t.loading}</p>;

  return (
    <div class={styles.page}>
      <TimeDateSection />
    </div>
  );
}

export function DateTimePage(_props: { path?: string }) {
  return (
    <SettingsProvider>
      <DateTimeContent />
    </SettingsProvider>
  );
}
