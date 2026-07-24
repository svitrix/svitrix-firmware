import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Select, Card, FormRow, Button } from "../../../components/ui";
import styles from "./sections.module.css";

export function DisplaySection() {
  const { t } = useTranslation();
  const { settings, effects, updateSettings, saveDisplaySettings } = useSettings();
  const [saving, setSaving] = useState(false);
  if (!settings) return null;
  const s = settings;

  const humanize = (name: string, fallback: string) => {
    const raw = (name ?? "").trim();
    if (!raw) return fallback;
    return raw
      .replace(/[_-]+/g, " ")
      .replace(/([a-z])([A-Z])/g, "$1 $2")
      .replace(/([A-Z]+)([A-Z][a-z])/g, "$1 $2")
      .replace(/\s+/g, " ")
      .trim();
  };

  async function handleSave() {
    setSaving(true);
    await saveDisplaySettings({
      MATP: s.MATP, ABRI: s.ABRI, BRI: s.BRI,
      GAMMA: s.GAMMA, UPPERCASE: s.UPPERCASE,
      TCOL: s.TCOL, CCORRECTION: s.CCORRECTION, CTEMP: s.CTEMP,
      BEFF: s.BEFF,
    });
    setSaving(false);
  }

  return (
    <Card title={t("settingsDisplay.display.title")}>
      <div class={styles.stack}>
        <Toggle label={t("settingsDisplay.display.matrixPower")} checked={s.MATP} onChange={(v) => updateSettings({ MATP: v })} />
        <Toggle label={t("settingsDisplay.display.autoBrightness")} checked={s.ABRI} onChange={(v) => updateSettings({ ABRI: v })} />
        <Slider label={t("settingsDisplay.display.brightness")} min={0} max={255} value={s.BRI} onChange={(v) => updateSettings({ BRI: v })} />
        <Slider label={t("settingsDisplay.display.gamma")} min={0.5} max={3} step={0.1} value={s.GAMMA} onChange={(v) => updateSettings({ GAMMA: v })} />
        <Toggle label={t("settingsDisplay.display.uppercase")} checked={s.UPPERCASE} onChange={(v) => updateSettings({ UPPERCASE: v })} />
        <ColorField label={t("settingsDisplay.display.textColor")} value={s.TCOL} onChange={(v) => updateSettings({ TCOL: v })} />
        <Select
          label={t("settingsDisplay.display.backgroundEffect")}
          value={s.BEFF ?? -1}
          options={[
            { value: -1, label: t("common.none") },
            ...effects.map((e, i) => ({
              value: i,
              label: humanize(e.name, t("settingsDisplay.display.effectFallback", { n: i + 1 })),
            })),
          ]}
          onChange={(v) => updateSettings({ BEFF: v as number })}
        />
        <FormRow>
          <div class="form-group">
            <label>{t("settingsDisplay.display.colorCorrection")}</label>
            <input type="color" value={s.CCORRECTION}
              onInput={(e) => updateSettings({ CCORRECTION: (e.target as HTMLInputElement).value })} />
          </div>
          <div class="form-group">
            <label>{t("settingsDisplay.display.colorTemperature")}</label>
            <input type="color" value={s.CTEMP}
              onInput={(e) => updateSettings({ CTEMP: (e.target as HTMLInputElement).value })} />
          </div>
        </FormRow>
        <Button variant="primary" onClick={handleSave} loading={saving}>{t("settingsDisplay.display.save")}</Button>
      </div>
    </Card>
  );
}
