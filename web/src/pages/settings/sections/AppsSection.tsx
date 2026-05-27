import { useState } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Select, Card, Button } from "../../../components/ui";
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

const TEMP_OFFSET_OPTIONS = [
  { value: 5, label: "+5°" },
  { value: 4, label: "+4°" },
  { value: 3, label: "+3°" },
  { value: 2, label: "+2°" },
  { value: 1, label: "+1°" },
  { value: 0, label: "0°" },
  { value: -1, label: "-1°" },
  { value: -2, label: "-2°" },
  { value: -3, label: "-3°" },
  { value: -4, label: "-4°" },
  { value: -5, label: "-5°" },
  { value: -6, label: "-6°" },
  { value: -7, label: "-7°" },
  { value: -8, label: "-8°" },
  { value: -9, label: "-9°" },
  { value: -10, label: "-10°" },
  { value: -11, label: "-11°" },
  { value: -12, label: "-12°" },
  { value: -13, label: "-13°" },
  { value: -14, label: "-14°" },
  { value: -15, label: "-15°" },
];

export function AppsSection() {
  const { settings, updateSettings, saveDisplaySettings } = useSettings();
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
      STIMER: s.STIMER, TIMER_COL: s.TIMER_COL,
      SSW: s.SSW, SW_COL: s.SW_COL,
      SALARMS: s.SALARMS, ALARMS_COL: s.ALARMS_COL,
      ATIME: s.ATIME, ATRANS: s.ATRANS,
      TEFF: s.TEFF, TSPEED: s.TSPEED,
      SSPEED: s.SSPEED, BLOCKN: s.BLOCKN,
      TIMEDUR: s.TIMEDUR,
      DATEDUR: s.DATEDUR,
      TEMPDUR: s.TEMPDUR,
      HUMDUR: s.HUMDUR,
      BATDUR: s.BATDUR,
      CEL: s.CEL,
      TOFF: s.TOFF,
    });
    setSaving(false);
  }

  return (
    <Card title="Apps">
      <div class={styles.stack}>
        <div class={styles.appRow}>
          <Toggle label="Time" checked={s.TIM} onChange={(v) => updateSettings({ TIM: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={300} value={s.TIMEDUR || 7} onChange={(v) => updateSettings({ TIMEDUR: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label="Date" checked={s.DAT} onChange={(v) => updateSettings({ DAT: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={s.DATEDUR || 7} onChange={(v) => updateSettings({ DATEDUR: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label="Temperature" checked={s.TEMP} onChange={(v) => updateSettings({ TEMP: v })} />
          <ColorField label="" value={s.TEMP_COL} onChange={(v) => updateSettings({ TEMP_COL: v })} />
          <Toggle label="Celsius" checked={s.CEL} onChange={(v) => updateSettings({ CEL: v })} />
          <Select
            label="Offset"
            value={s.TOFF ?? -9}
            options={TEMP_OFFSET_OPTIONS}
            onChange={(v) => updateSettings({ TOFF: v as number })}
          />
          <div style={{ width: "470px", flexShrink: 0 }}>
            <Slider label="" min={1} max={60} value={s.TEMPDUR || 7} onChange={(v) => updateSettings({ TEMPDUR: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label="Humidity" checked={s.HUM} onChange={(v) => updateSettings({ HUM: v })} />
          <ColorField label="" value={s.HUM_COL} onChange={(v) => updateSettings({ HUM_COL: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={s.HUMDUR || 7} onChange={(v) => updateSettings({ HUMDUR: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label="Battery" checked={s.BAT} onChange={(v) => updateSettings({ BAT: v })} />
          <ColorField label="" value={s.BAT_COL} onChange={(v) => updateSettings({ BAT_COL: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={s.BATDUR || 7} onChange={(v) => updateSettings({ BATDUR: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label="Timer" checked={s.STIMER} onChange={(v) => updateSettings({ STIMER: v })} />
          <Toggle label="Auto" checked={s.TIMER_COL === 0} onChange={(v) => updateSettings({ TIMER_COL: v ? 0 : 0xFFFFFF })} />
          {s.TIMER_COL !== 0 && <ColorField label="" value={s.TIMER_COL} onChange={(v) => updateSettings({ TIMER_COL: v })} />}
        </div>

        <div class={styles.appRow}>
          <Toggle label="Stopwatch" checked={s.SSW} onChange={(v) => updateSettings({ SSW: v })} />
          <Toggle label="Auto" checked={s.SW_COL === 0} onChange={(v) => updateSettings({ SW_COL: v ? 0 : 0xFFFFFF })} />
          {s.SW_COL !== 0 && <ColorField label="" value={s.SW_COL} onChange={(v) => updateSettings({ SW_COL: v })} />}
        </div>

        <div class={styles.appRow}>
          <Toggle label="Alarms" checked={s.SALARMS} onChange={(v) => updateSettings({ SALARMS: v })} />
          <Toggle label="Auto" checked={s.ALARMS_COL === 0} onChange={(v) => updateSettings({ ALARMS_COL: v ? 0 : 0xFFFFFF })} />
          {s.ALARMS_COL !== 0 && <ColorField label="" value={s.ALARMS_COL} onChange={(v) => updateSettings({ ALARMS_COL: v })} />}
        </div>

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
        <Button variant="primary" onClick={handleSave} loading={saving}>Save Apps</Button>
      </div>
    </Card>
  );
}
