import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, Slider, ColorField, Select, Card, FormRow } from "../../../components/ui";
import styles from "./sections.module.css";

export function DisplaySection() {
  const { t } = useTranslation();
  const { settings, effects, setSetting } = useSettings();
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
    <Card title={t("settingsDisplay.display.title")}>
      <div class={styles.stack}>
        <Toggle label={t("settingsDisplay.display.matrixPower")} helper={t("settingsDisplay.display.matrixPowerHelper")} checked={s.MATP} onChange={(v) => setSetting({ MATP: v }, true)} />
        <Toggle label={t("settingsDisplay.display.autoBrightness")} checked={s.ABRI} onChange={(v) => setSetting({ ABRI: v }, true)} />
        <Slider label={t("settingsDisplay.display.brightness")} min={5} max={255} value={s.BRI} onChange={(v) => setSetting({ BRI: v })} />
        <Slider label={t("settingsDisplay.display.gamma")} helper={t("settingsDisplay.display.gammaHelper")} min={0.5} max={3} step={0.1} value={s.GAMMA} onChange={(v) => setSetting({ GAMMA: v })} />
        <Toggle label={t("settingsDisplay.display.uppercase")} checked={s.UPPERCASE} onChange={(v) => setSetting({ UPPERCASE: v }, true)} />
        <ColorField label={t("settingsDisplay.display.textColor")} value={s.TCOL} onChange={(v) => setSetting({ TCOL: v }, true)} />
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
          onChange={(v) => setSetting({ BEFF: v as number }, true)}
        />
        <fieldset class={styles.fieldset}>
          <legend class={styles.legend}>{t("settingsDisplay.display.whiteBalance")}</legend>
          <FormRow>
            <ColorField
              hex
              label={t("settingsDisplay.display.colorCorrection")}
              helper={t("settingsDisplay.display.colorCorrectionHelper")}
              value={s.CCORRECTION}
              onChange={(v) => setSetting({ CCORRECTION: v }, true)}
            />
            <ColorField
              hex
              label={t("settingsDisplay.display.colorTemperature")}
              helper={t("settingsDisplay.display.colorTemperatureHelper")}
              value={s.CTEMP}
              onChange={(v) => setSetting({ CTEMP: v }, true)}
            />
          </FormRow>
        </fieldset>
      </div>
    </Card>
  );
}
