import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import styles from "./Settings.module.css";
import {
  StatsBar,
  WifiSection,
  NetworkSection,
  MqttSection,
  NtpSection,
  AuthSection,
  InfraSaveButton,
  DisplaySection,
  AppsSection,
  TimeDateSection,
  SoundSection,
  IconPickerSection,
  ActionsBar,
  NotifySection,
  NightModeSection,
} from "./sections";

function SettingsContent() {
  const { settings, apiAvailable, loading } = useSettings();

  if (loading) return <p>Loading...</p>;

  if (!settings && !apiAvailable) {
    return (
      <div class={styles.page}>
        <WifiSection apMode />
      </div>
    );
  }

  if (!settings) return <p>Loading...</p>;

  return (
    <div class={styles.page}>
      <StatsBar />
      <WifiSection />
      <NetworkSection />
      <MqttSection />
      <NtpSection />
      <AuthSection />
      <InfraSaveButton />
      <NightModeSection />
      <DisplaySection />
      <AppsSection />
      <TimeDateSection />
      <SoundSection />
      <NotifySection />
      <IconPickerSection />
      <ActionsBar />
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
