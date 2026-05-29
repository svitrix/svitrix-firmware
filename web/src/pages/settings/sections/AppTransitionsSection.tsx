import { useState } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, Select, Card, Button } from "../../../components/ui";
import styles from "./sections.module.css";

const TRANSITION_OPTIONS = [
  { value: 0, label: "Random" },
  { value: 1, label: "Slide Down" },
  { value: 11, label: "Slide Up" },
  { value: 12, label: "Slide Left" },
  { value: 13, label: "Slide Right" },
  { value: 2, label: "Dim" },
  { value: 3, label: "Zoom" },
  { value: 4, label: "Rotate" },
  { value: 5, label: "Pixelate" },
  { value: 6, label: "Curtain" },
  { value: 7, label: "Ripple" },
  { value: 8, label: "Blink" },
  { value: 9, label: "Reload" },
  { value: 10, label: "Fade" },
];

export function AppTransitionsSection() {
  const { settings, updateSettings, saveDisplaySettings } = useSettings();
  const [saving, setSaving] = useState(false);

  if (!settings) return null;
  const s = settings;

  async function handleSave() {
    setSaving(true);
    await saveDisplaySettings({
      SALARMS: s.SALARMS,
      ATRANS: s.ATRANS,
      TEFF: s.TEFF,
      TSPEED: s.TSPEED,
      SSPEED: s.SSPEED,
      BLOCKN: s.BLOCKN,
    });
    setSaving(false);
  }

  return (
    <Card title="Transitions & Navigation">
      <div class={styles.stack}>
        <Toggle label="Alarms Indicator" checked={s.SALARMS} onChange={(v) => updateSettings({ SALARMS: v })} />
        <Toggle label="Auto Transition" checked={s.ATRANS} onChange={(v) => updateSettings({ ATRANS: v })} />
        <Select
          label="Transition Effect"
          value={s.TEFF}
          options={TRANSITION_OPTIONS}
          onChange={(v) => updateSettings({ TEFF: v as number })}
        />
        <Slider label="Transition Speed" min={100} max={2000} step={100} value={s.TSPEED} onChange={(v) => updateSettings({ TSPEED: v })} unit="ms" />
        <Slider label="Scroll Speed" min={10} max={100} value={s.SSPEED} onChange={(v) => updateSettings({ SSPEED: v })} />
        <Toggle label="Block Navigation" checked={s.BLOCKN} onChange={(v) => updateSettings({ BLOCKN: v })} />
        <Button variant="primary" onClick={handleSave} loading={saving}>Save Transitions</Button>
      </div>
    </Card>
  );
}
