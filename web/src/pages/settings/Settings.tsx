import { useTranslation } from "react-i18next";
import { SettingsProvider, useSettings } from "../../context/SettingsContext";
import { SettingsLayout } from "./SettingsLayout";
import { WifiSection } from "./sections";
import styles from "./Settings.module.css";

function SettingsContent() {
  const { settings, apiAvailable, loading } = useSettings();
  const { t } = useTranslation();

  if (loading) return <p>{t("common.loading")}</p>;

  if (!settings && !apiAvailable) {
    return (
      <div class={styles.page}>
        <header class={styles.welcome}>
          <h1 class={styles.welcomeTitle}>{t("deviceState.welcomeTitle")}</h1>
          <p class={styles.welcomeSubtitle}>{t("deviceState.welcomeSubtitle")}</p>
        </header>
        <WifiSection apMode />
      </div>
    );
  }

  if (!settings) return <p>{t("common.loading")}</p>;

  return <SettingsLayout />;
}

export function SettingsPage(_props: { path?: string }) {
  return (
    <SettingsProvider>
      <SettingsContent />
    </SettingsProvider>
  );
}
