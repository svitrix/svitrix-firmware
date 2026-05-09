import { useState, useMemo } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, ColorField, Select, Card, FormRow, Button } from "../../../components/ui";
import styles from "./sections.module.css";

const TIME_FORMATS = [
  { value: "%H %M", label: "24H HH:MM (blink)" },
  { value: "%H:%M", label: "24H HH:MM (:)" },
  { value: "%H-%M", label: "24H HH:MM (-)" },
  { value: "%I %M", label: "12H HH:MM (blink)" },
  { value: "%I:%M", label: "12H HH:MM (:)" },
  { value: "%I-%M", label: "12H HH:MM (-)" },
  { value: "%H %M %S", label: "24H HH:MM:SS (blink)" },
  { value: "%H:%M:%S", label: "24H HH:MM:SS (:)" },
  { value: "%H-%M-%S", label: "24H HH:MM:SS (-)" },
  { value: "%I %M %S", label: "12H HH:MM:SS (blink)" },
  { value: "%I:%M:%S", label: "12H HH:MM:SS (:)" },
  { value: "%I-%M-%S", label: "12H HH:MM:SS (-)" },
];

const DATE_FORMATS = [
  { value: "%d.%m.%y", label: "DD.MM.YY" },
  { value: "%d-%m-%y", label: "DD-MM-YY" },
  { value: "%d %b", label: "DD Mon" },
  { value: "%d %b %Y", label: "DD Mon YYYY" },
  { value: "%m.%d.%y", label: "MM.DD.YY" },
  { value: "%m-%d-%y", label: "MM-DD-YY" },
  { value: "%b %d", label: "Mon DD" },
  { value: "%b %d %Y", label: "Mon DD YYYY" },
];

const ALL_TIME_MODES = [
  { value: 0, label: "Plain Text (weekday below)" },
  { value: 1, label: "Calendar (weekday below)" },
  { value: 2, label: "Calendar (weekday above)" },
  { value: 3, label: "Calendar Alt (weekday below)" },
  { value: 4, label: "Calendar Alt (weekday above)" },
  { value: 5, label: "Big Digits" },
  { value: 6, label: "Binary" },
];

export function TimeDateSection() {
  const { settings, updateSettings, saveDisplaySettings } = useSettings();
  const [saving, setSaving] = useState(false);
  if (!settings) return null;
  const s = settings;

  const hasSeconds = s.TFORMAT?.includes("%S") ?? false;
  const timeModeOptions = useMemo(() => {
    if (hasSeconds) {
      return ALL_TIME_MODES.filter((m) => m.value === 0 || m.value === 6);
    }
    return ALL_TIME_MODES;
  }, [hasSeconds]);

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
      SOM: s.SOM, CEL: s.CEL,
      TIME_COL: s.TIME_COL, DATE_COL: s.DATE_COL,
      WD: s.WD, WDCA: s.WDCA, WDCI: s.WDCI,
      CHCOL: s.CHCOL, CTCOL: s.CTCOL, CBCOL: s.CBCOL,
    });
    setSaving(false);
  }

  return (
    <Card title="Time & Date Format">
      <div class={styles.stack}>
        <FormRow>
          <Select
            label="Time Format"
            value={s.TFORMAT}
            options={TIME_FORMATS}
            onChange={(v) => handleTimeFormatChange(v as string)}
          />
          <Select
            label="Date Format"
            value={s.DFORMAT}
            options={DATE_FORMATS}
            onChange={(v) => updateSettings({ DFORMAT: v as string })}
          />
        </FormRow>
        <Select
          label="Time Mode"
          value={s.TMODE}
          options={timeModeOptions}
          onChange={(v) => updateSettings({ TMODE: v as number })}
        />
        {hasSeconds && <p class={styles.hint}>Calendar and Big Digits modes disabled when showing seconds</p>}
        <Toggle label="Start on Monday" checked={s.SOM} onChange={(v) => updateSettings({ SOM: v })} />
        <Toggle label="Celsius" checked={s.CEL} onChange={(v) => updateSettings({ CEL: v })} />
        <FormRow>
          <ColorField label="Time Color" value={s.TIME_COL} onChange={(v) => updateSettings({ TIME_COL: v })} />
          <ColorField label="Date Color" value={s.DATE_COL} onChange={(v) => updateSettings({ DATE_COL: v })} />
        </FormRow>
        <FormRow>
          <Toggle label="Show Weekday" checked={s.WD} onChange={(v) => updateSettings({ WD: v })} />
        </FormRow>
        <FormRow>
          <ColorField label="Weekday Active" value={s.WDCA} onChange={(v) => updateSettings({ WDCA: v })} />
          <ColorField label="Weekday Inactive" value={s.WDCI} onChange={(v) => updateSettings({ WDCI: v })} />
        </FormRow>
        <FormRow>
          <ColorField label="Cal Header" value={s.CHCOL} onChange={(v) => updateSettings({ CHCOL: v })} />
          <ColorField label="Cal Text" value={s.CTCOL} onChange={(v) => updateSettings({ CTCOL: v })} />
        </FormRow>
        <ColorField label="Cal Body" value={s.CBCOL} onChange={(v) => updateSettings({ CBCOL: v })} />
        <Button variant="primary" onClick={handleSave} loading={saving}>Save Time & Date</Button>
      </div>
    </Card>
  );
}
