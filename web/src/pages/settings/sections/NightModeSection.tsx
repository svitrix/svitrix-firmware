import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, TimeField, Card, FormRow } from "../../../components/ui";
import styles from "./sections.module.css";

function minutesToTime(minutes: number): string {
  const h = Math.floor(minutes / 60) % 24;
  const m = minutes % 60;
  return `${String(h).padStart(2, "0")}:${String(m).padStart(2, "0")}`;
}

function timeToMinutes(time: string): number {
  const [h, m] = time.split(":").map(Number);
  return h * 60 + m;
}

export function NightModeSection() {
  const { t } = useTranslation();
  const { settings, setSetting } = useSettings();
  if (!settings) return null;
  const s = settings;

  return (
    <Card title={t("settingsDisplay.nightMode.title")}>
      <div class={styles.stack}>
        <Toggle
          label={t("settingsDisplay.nightMode.enable")}
          checked={s.NMODE}
          onChange={(v) => setSetting({ NMODE: v }, true)}
        />
        {s.NMODE && (
          <>
            <FormRow>
              <TimeField
                label={t("settingsDisplay.nightMode.start")}
                value={minutesToTime(s.NSTART)}
                onChange={(v) => setSetting({ NSTART: timeToMinutes(v) }, true)}
              />
              <TimeField
                label={t("settingsDisplay.nightMode.end")}
                value={minutesToTime(s.NEND)}
                onChange={(v) => setSetting({ NEND: timeToMinutes(v) }, true)}
              />
            </FormRow>
            <Slider
              label={t("settingsDisplay.nightMode.nightBrightness")}
              min={1}
              max={50}
              value={s.NBRI}
              onChange={(v) => setSetting({ NBRI: v })}
            />
            <ColorField
              label={t("settingsDisplay.nightMode.nightColor")}
              value={s.NCOL}
              onChange={(v) => setSetting({ NCOL: v }, true)}
            />
            <Toggle
              label={t("settingsDisplay.nightMode.blockAutoTransition")}
              helper={t("settingsDisplay.nightMode.blockAutoTransitionHelper")}
              checked={s.NBTRANS}
              onChange={(v) => setSetting({ NBTRANS: v }, true)}
            />
          </>
        )}
      </div>
    </Card>
  );
}
