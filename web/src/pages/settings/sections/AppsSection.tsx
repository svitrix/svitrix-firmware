import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Select, Card, FormRow, Button } from "../../../components/ui";
import styles from "./sections.module.css";

export function AppsSection() {
  const { t } = useTranslation();
  const { settings, transitions, updateSettings, saveDisplaySettings } = useSettings();
  const [saving, setSaving] = useState(false);
  if (!settings) return null;
  const s = settings;

  async function handleSave() {
    setSaving(true);
    await saveDisplaySettings({
      TIM: s.TIM, DAT: s.DAT,
      TEMP: s.TEMP, TEMP_COL: s.TEMP_COL,
      HUM: s.HUM, HUM_COL: s.HUM_COL,
      BAT: s.BAT, BAT_COL: s.BAT_COL,
      ATIME: s.ATIME, ATRANS: s.ATRANS,
      TEFF: s.TEFF, TSPEED: s.TSPEED,
      SSPEED: s.SSPEED, BLOCKN: s.BLOCKN,
    });
    setSaving(false);
  }

  return (
    <Card title={t("settingsDisplay.apps.title")}>
      <div class={styles.stack}>
        <Toggle label={t("settingsDisplay.apps.time")} checked={s.TIM} onChange={(v) => updateSettings({ TIM: v })} />
        <Toggle label={t("settingsDisplay.apps.date")} checked={s.DAT} onChange={(v) => updateSettings({ DAT: v })} />
        <FormRow>
          <Toggle label={t("settingsDisplay.apps.temperature")} checked={s.TEMP} onChange={(v) => updateSettings({ TEMP: v })} />
          <ColorField label={t("settingsDisplay.apps.tempColor")} value={s.TEMP_COL} onChange={(v) => updateSettings({ TEMP_COL: v })} />
        </FormRow>
        <FormRow>
          <Toggle label={t("settingsDisplay.apps.humidity")} checked={s.HUM} onChange={(v) => updateSettings({ HUM: v })} />
          <ColorField label={t("settingsDisplay.apps.humColor")} value={s.HUM_COL} onChange={(v) => updateSettings({ HUM_COL: v })} />
        </FormRow>
        <FormRow>
          <Toggle label={t("settingsDisplay.apps.battery")} checked={s.BAT} onChange={(v) => updateSettings({ BAT: v })} />
          <ColorField label={t("settingsDisplay.apps.batteryColor")} value={s.BAT_COL} onChange={(v) => updateSettings({ BAT_COL: v })} />
        </FormRow>
        <Slider label={t("settingsDisplay.apps.appDuration")} min={1} max={60} value={s.ATIME} onChange={(v) => updateSettings({ ATIME: v })} unit="s" />
        <Toggle label={t("settingsDisplay.apps.autoTransition")} checked={s.ATRANS} onChange={(v) => updateSettings({ ATRANS: v })} />
        {transitions.length > 0 && (
          <Select
            label={t("settingsDisplay.apps.transitionEffect")}
            value={s.TEFF}
            options={transitions.map((tr, i) => ({ value: i, label: tr }))}
            onChange={(v) => updateSettings({ TEFF: v as number })}
          />
        )}
        <Slider label={t("settingsDisplay.apps.transitionSpeed")} min={100} max={2000} step={100} value={s.TSPEED} onChange={(v) => updateSettings({ TSPEED: v })} unit="ms" />
        <Slider label={t("settingsDisplay.apps.scrollSpeed")} min={10} max={100} value={s.SSPEED} onChange={(v) => updateSettings({ SSPEED: v })} />
        <Toggle label={t("settingsDisplay.apps.blockNavigation")} checked={s.BLOCKN} onChange={(v) => updateSettings({ BLOCKN: v })} />
        <Button variant="primary" onClick={handleSave} loading={saving}>{t("settingsDisplay.apps.save")}</Button>
      </div>
    </Card>
  );
}
