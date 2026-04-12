import { useState } from "preact/hooks";
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
    <Card title="Night Mode">
      <div class={styles.stack}>
        <Toggle
          label="Enable Night Mode"
          checked={s.NMODE}
          onChange={(v) => updateSettings({ NMODE: v })}
        />
        {s.NMODE && (
          <>
            <FormRow>
              <div class="form-group">
                <label htmlFor="night-start">Start</label>
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
                <label htmlFor="night-end">End</label>
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
              label="Night Brightness"
              min={1}
              max={50}
              value={s.NBRI}
              onChange={(v) => updateSettings({ NBRI: v })}
            />
            <ColorField
              label="Night Color"
              value={s.NCOL}
              onChange={(v) => updateSettings({ NCOL: v })}
            />
            <Toggle
              label="Block Auto-Transition"
              checked={s.NBTRANS}
              onChange={(v) => updateSettings({ NBTRANS: v })}
            />
          </>
        )}
        <Button variant="primary" onClick={handleSave} loading={saving}>
          Save Night Mode
        </Button>
      </div>
    </Card>
  );
}
