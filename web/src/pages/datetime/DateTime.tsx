import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { TimeDateSection } from "../settings/sections";
import styles from "../settings/Settings.module.css";

function DateTimeContent() {
  const { settings, loading } = useSettings();

  if (loading) return <p>Loading...</p>;
  if (!settings) return <p>Loading...</p>;

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
