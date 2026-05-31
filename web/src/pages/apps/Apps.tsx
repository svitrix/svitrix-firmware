import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { AppOrderSection, AppsSection, WeatherAppsSection, AppTransitionsSection } from "../settings/sections";
import { useT } from "../../i18n";
import styles from "../settings/Settings.module.css";

function AppsContent() {
  const { settings, loading } = useSettings();
  const t = useT();

  if (loading) return <p>{t.loading}</p>;
  if (!settings) return <p>{t.loading}</p>;

  return (
    <div class={styles.page}>
      <AppOrderSection />
      <AppsSection />
      <WeatherAppsSection />
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
