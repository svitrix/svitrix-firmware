import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { AppTransitionsSection } from "../settings/sections";
import { UnifiedRotationSection } from "./components/UnifiedRotationSection";
import { useT } from "../../i18n";
import styles from "../settings/Settings.module.css";

function AppsContent() {
  const { settings, loading } = useSettings();
  const t = useT();

  if (loading) return <p>{t.loading}</p>;
  if (!settings) return <p>{t.loading}</p>;

  return (
    <div class={styles.page}>
      <UnifiedRotationSection />
      <AppTransitionsSection />
    </div>
  );
}

export function AppsPage(_props: { path?: string }) {
  return (
    <SettingsProvider>
      <AppsContent />
    </SettingsProvider>
  );
}
