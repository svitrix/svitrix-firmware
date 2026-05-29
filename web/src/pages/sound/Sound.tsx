import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { SoundSection } from "../settings/sections";
import styles from "../settings/Settings.module.css";

function SoundContent() {
  const { settings, loading } = useSettings();

  if (loading) return <p>Loading...</p>;
  if (!settings) return <p>Loading...</p>;

  return (
    <div class={styles.page}>
      <SoundSection />
    </div>
  );
}

export function SoundPage(_props: { path?: string }) {
  return (
    <SettingsProvider>
      <SoundContent />
    </SettingsProvider>
  );
}
