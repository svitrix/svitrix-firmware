import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { DisplaySection, NightModeSection, NotifySection } from "../settings/sections";
import styles from "../settings/Settings.module.css";

function DisplayContent() {
  const { settings, loading } = useSettings();

  if (loading) return <p>Loading...</p>;
  if (!settings) return <p>Loading...</p>;

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
