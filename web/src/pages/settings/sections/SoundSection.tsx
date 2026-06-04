import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, Card } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function SoundSection() {
  const { settings, autoSave, instantSave } = useSettings();
  const t = useT();
  if (!settings) return null;
  const s = settings;

  return (
    <Card title={t.sound.title}>
      <div class={styles.stack}>
        <Toggle label={t.sound.enabled} checked={s.SOUND} onChange={(v) => instantSave({ SOUND: v })} />
        <Slider label={t.sound.volume} min={0} max={30} value={s.VOL} onChange={(v) => autoSave({ VOL: v })} />
      </div>
    </Card>
  );
}
