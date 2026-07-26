import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Select, Card, FormRow } from "../../../components/ui";
import styles from "./sections.module.css";

export function AppsSection() {
  const { t } = useTranslation();
  const { settings, transitions, setSetting } = useSettings();
  if (!settings) return null;
  const s = settings;

  return (
    <Card title={t("settingsDisplay.apps.title")}>
      <div class={styles.stack}>
        <Toggle label={t("settingsDisplay.apps.time")} checked={s.TIM} onChange={(v) => setSetting({ TIM: v }, true)} />
        <Toggle label={t("settingsDisplay.apps.date")} checked={s.DAT} onChange={(v) => setSetting({ DAT: v }, true)} />
        <FormRow>
          <Toggle label={t("settingsDisplay.apps.temperature")} checked={s.TEMP} onChange={(v) => setSetting({ TEMP: v }, true)} />
          <ColorField label={t("settingsDisplay.apps.tempColor")} value={s.TEMP_COL} onChange={(v) => setSetting({ TEMP_COL: v }, true)} />
        </FormRow>
        <FormRow>
          <Toggle label={t("settingsDisplay.apps.humidity")} checked={s.HUM} onChange={(v) => setSetting({ HUM: v }, true)} />
          <ColorField label={t("settingsDisplay.apps.humColor")} value={s.HUM_COL} onChange={(v) => setSetting({ HUM_COL: v }, true)} />
        </FormRow>
        <FormRow>
          <Toggle label={t("settingsDisplay.apps.battery")} checked={s.BAT} onChange={(v) => setSetting({ BAT: v }, true)} />
          <ColorField label={t("settingsDisplay.apps.batteryColor")} value={s.BAT_COL} onChange={(v) => setSetting({ BAT_COL: v }, true)} />
        </FormRow>
        <Slider label={t("settingsDisplay.apps.appDuration")} min={1} max={60} value={s.ATIME} onChange={(v) => setSetting({ ATIME: v })} unit="s" />
        <Toggle label={t("settingsDisplay.apps.autoTransition")} checked={s.ATRANS} onChange={(v) => setSetting({ ATRANS: v }, true)} />
        {transitions.length > 0 && (
          <Select
            label={t("settingsDisplay.apps.transitionEffect")}
            value={s.TEFF}
            options={transitions.map((tr, i) => ({ value: i, label: tr }))}
            onChange={(v) => setSetting({ TEFF: v as number }, true)}
          />
        )}
        <Slider label={t("settingsDisplay.apps.transitionSpeed")} min={100} max={2000} step={100} value={s.TSPEED} onChange={(v) => setSetting({ TSPEED: v })} unit="ms" />
        <Slider label={t("settingsDisplay.apps.scrollSpeed")} helper={t("settingsDisplay.apps.scrollSpeedHelper")} min={10} max={100} value={s.SSPEED} onChange={(v) => setSetting({ SSPEED: v })} />
        <Toggle label={t("settingsDisplay.apps.blockNavigation")} helper={t("settingsDisplay.apps.blockNavigationHelper")} checked={s.BLOCKN} onChange={(v) => setSetting({ BLOCKN: v }, true)} />
      </div>
    </Card>
  );
}
