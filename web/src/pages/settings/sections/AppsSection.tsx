import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Select, Card } from "../../../components/ui";
import { useT } from "../../../i18n";
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
  const { settings, autoSave, instantSave } = useSettings();
  const t = useT();
  if (!settings) return null;
  const s = settings;

  return (
    <Card title={t.apps.title}>
      <div class={styles.stack}>
        <div class={styles.appRow}>
          <Toggle label={t.apps.time} checked={s.TIM} onChange={(v) => instantSave({ TIM: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={300} value={s.TIMEDUR || 7} onChange={(v) => autoSave({ TIMEDUR: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label={t.apps.date} checked={s.DAT} onChange={(v) => instantSave({ DAT: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={s.DATEDUR || 7} onChange={(v) => autoSave({ DATEDUR: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label={t.apps.temperature} checked={s.TEMP} onChange={(v) => instantSave({ TEMP: v })} />
          <ColorField label="" value={s.TEMP_COL} onChange={(v) => autoSave({ TEMP_COL: v })} />
          <Toggle label={t.apps.celsius} checked={s.CEL} onChange={(v) => instantSave({ CEL: v })} />
          <Select
            label={t.apps.offset}
            value={s.TOFF ?? -9}
            options={TEMP_OFFSET_OPTIONS}
            onChange={(v) => instantSave({ TOFF: v as number })}
          />
          <div style={{ width: "470px", flexShrink: 0 }}>
            <Slider label="" min={1} max={60} value={s.TEMPDUR || 7} onChange={(v) => autoSave({ TEMPDUR: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label={t.apps.humidity} checked={s.HUM} onChange={(v) => instantSave({ HUM: v })} />
          <ColorField label="" value={s.HUM_COL} onChange={(v) => autoSave({ HUM_COL: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={s.HUMDUR || 7} onChange={(v) => autoSave({ HUMDUR: v })} unit="s" />
          </div>
        </div>

        <div class={styles.appRow}>
          <Toggle label={t.apps.battery} checked={s.BAT} onChange={(v) => instantSave({ BAT: v })} />
          <ColorField label="" value={s.BAT_COL} onChange={(v) => autoSave({ BAT_COL: v })} />
          <div class={styles.appSlider}>
            <Slider label="" min={1} max={60} value={s.BATDUR || 7} onChange={(v) => autoSave({ BATDUR: v })} unit="s" />
          </div>
        </div>
      </div>
    </Card>
  );
}
