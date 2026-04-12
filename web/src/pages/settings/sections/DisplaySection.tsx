import { useState } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Card, FormRow, Button } from "../../../components/ui";
import styles from "./sections.module.css";

export function DisplaySection() {
  const { settings, updateSettings, saveDisplaySettings } = useSettings();
  const [saving, setSaving] = useState(false);
  if (!settings) return null;
  const s = settings;

  async function handleSave() {
    setSaving(true);
    await saveDisplaySettings({
      MATP: s.MATP, ABRI: s.ABRI, BRI: s.BRI,
      GAMMA: s.GAMMA, UPPERCASE: s.UPPERCASE,
      TCOL: s.TCOL, CCORRECTION: s.CCORRECTION, CTEMP: s.CTEMP,
    });
    setSaving(false);
  }

  return (
    <Card title="Display">
      <div class={styles.stack}>
        <Toggle label="Matrix Power" checked={s.MATP} onChange={(v) => updateSettings({ MATP: v })} />
        <Toggle label="Auto Brightness" checked={s.ABRI} onChange={(v) => updateSettings({ ABRI: v })} />
        <Slider label="Brightness" min={0} max={255} value={s.BRI} onChange={(v) => updateSettings({ BRI: v })} />
        <Slider label="Gamma" min={0.5} max={3} step={0.1} value={s.GAMMA} onChange={(v) => updateSettings({ GAMMA: v })} />
        <Toggle label="Uppercase" checked={s.UPPERCASE} onChange={(v) => updateSettings({ UPPERCASE: v })} />
        <ColorField label="Text Color" value={s.TCOL} onChange={(v) => updateSettings({ TCOL: v })} />
        <FormRow>
          <div class="form-group">
            <label>Color Correction</label>
            <input type="color" value={s.CCORRECTION}
              onInput={(e) => updateSettings({ CCORRECTION: (e.target as HTMLInputElement).value })} />
          </div>
          <div class="form-group">
            <label>Color Temperature</label>
            <input type="color" value={s.CTEMP}
              onInput={(e) => updateSettings({ CTEMP: (e.target as HTMLInputElement).value })} />
          </div>
        </FormRow>
        <Button variant="primary" onClick={handleSave} loading={saving}>Save Display</Button>
      </div>
    </Card>
  );
}
