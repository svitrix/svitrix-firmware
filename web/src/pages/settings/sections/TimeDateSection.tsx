import { useState, useMemo } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, ColorField, Select, Card, FormRow, Button } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

const TIME_FORMATS = [
  { value: "%H %M", label: "24H HH MM (blink)" },
  { value: "%H:%M", label: "24H HH:MM" },
  { value: "%H-%M", label: "24H HH-MM" },
  { value: "%I %M", label: "12H HH MM (blink)" },
  { value: "%I:%M", label: "12H HH:MM" },
  { value: "%I-%M", label: "12H HH-MM" },
  { value: "%H %M %S", label: "24H HH MM SS (blink)" },
  { value: "%H:%M:%S", label: "24H HH:MM:SS" },
  { value: "%H-%M-%S", label: "24H HH-MM-SS" },
  { value: "%I %M %S", label: "12H HH MM SS (blink)" },
  { value: "%I:%M:%S", label: "12H HH:MM:SS" },
  { value: "%I-%M-%S", label: "12H HH-MM-SS" },
];

const DATE_FORMATS = [
  { value: "%d.%m.%y", label: "DD.MM.YY" },
  { value: "%d-%m-%y", label: "DD-MM-YY" },
  { value: "%d.%m.", label: "DD.MM." },
  { value: "%d %b", label: "DD Mon" },
  { value: "%m.%d.%y", label: "MM.DD.YY" },
  { value: "%m-%d-%y", label: "MM-DD-YY" },
  { value: "%m/%d/%y", label: "MM/DD/YY" },
  { value: "%b %d", label: "Mon DD" },
  { value: "%y-%m-%d", label: "YY-MM-DD (ISO)" },
];

export function TimeDateSection() {
  const { settings, updateSettings, saveDisplaySettings } = useSettings();
  const [saving, setSaving] = useState(false);
  const t = useT();
  if (!settings) return null;
  const s = settings;

  const timeModeOptions = useMemo(() => [
    { value: 0, label: t.timeModes.plainText },
    { value: 1, label: t.timeModes.calendarBelow },
    { value: 2, label: t.timeModes.calendarAbove },
    { value: 3, label: t.timeModes.calendarAltBelow },
    { value: 4, label: t.timeModes.calendarAltAbove },
    { value: 5, label: t.timeModes.bigDigits },
    { value: 6, label: t.timeModes.binary },
  ], [t]);

  const hasSeconds = s.TFORMAT?.includes("%S") ?? false;
  const filteredTimeModes = useMemo(() => {
    if (hasSeconds) {
      return timeModeOptions.filter((m) => m.value === 0 || m.value === 6);
    }
    return timeModeOptions;
  }, [hasSeconds, timeModeOptions]);

  function handleTimeFormatChange(format: string) {
    const newHasSeconds = format.includes("%S");
    const updates: Record<string, unknown> = { TFORMAT: format };
    if (newHasSeconds && s.TMODE >= 1 && s.TMODE <= 5) {
      updates.TMODE = 0;
    }
    updateSettings(updates);
  }

  async function handleSave() {
    setSaving(true);
    await saveDisplaySettings({
      TFORMAT: s.TFORMAT, DFORMAT: s.DFORMAT, TMODE: s.TMODE,
      SOM: s.SOM,
      TIME_COL: s.TIME_COL, DATE_COL: s.DATE_COL,
      WD: s.WD, WDCA: s.WDCA, WDCI: s.WDCI,
      CHCOL: s.CHCOL, CTCOL: s.CTCOL, CBCOL: s.CBCOL,
    }, t.datetime.datetimeSaved);
    setSaving(false);
  }

  return (
    <Card title={t.datetime.title}>
      <div class={styles.stack}>
        <FormRow>
          <Select
            label={t.datetime.timeFormat}
            value={s.TFORMAT}
            options={TIME_FORMATS}
            onChange={(v) => handleTimeFormatChange(v as string)}
          />
          <Select
            label={t.datetime.dateFormat}
            value={s.DFORMAT}
            options={DATE_FORMATS}
            onChange={(v) => updateSettings({ DFORMAT: v as string })}
          />
        </FormRow>
        <Select
          label={t.datetime.timeMode}
          value={s.TMODE}
          options={filteredTimeModes}
          onChange={(v) => updateSettings({ TMODE: v as number })}
        />
        <Toggle label={t.datetime.startOnMonday} checked={s.SOM} onChange={(v) => updateSettings({ SOM: v })} />
        <Toggle label={t.datetime.showWeekday} checked={s.WD} onChange={(v) => updateSettings({ WD: v })} />
        <FormRow>
          <ColorField label={t.datetime.timeColor} value={s.TIME_COL} onChange={(v) => updateSettings({ TIME_COL: v })} />
          <ColorField label={t.datetime.dateColor} value={s.DATE_COL} onChange={(v) => updateSettings({ DATE_COL: v })} />
        </FormRow>
        <FormRow>
          <ColorField label={t.datetime.weekdayActive} value={s.WDCA} onChange={(v) => updateSettings({ WDCA: v })} />
          <ColorField label={t.datetime.weekdayInactive} value={s.WDCI} onChange={(v) => updateSettings({ WDCI: v })} />
        </FormRow>
        <FormRow>
          <ColorField label={t.datetime.calHeader} value={s.CHCOL} onChange={(v) => updateSettings({ CHCOL: v })} />
          <ColorField label={t.datetime.calText} value={s.CTCOL} onChange={(v) => updateSettings({ CTCOL: v })} />
        </FormRow>
        <ColorField label={t.datetime.calBody} value={s.CBCOL} onChange={(v) => updateSettings({ CBCOL: v })} />
        <Button variant="primary" onClick={handleSave} loading={saving}>{t.datetime.saveButton}</Button>
      </div>
    </Card>
  );
}
