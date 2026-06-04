import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Select, Card, FormRow } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function DisplaySection() {
  const { settings, effects, autoSave, instantSave } = useSettings();
  const t = useT();
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

  return (
    <Card title={t.display.title}>
      <div class={styles.stack}>
        <Toggle label={t.display.matrixPower} checked={s.MATP} onChange={(v) => instantSave({ MATP: v })} />
        <div style={{ display: "flex", alignItems: "center", gap: "16px", flexWrap: "wrap" }}>
          <Toggle label={t.display.autoBrightness} checked={s.ABRI} onChange={(v) => instantSave({ ABRI: v })} />
          {s.ABRI && (
            <>
              <div style={{ minWidth: "335px" }}>
                <Slider label={t.display.minBrightness} min={1} max={50} value={s.MINBRI ?? 2} onChange={(v) => autoSave({ MINBRI: v })} />
              </div>
              <div style={{ minWidth: "335px" }}>
                <Slider label={t.display.maxBrightness} min={0} max={255} value={s.MAXBRI ?? 160} onChange={(v) => autoSave({ MAXBRI: v })} />
              </div>
            </>
          )}
        </div>
        <Slider label={t.display.brightness} min={0} max={255} value={s.BRI} onChange={(v) => autoSave({ BRI: v })} />
        <Slider label={t.display.gamma} min={0.5} max={3} step={0.1} value={s.GAMMA} onChange={(v) => autoSave({ GAMMA: v })} />
        <Toggle label={t.display.uppercase} checked={s.UPPERCASE} onChange={(v) => instantSave({ UPPERCASE: v })} />
        <ColorField label={t.display.textColor} value={s.TCOL} onChange={(v) => autoSave({ TCOL: v })} />
        <Select
          label={t.display.backgroundEffect}
          value={s.BEFF ?? -1}
          options={[
            { value: -1, label: t.display.none },
            ...effects.map((e, i) => ({
              value: i,
              label: humanize(e.name, `Effect ${i + 1}`),
            })),
          ]}
          onChange={(v) => instantSave({ BEFF: v as number })}
        />
        <FormRow>
          <div class="form-group">
            <label>{t.display.colorCorrection}</label>
            <input type="color" value={s.CCORRECTION}
              onInput={(e) => autoSave({ CCORRECTION: (e.target as HTMLInputElement).value })} />
          </div>
          <div class="form-group">
            <label>{t.display.colorTemperature}</label>
            <input type="color" value={s.CTEMP}
              onInput={(e) => autoSave({ CTEMP: (e.target as HTMLInputElement).value })} />
          </div>
        </FormRow>
      </div>
    </Card>
  );
}
