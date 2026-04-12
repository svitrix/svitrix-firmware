import { useState } from "preact/hooks";
import { useSettings } from "../../context/SettingsContext";
import { Toggle, TextField, ColorField, Select, Card, FormRow, Button } from "../../components/ui";

export function TimeDateSection() {
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
    <Card title="Time & Date Format">
      <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
        <FormRow>
          <TextField label="Time Format" value={s.TFORMAT} onChange={(v) => updateSettings({ TFORMAT: v })} />
          <TextField label="Date Format" value={s.DFORMAT} onChange={(v) => updateSettings({ DFORMAT: v })} />
        </FormRow>
        <Select
          label="Time Mode"
          value={s.TMODE}
          options={[
            { value: 0, label: "Plain Text" },
            { value: 1, label: "Calendar" },
            { value: 2, label: "Calendar Top" },
            { value: 3, label: "Calendar Alt" },
            { value: 4, label: "Calendar Alt Top" },
            { value: 5, label: "Big Digits" },
            { value: 6, label: "Binary" },
          ]}
          onChange={(v) => updateSettings({ TMODE: +v })}
        />
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
