import { useState } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Select, Card, FormRow, Button } from "../../../components/ui";
import styles from "./sections.module.css";

export function AppsSection() {
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
    <Card title="Apps">
      <div class={styles.stack}>
        <Toggle label="Time" checked={s.TIM} onChange={(v) => updateSettings({ TIM: v })} />
        <Toggle label="Date" checked={s.DAT} onChange={(v) => updateSettings({ DAT: v })} />
        <FormRow>
          <Toggle label="Temperature" checked={s.TEMP} onChange={(v) => updateSettings({ TEMP: v })} />
          <ColorField label="Temp Color" value={s.TEMP_COL} onChange={(v) => updateSettings({ TEMP_COL: v })} />
        </FormRow>
        <FormRow>
          <Toggle label="Humidity" checked={s.HUM} onChange={(v) => updateSettings({ HUM: v })} />
          <ColorField label="Hum Color" value={s.HUM_COL} onChange={(v) => updateSettings({ HUM_COL: v })} />
        </FormRow>
        <FormRow>
          <Toggle label="Battery" checked={s.BAT} onChange={(v) => updateSettings({ BAT: v })} />
          <ColorField label="Battery Color" value={s.BAT_COL} onChange={(v) => updateSettings({ BAT_COL: v })} />
        </FormRow>
        <Slider label="App Duration" min={1} max={60} value={s.ATIME} onChange={(v) => updateSettings({ ATIME: v })} unit="s" />
        <Toggle label="Auto Transition" checked={s.ATRANS} onChange={(v) => updateSettings({ ATRANS: v })} />
        {transitions.length > 0 && (
          <Select
            label="Transition Effect"
            value={s.TEFF}
            options={transitions.map((t, i) => ({ value: i, label: t.name }))}
            onChange={(v) => updateSettings({ TEFF: v as number })}
          />
        )}
        <Slider label="Transition Speed" min={100} max={2000} step={100} value={s.TSPEED} onChange={(v) => updateSettings({ TSPEED: v })} unit="ms" />
        <Slider label="Scroll Speed" min={10} max={100} value={s.SSPEED} onChange={(v) => updateSettings({ SSPEED: v })} />
        <Toggle label="Block Navigation" checked={s.BLOCKN} onChange={(v) => updateSettings({ BLOCKN: v })} />
        <Button variant="primary" onClick={handleSave} loading={saving}>Save Apps</Button>
      </div>
    </Card>
  );
}
