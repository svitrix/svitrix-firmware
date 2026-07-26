import { useTranslation } from "react-i18next";
import { Button } from "../../components/ui";
import styles from "./OfflineBanner.module.css";

/** Top-of-page alert shown when the heartbeat can't reach the device. Does not
 *  disable controls this pass (out of scope) — the banner is the honest signal. */
export function OfflineBanner({
  lastSeen,
  onRetry,
}: {
  lastSeen: number | null;
  onRetry: () => void;
}) {
  const { t } = useTranslation();
  const browserOffline = typeof navigator !== "undefined" && navigator.onLine === false;

  return (
    <div class={styles.banner} role="alert">
      <div class={styles.text}>
        <strong class={styles.title}>{t("deviceState.offlineTitle")}</strong>
        <span class={styles.hint}>
          {browserOffline ? t("deviceState.browserOffline") : t("deviceState.offlineBody")}
          {lastSeen != null && (
            <> {t("deviceState.lastSeen", { time: new Date(lastSeen).toLocaleTimeString() })}</>
          )}
        </span>
      </div>
      <Button onClick={onRetry}>{t("deviceState.retry")}</Button>
    </div>
  );
}
