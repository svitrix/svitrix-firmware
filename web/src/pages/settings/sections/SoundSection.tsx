import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, Card } from "../../../components/ui";
import styles from "./sections.module.css";

export function SoundSection() {
  const { t } = useTranslation();
  const { settings, setSetting } = useSettings();
  if (!settings) return null;
  const s = settings;

  return (
    <Card title={t("settingsDisplay.sound.title")}>
      <div class={styles.stack}>
        <Toggle label={t("settingsDisplay.sound.soundEnabled")} checked={s.SOUND} onChange={(v) => setSetting({ SOUND: v }, true)} />
        <Slider label={t("settingsDisplay.sound.volume")} min={0} max={30} value={s.VOL} onChange={(v) => setSetting({ VOL: v })} />
      </div>
    </Card>
  );
}
