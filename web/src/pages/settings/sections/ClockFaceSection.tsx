import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { Toggle, TextField, ColorField, Select, Card, FormRow } from "../../../components/ui";
import styles from "./sections.module.css";

export function ClockFaceSection() {
  const { t } = useTranslation();
  const { settings, setSetting } = useSettings();
  if (!settings) return null;
  const s = settings;

  return (
    <Card title={t("settingsDisplay.timeDate.title")}>
      <div class={styles.stack}>
        <FormRow>
          <TextField label={t("settingsDisplay.timeDate.timeFormat")} value={s.TFORMAT} onChange={(v) => setSetting({ TFORMAT: v })} />
          <TextField label={t("settingsDisplay.timeDate.dateFormat")} value={s.DFORMAT} onChange={(v) => setSetting({ DFORMAT: v })} />
        </FormRow>
        <Select
          label={t("settingsDisplay.timeDate.timeMode")}
          value={s.TMODE}
          options={[
            { value: 0, label: t("settingsDisplay.timeDate.modePlainText") },
            { value: 1, label: t("settingsDisplay.timeDate.modeCalendar") },
            { value: 2, label: t("settingsDisplay.timeDate.modeCalendarTop") },
            { value: 3, label: t("settingsDisplay.timeDate.modeCalendarAlt") },
            { value: 4, label: t("settingsDisplay.timeDate.modeCalendarAltTop") },
            { value: 5, label: t("settingsDisplay.timeDate.modeBigDigits") },
            { value: 6, label: t("settingsDisplay.timeDate.modeBinary") },
          ]}
          onChange={(v) => setSetting({ TMODE: v as number }, true)}
        />
        <Toggle label={t("settingsDisplay.timeDate.startOnMonday")} checked={s.SOM} onChange={(v) => setSetting({ SOM: v }, true)} />
        <Toggle label={t("settingsDisplay.timeDate.celsius")} checked={s.CEL} onChange={(v) => setSetting({ CEL: v }, true)} />

        <h3 class={styles.subheading}>{t("settingsDisplay.timeDate.colorsHeading")}</h3>
        <FormRow>
          <ColorField label={t("settingsDisplay.timeDate.timeColor")} value={s.TIME_COL} onChange={(v) => setSetting({ TIME_COL: v }, true)} />
          <ColorField label={t("settingsDisplay.timeDate.dateColor")} value={s.DATE_COL} onChange={(v) => setSetting({ DATE_COL: v }, true)} />
        </FormRow>

        {[1, 2, 3, 4].includes(s.TMODE) && (
          <fieldset class={styles.fieldset}>
            <legend class={styles.legend}>{t("settingsDisplay.timeDate.calendarColors")}</legend>
            <FormRow>
              <ColorField label={t("settingsDisplay.timeDate.calHeader")} value={s.CHCOL} onChange={(v) => setSetting({ CHCOL: v }, true)} />
              <ColorField label={t("settingsDisplay.timeDate.calText")} value={s.CTCOL} onChange={(v) => setSetting({ CTCOL: v }, true)} />
            </FormRow>
            <ColorField label={t("settingsDisplay.timeDate.calBody")} value={s.CBCOL} onChange={(v) => setSetting({ CBCOL: v }, true)} />
          </fieldset>
        )}

        <Toggle label={t("settingsDisplay.timeDate.showWeekday")} checked={s.WD} onChange={(v) => setSetting({ WD: v }, true)} />
        {s.WD && (
          <fieldset class={styles.fieldset}>
            <legend class={styles.legend}>{t("settingsDisplay.timeDate.weekdayColors")}</legend>
            <FormRow>
              <ColorField label={t("settingsDisplay.timeDate.weekdayActive")} value={s.WDCA} onChange={(v) => setSetting({ WDCA: v }, true)} />
              <ColorField label={t("settingsDisplay.timeDate.weekdayInactive")} value={s.WDCI} onChange={(v) => setSetting({ WDCI: v }, true)} />
            </FormRow>
          </fieldset>
        )}
      </div>
    </Card>
  );
}
