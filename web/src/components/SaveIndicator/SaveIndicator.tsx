import { useTranslation } from "react-i18next";
import { useSettings } from "../../context/SettingsContext";
import styles from "./SaveIndicator.module.css";

export function SaveIndicator() {
  const { saveState } = useSettings();
  const { t } = useTranslation();

  let text = "";
  let stateClass = "";
  if (saveState === "saving") {
    text = t("common.saving");
  } else if (saveState === "saved") {
    text = t("common.saved");
    stateClass = styles.saved;
  } else if (saveState === "error") {
    text = t("common.notSaved");
    stateClass = styles.error;
  }

  return (
    <span role="status" aria-live="polite" class={`${styles.indicator} ${stateClass}`}>
      {text}
    </span>
  );
}
