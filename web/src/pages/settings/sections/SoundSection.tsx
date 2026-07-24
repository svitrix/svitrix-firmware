import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, Card, Button } from "../../../components/ui";
import styles from "./sections.module.css";

export function SoundSection() {
  const { t } = useTranslation();
  const { settings, updateSettings, saveDisplaySettings } = useSettings();
  const [saving, setSaving] = useState(false);
  if (!settings) return null;
  const s = settings;

  async function handleSave() {
    setSaving(true);
    await saveDisplaySettings({ SOUND: s.SOUND, VOL: s.VOL });
    setSaving(false);
  }

  return (
    <Card title={t("settingsDisplay.sound.title")}>
      <div class={styles.stack}>
        <Toggle label={t("settingsDisplay.sound.soundEnabled")} checked={s.SOUND} onChange={(v) => updateSettings({ SOUND: v })} />
        <Slider label={t("settingsDisplay.sound.volume")} min={0} max={30} value={s.VOL} onChange={(v) => updateSettings({ VOL: v })} />
        <Button variant="primary" onClick={handleSave} loading={saving}>{t("settingsDisplay.sound.save")}</Button>
      </div>
    </Card>
  );
}
