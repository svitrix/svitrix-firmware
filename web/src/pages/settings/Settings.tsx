import { useTranslation } from "react-i18next";
import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import styles from "./Settings.module.css";
import {
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
  const { t } = useTranslation();

  if (loading) return <p>{t("common.loading")}</p>;

  if (!settings && !apiAvailable) {
    return (
      <div class={styles.page}>
        <WifiSection apMode />
      </div>
    );
  }

  if (!settings) return <p>{t("common.loading")}</p>;

  return (
    <div class={styles.page}>
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
