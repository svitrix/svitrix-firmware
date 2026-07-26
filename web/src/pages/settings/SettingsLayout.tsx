import type { ComponentChildren } from "preact";
import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { LivePreview } from "../../components/LivePreview";
import { SaveIndicator } from "../../components/SaveIndicator";
import { useSettings } from "../../context/SettingsContext";
import { OfflineBanner } from "./OfflineBanner";
import { SettingsNav, type CategoryId } from "./SettingsNav";
import styles from "./SettingsLayout.module.css";
import {
  WifiSection,
  NetworkSection,
  MqttSection,
  NtpSection,
  AuthSection,
  InfraSaveBar,
  DisplaySection,
  AppsSection,
  ClockFaceSection,
  SoundSection,
  IconPickerSection,
  ActionsBar,
  NotifySection,
  NightModeSection,
} from "./sections";

const CATEGORY_SECTIONS: Record<CategoryId, () => ComponentChildren> = {
  appearance: () => (
    <>
      <DisplaySection />
      <ClockFaceSection />
      <NightModeSection />
    </>
  ),
  screens: () => <AppsSection />,
  network: () => (
    <>
      <WifiSection />
      <MqttSection />
      <NtpSection />
      <NetworkSection />
      <InfraSaveBar />
    </>
  ),
  system: () => (
    <>
      <AuthSection />
      <SoundSection />
      <ActionsBar />
    </>
  ),
  tools: () => (
    <>
      <NotifySection />
      <IconPickerSection />
    </>
  ),
};

export function SettingsLayout() {
  const { t } = useTranslation();
  const { online, lastSeen, reload } = useSettings();
  const [active, setActive] = useState<CategoryId>("appearance");

  return (
    <div class={styles.layout}>
      {!online && <OfflineBanner lastSeen={lastSeen} onRetry={reload} />}
      <div class={styles.header}>
        <LivePreview variant="compact" />
        <SaveIndicator />
      </div>
      <div class={styles.body}>
        <SettingsNav active={active} onChange={setActive} />
        <section
          class={`${styles.panel} ${active === "tools" ? styles.tools : ""}`}
          role="tabpanel"
          id={`settings-panel-${active}`}
          aria-labelledby={`settings-tab-${active}`}
          tabIndex={0}
        >
          <header class={styles.panelHead}>
            <h2 class={styles.heading}>{t(`settingsNav.${active}.label`)}</h2>
            <p class={styles.subtitle}>{t(`settingsNav.${active}.description`)}</p>
          </header>
          {/* Native fieldset[disabled] switches off every descendant control while
              the device is unreachable — one place, no per-control threading. */}
          <fieldset class={styles.fieldset} disabled={!online}>
            <div class={styles.sections}>{CATEGORY_SECTIONS[active]()}</div>
          </fieldset>
        </section>
      </div>
    </div>
  );
}
