import { useState, useMemo } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, Select, Card, Button } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function AppTransitionsSection() {
  const { settings, updateSettings, saveDisplaySettings } = useSettings();
  const [saving, setSaving] = useState(false);
  const t = useT();

  const transitionOptions = useMemo(() => [
    { value: 0, label: t.transitions.random },
    { value: 1, label: t.transitions.slideDown },
    { value: 11, label: t.transitions.slideUp },
    { value: 12, label: t.transitions.slideLeft },
    { value: 13, label: t.transitions.slideRight },
    { value: 2, label: t.transitions.dim },
    { value: 3, label: t.transitions.zoom },
    { value: 4, label: t.transitions.rotate },
    { value: 5, label: t.transitions.pixelate },
    { value: 6, label: t.transitions.curtain },
    { value: 7, label: t.transitions.ripple },
    { value: 8, label: t.transitions.blink },
    { value: 9, label: t.transitions.reload },
    { value: 10, label: t.transitions.fade },
  ], [t]);

  if (!settings) return null;
  const s = settings;

  async function handleSave() {
    setSaving(true);
    await saveDisplaySettings({
      SALARMS: s.SALARMS,
      ATRANS: s.ATRANS,
      TEFF: s.TEFF,
      TSPEED: s.TSPEED,
      SSPEED: s.SSPEED,
      BLOCKN: s.BLOCKN,
    }, t.apps.transitionsSaved);
    setSaving(false);
  }

  return (
    <Card title={t.apps.transitions}>
      <div class={styles.stack}>
        <Toggle label={t.apps.alarmsIndicator} checked={s.SALARMS} onChange={(v) => updateSettings({ SALARMS: v })} />
        <Toggle label={t.apps.autoTransition} checked={s.ATRANS} onChange={(v) => updateSettings({ ATRANS: v })} />
        <Select
          label={t.apps.transitionEffect}
          value={s.TEFF}
          options={transitionOptions}
          onChange={(v) => updateSettings({ TEFF: v as number })}
        />
        <Slider label={t.apps.transitionSpeed} min={100} max={2000} step={100} value={s.TSPEED} onChange={(v) => updateSettings({ TSPEED: v })} unit="ms" />
        <Slider label={t.apps.scrollSpeed} min={10} max={100} value={s.SSPEED} onChange={(v) => updateSettings({ SSPEED: v })} />
        <Toggle label={t.apps.blockNavigation} checked={s.BLOCKN} onChange={(v) => updateSettings({ BLOCKN: v })} />
        <Button variant="primary" onClick={handleSave} loading={saving}>{t.apps.saveTransitions}</Button>
      </div>
    </Card>
  );
}
