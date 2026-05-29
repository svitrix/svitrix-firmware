import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import styles from "./Settings.module.css";
import {
  StatsBar,
  WifiSection,
  NetworkSection,
  NtpSection,
  InfraSaveButton,
  WeatherApiSection,
} from "./sections";

function SettingsContent() {
  const { settings, apiAvailable, loading } = useSettings();

  if (loading) return <p>Loading...</p>;

  if (!settings && !apiAvailable) {
    return (
      <div class={styles.page}>
        <WifiSection />
      </div>
    );
  }

  if (!settings) return <p>Loading...</p>;

  return (
    <div class={styles.page}>
      <StatsBar />
      <WifiSection />
      <NetworkSection />
      <NtpSection />
      <InfraSaveButton />
      <WeatherApiSection />
    </div>
  );
}

export function SettingsPage(_props: { path?: string }) {
  return (
    <SettingsProvider>
      <SettingsContent />
    </SettingsProvider>
  );
}
