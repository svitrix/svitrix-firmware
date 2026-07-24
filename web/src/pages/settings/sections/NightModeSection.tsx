import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Card, FormRow, Button } from "../../../components/ui";
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
  const { settings, updateSettings, saveDisplaySettings } = useSettings();
  const [saving, setSaving] = useState(false);
  if (!settings) return null;
  const s = settings;

  async function handleSave() {
    setSaving(true);
    await saveDisplaySettings({
      NMODE: s.NMODE,
      NSTART: s.NSTART,
      NEND: s.NEND,
      NBRI: s.NBRI,
      NCOL: s.NCOL,
      NBTRANS: s.NBTRANS,
    });
    setSaving(false);
  }

  return (
    <Card title={t("settingsDisplay.nightMode.title")}>
      <div class={styles.stack}>
        <Toggle
          label={t("settingsDisplay.nightMode.enable")}
          checked={s.NMODE}
          onChange={(v) => updateSettings({ NMODE: v })}
        />
        {s.NMODE && (
          <>
            <FormRow>
              <div class="form-group">
                <label htmlFor="night-start">{t("settingsDisplay.nightMode.start")}</label>
                <input
                  id="night-start"
                  type="time"
                  value={minutesToTime(s.NSTART)}
                  onInput={(e) =>
                    updateSettings({ NSTART: timeToMinutes((e.target as HTMLInputElement).value) })
                  }
                />
              </div>
              <div class="form-group">
                <label htmlFor="night-end">{t("settingsDisplay.nightMode.end")}</label>
                <input
                  id="night-end"
                  type="time"
                  value={minutesToTime(s.NEND)}
                  onInput={(e) =>
                    updateSettings({ NEND: timeToMinutes((e.target as HTMLInputElement).value) })
                  }
                />
              </div>
            </FormRow>
            <Slider
              label={t("settingsDisplay.nightMode.nightBrightness")}
              min={1}
              max={50}
              value={s.NBRI}
              onChange={(v) => updateSettings({ NBRI: v })}
            />
            <ColorField
              label={t("settingsDisplay.nightMode.nightColor")}
              value={s.NCOL}
              onChange={(v) => updateSettings({ NCOL: v })}
            />
            <Toggle
              label={t("settingsDisplay.nightMode.blockAutoTransition")}
              checked={s.NBTRANS}
              onChange={(v) => updateSettings({ NBTRANS: v })}
            />
          </>
        )}
        <Button variant="primary" onClick={handleSave} loading={saving}>
          {t("settingsDisplay.nightMode.save")}
        </Button>
      </div>
    </Card>
  );
}
