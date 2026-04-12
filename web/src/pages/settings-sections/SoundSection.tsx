import { useState } from "preact/hooks";
import { useSettings } from "../../context/SettingsContext";
import { Toggle, Slider, Card, Button } from "../../components/ui";

export function SoundSection() {
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
    <Card title="Sound">
      <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
        <Toggle label="Sound Enabled" checked={s.SOUND} onChange={(v) => updateSettings({ SOUND: v })} />
        <Slider label="Volume" min={0} max={30} value={s.VOL} onChange={(v) => updateSettings({ VOL: v })} />
        <Button variant="primary" onClick={handleSave} loading={saving}>Save Sound</Button>
      </div>
    </Card>
  );
}
