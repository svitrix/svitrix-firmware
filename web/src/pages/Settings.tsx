import { SettingsProvider, useSettings } from "../context/SettingsContext";
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
} from "./settings-sections";

function SettingsContent() {
  const { settings, apiAvailable, loading } = useSettings();

  if (loading) return <p>Loading...</p>;

  if (!settings && !apiAvailable) {
    return (
      <div style={{ display: "flex", flexDirection: "column", gap: 16 }}>
        <WifiSection apMode />
      </div>
    );
  }

  if (!settings) return <p>Loading...</p>;

  return (
    <div style={{ display: "flex", flexDirection: "column", gap: 16 }}>
      <StatsBar />
      <WifiSection />
      <NetworkSection />
      <MqttSection />
      <NtpSection />
      <AuthSection />
      <InfraSaveButton />
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
