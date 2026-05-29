import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { AppsSection, WeatherAppsSection, AppTransitionsSection } from "../settings/sections";
import styles from "../settings/Settings.module.css";

function AppsContent() {
  const { settings, loading } = useSettings();

  if (loading) return <p>Loading...</p>;
  if (!settings) return <p>Loading...</p>;

  return (
    <div class={styles.page}>
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
