import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, TextField, ColorField, Select, Card, FormRow, Button } from "../../../components/ui";
import styles from "./sections.module.css";

export function TimeDateSection() {
  const { t } = useTranslation();
  const { settings, updateSettings, saveDisplaySettings } = useSettings();
  const [saving, setSaving] = useState(false);
  if (!settings) return null;
  const s = settings;

  async function handleSave() {
    setSaving(true);
    await saveDisplaySettings({
      TFORMAT: s.TFORMAT, DFORMAT: s.DFORMAT, TMODE: s.TMODE,
      SOM: s.SOM, CEL: s.CEL,
      TIME_COL: s.TIME_COL, DATE_COL: s.DATE_COL,
      WD: s.WD, WDCA: s.WDCA, WDCI: s.WDCI,
      CHCOL: s.CHCOL, CTCOL: s.CTCOL, CBCOL: s.CBCOL,
    });
    setSaving(false);
  }

  return (
    <Card title={t("settingsDisplay.timeDate.title")}>
      <div class={styles.stack}>
        <FormRow>
          <TextField label={t("settingsDisplay.timeDate.timeFormat")} value={s.TFORMAT} onChange={(v) => updateSettings({ TFORMAT: v })} />
          <TextField label={t("settingsDisplay.timeDate.dateFormat")} value={s.DFORMAT} onChange={(v) => updateSettings({ DFORMAT: v })} />
        </FormRow>
        <Select
          label={t("settingsDisplay.timeDate.timeMode")}
          value={s.TMODE}
          options={[
            { value: 0, label: t("settingsDisplay.timeDate.modePlainText") },
            { value: 1, label: t("settingsDisplay.timeDate.modeCalendar") },
            { value: 2, label: t("settingsDisplay.timeDate.modeCalendarTop") },
            { value: 3, label: t("settingsDisplay.timeDate.modeCalendarAlt") },
            { value: 4, label: t("settingsDisplay.timeDate.modeCalendarAltTop") },
            { value: 5, label: t("settingsDisplay.timeDate.modeBigDigits") },
            { value: 6, label: t("settingsDisplay.timeDate.modeBinary") },
          ]}
          onChange={(v) => updateSettings({ TMODE: v as number })}
        />
        <Toggle label={t("settingsDisplay.timeDate.startOnMonday")} checked={s.SOM} onChange={(v) => updateSettings({ SOM: v })} />
        <Toggle label={t("settingsDisplay.timeDate.celsius")} checked={s.CEL} onChange={(v) => updateSettings({ CEL: v })} />
        <FormRow>
          <ColorField label={t("settingsDisplay.timeDate.timeColor")} value={s.TIME_COL} onChange={(v) => updateSettings({ TIME_COL: v })} />
          <ColorField label={t("settingsDisplay.timeDate.dateColor")} value={s.DATE_COL} onChange={(v) => updateSettings({ DATE_COL: v })} />
        </FormRow>
        <FormRow>
          <Toggle label={t("settingsDisplay.timeDate.showWeekday")} checked={s.WD} onChange={(v) => updateSettings({ WD: v })} />
        </FormRow>
        <FormRow>
          <ColorField label={t("settingsDisplay.timeDate.weekdayActive")} value={s.WDCA} onChange={(v) => updateSettings({ WDCA: v })} />
          <ColorField label={t("settingsDisplay.timeDate.weekdayInactive")} value={s.WDCI} onChange={(v) => updateSettings({ WDCI: v })} />
        </FormRow>
        <FormRow>
          <ColorField label={t("settingsDisplay.timeDate.calHeader")} value={s.CHCOL} onChange={(v) => updateSettings({ CHCOL: v })} />
          <ColorField label={t("settingsDisplay.timeDate.calText")} value={s.CTCOL} onChange={(v) => updateSettings({ CTCOL: v })} />
        </FormRow>
        <ColorField label={t("settingsDisplay.timeDate.calBody")} value={s.CBCOL} onChange={(v) => updateSettings({ CBCOL: v })} />
        <Button variant="primary" onClick={handleSave} loading={saving}>{t("settingsDisplay.timeDate.save")}</Button>
      </div>
    </Card>
  );
}
