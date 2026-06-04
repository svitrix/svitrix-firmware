import { useMemo } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, ColorField, Select, Card, FormRow } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";
import type { Settings } from "../../../api/types";

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
  const { settings, autoSave, instantSave } = useSettings();
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
    const updates: Partial<Settings> = { TFORMAT: format };
    if (newHasSeconds && s.TMODE >= 1 && s.TMODE <= 5) {
      updates.TMODE = 0;
    }
    instantSave(updates);
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
            onChange={(v) => instantSave({ DFORMAT: v as string })}
          />
        </FormRow>
        <Select
          label={t.datetime.timeMode}
          value={s.TMODE}
          options={filteredTimeModes}
          onChange={(v) => instantSave({ TMODE: v as number })}
        />
        <Toggle label={t.datetime.startOnMonday} checked={s.SOM} onChange={(v) => instantSave({ SOM: v })} />
        <Toggle label={t.datetime.showWeekday} checked={s.WD} onChange={(v) => instantSave({ WD: v })} />
        <FormRow>
          <ColorField label={t.datetime.timeColor} value={s.TIME_COL} onChange={(v) => autoSave({ TIME_COL: v })} />
          <ColorField label={t.datetime.dateColor} value={s.DATE_COL} onChange={(v) => autoSave({ DATE_COL: v })} />
        </FormRow>
        <FormRow>
          <ColorField label={t.datetime.weekdayActive} value={s.WDCA} onChange={(v) => autoSave({ WDCA: v })} />
          <ColorField label={t.datetime.weekdayInactive} value={s.WDCI} onChange={(v) => autoSave({ WDCI: v })} />
        </FormRow>
        <FormRow>
          <ColorField label={t.datetime.calHeader} value={s.CHCOL} onChange={(v) => autoSave({ CHCOL: v })} />
          <ColorField label={t.datetime.calText} value={s.CTCOL} onChange={(v) => autoSave({ CTCOL: v })} />
        </FormRow>
        <ColorField label={t.datetime.calBody} value={s.CBCOL} onChange={(v) => autoSave({ CBCOL: v })} />
      </div>
    </Card>
  );
}
