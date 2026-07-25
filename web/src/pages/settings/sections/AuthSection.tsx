import { useTranslation } from "react-i18next";
import { useSettings } from "../../../context/SettingsContext";
import { TextField, Card, FormRow } from "../../../components/ui";
import styles from "./sections.module.css";

export function AuthSection() {
  const { config, updateConfig } = useSettings();
  const { t } = useTranslation();
  if (!config) return null;

  return (
    <Card title={t("settings.auth.title")}>
      <FormRow>
        <TextField label={t("settings.auth.username")} value={config["Auth Username"] || ""} onChange={(v) => updateConfig("Auth Username", v)} autocomplete="username" />
        <TextField label={t("settings.auth.password")} value={config["Auth Password"] || ""} onChange={(v) => updateConfig("Auth Password", v)} type="password" autocomplete="current-password" />
      </FormRow>
      <p class={styles.hintMt}>
        {t("settings.auth.hint")}
      </p>
    </Card>
  );
}
