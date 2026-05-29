import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { IconPickerSection } from "../settings/sections";
import { useT } from "../../i18n";
import styles from "../settings/Settings.module.css";

function IconsContent() {
  const { settings, loading } = useSettings();
  const t = useT();

  if (loading) return <p>{t.loading}</p>;
  if (!settings) return <p>{t.loading}</p>;

  return (
    <div class={styles.page}>
      <IconPickerSection />
    </div>
  );
}

export function IconsPage(_props: { path?: string }) {
  return (
    <SettingsProvider>
      <IconsContent />
    </SettingsProvider>
  );
}
