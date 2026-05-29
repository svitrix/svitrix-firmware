import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { IconPickerSection } from "../settings/sections";
import styles from "../settings/Settings.module.css";

function IconsContent() {
  const { settings, loading } = useSettings();

  if (loading) return <p>Loading...</p>;
  if (!settings) return <p>Loading...</p>;

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
