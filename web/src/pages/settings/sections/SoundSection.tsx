import { useState } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, Card, Button } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function SoundSection() {
  const { settings, updateSettings, saveDisplaySettings } = useSettings();
  const [saving, setSaving] = useState(false);
  const t = useT();
  if (!settings) return null;
  const s = settings;

  async function handleSave() {
    setSaving(true);
    await saveDisplaySettings({ SOUND: s.SOUND, VOL: s.VOL });
    setSaving(false);
  }

  return (
    <Card title={t.sound.title}>
      <div class={styles.stack}>
        <Toggle label={t.sound.enabled} checked={s.SOUND} onChange={(v) => updateSettings({ SOUND: v })} />
        <Slider label={t.sound.volume} min={0} max={30} value={s.VOL} onChange={(v) => updateSettings({ VOL: v })} />
        <Button variant="primary" onClick={handleSave} loading={saving}>{t.sound.saveButton}</Button>
      </div>
    </Card>
  );
}
