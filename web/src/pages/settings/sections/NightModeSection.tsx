import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Card, FormRow } from "../../../components/ui";
import { useT } from "../../../i18n";
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
  const { settings, autoSave, instantSave } = useSettings();
  const t = useT();
  if (!settings) return null;
  const s = settings;

  return (
    <Card title={t.display.nightMode}>
      <div class={styles.stack}>
        <Toggle
          label={t.display.nightModeEnabled}
          checked={s.NMODE}
          onChange={(v) => instantSave({ NMODE: v })}
        />
        {s.NMODE && (
          <>
            <FormRow>
              <div class="form-group">
                <label htmlFor="night-start">{t.display.nightStart}</label>
                <input
                  id="night-start"
                  type="time"
                  value={minutesToTime(s.NSTART)}
                  onInput={(e) =>
                    autoSave({ NSTART: timeToMinutes((e.target as HTMLInputElement).value) })
                  }
                />
              </div>
              <div class="form-group">
                <label htmlFor="night-end">{t.display.nightEnd}</label>
                <input
                  id="night-end"
                  type="time"
                  value={minutesToTime(s.NEND)}
                  onInput={(e) =>
                    autoSave({ NEND: timeToMinutes((e.target as HTMLInputElement).value) })
                  }
                />
              </div>
            </FormRow>
            <Slider
              label={t.display.nightBrightness}
              min={1}
              max={50}
              value={s.NBRI}
              onChange={(v) => autoSave({ NBRI: v })}
            />
            <ColorField
              label={t.display.nightColor}
              value={s.NCOL}
              onChange={(v) => autoSave({ NCOL: v })}
            />
            <Toggle
              label={t.display.blockAutoTransition}
              checked={s.NBTRANS}
              onChange={(v) => instantSave({ NBTRANS: v })}
            />
          </>
        )}
      </div>
    </Card>
  );
}
