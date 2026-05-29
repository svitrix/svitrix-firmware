import { useState } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Select, Card, Button } from "../../../components/ui";
import styles from "./sections.module.css";

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

        <Button variant="primary" onClick={handleSave} loading={saving}>Save Apps</Button>
      </div>
    </Card>
  );
}
