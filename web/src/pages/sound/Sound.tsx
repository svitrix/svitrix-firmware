import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { SoundSection, MelodiesSection } from "../settings/sections";
import { useT } from "../../i18n";
import styles from "../settings/Settings.module.css";

function SoundContent() {
  const { settings, loading } = useSettings();
  const t = useT();

  if (loading) return <p>{t.loading}</p>;
  if (!settings) return <p>{t.loading}</p>;

  return (
    <div class={styles.page}>
      <SoundSection />
      <MelodiesSection />
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
