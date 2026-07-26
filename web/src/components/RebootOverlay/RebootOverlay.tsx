import { useEffect, useRef, useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { ping } from "../../api/client";
import { Button } from "../ui";
import styles from "./RebootOverlay.module.css";

type Phase = "working" | "online" | "failed" | "info";

export interface RebootOverlayProps {
  /** Controls visibility. */
  open: boolean;
  /** Called after the device is confirmed back online (wire to `reload()`),
   *  and when the user dismisses. The caller is responsible for closing. */
  onBack: () => void;
  /** Optional heading override for the working state. */
  title?: string;
  /** "reconnect" polls until the device returns; "info" is an informational
   *  terminal state (Wi-Fi hand-off) that never auto-reloads. */
  mode?: "reconnect" | "info";
  /** Body copy for the "info" mode. */
  infoMessage?: string;
}

const POLL_START = 2000; // first probe after ~2s (device is still rebooting)
const POLL_MAX = 3500; // backoff ceiling
const POLL_STEP = 500;
const TIMEOUT_MS = 45000; // give up after ~45s → failure state
const ONLINE_HOLD = 1200; // show "Back online ✓" briefly before handing back

export function RebootOverlay({ open, onBack, title, mode = "reconnect", infoMessage }: RebootOverlayProps) {
  const { t } = useTranslation();
  const [phase, setPhase] = useState<Phase>("working");

  // active flag guards every async continuation against unmount/close.
  const activeRef = useRef(false);
  const pollTimer = useRef<ReturnType<typeof setTimeout>>();
  const holdTimer = useRef<ReturnType<typeof setTimeout>>();
  const onBackRef = useRef(onBack);
  onBackRef.current = onBack;

  function clearTimers() {
    clearTimeout(pollTimer.current);
    clearTimeout(holdTimer.current);
  }

  function startPolling() {
    clearTimers();
    activeRef.current = true;
    setPhase("working");
    const startedAt = Date.now();
    let delay = POLL_START;

    const tick = async () => {
      if (!activeRef.current) return;
      try {
        await ping();
        if (!activeRef.current) return;
        setPhase("online");
        holdTimer.current = setTimeout(() => {
          if (!activeRef.current) return;
          activeRef.current = false;
          onBackRef.current();
        }, ONLINE_HOLD);
      } catch {
        if (!activeRef.current) return;
        if (Date.now() - startedAt > TIMEOUT_MS) {
          activeRef.current = false;
          setPhase("failed");
          return;
        }
        delay = Math.min(delay + POLL_STEP, POLL_MAX);
        pollTimer.current = setTimeout(tick, delay);
      }
    };

    pollTimer.current = setTimeout(tick, delay);
  }

  useEffect(() => {
    if (!open) {
      activeRef.current = false;
      clearTimers();
      return;
    }
    if (mode === "info") {
      setPhase("info");
    } else {
      startPolling();
    }
    return () => {
      activeRef.current = false;
      clearTimers();
    };
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [open, mode]);

  if (!open) return null;

  const working = phase === "working";
  const heading =
    phase === "online"
      ? t("deviceState.backOnline")
      : phase === "failed"
        ? t("deviceState.unreachableTitle")
        : phase === "info"
          ? t("deviceState.lookAtClockTitle")
          : title || t("deviceState.restarting");

  return (
    <div class={styles.backdrop}>
      <div
        class={styles.dialog}
        role="dialog"
        aria-modal="true"
        aria-busy={working}
        aria-live="polite"
      >
        {working && <div class={styles.spinner} aria-hidden="true" />}
        <h2 class={styles.title}>{heading}</h2>

        {working && <p class={styles.body}>{t("deviceState.restartingHint")}</p>}
        {phase === "failed" && <p class={styles.body}>{t("deviceState.unreachableBody")}</p>}
        {phase === "info" && (
          <p class={styles.body}>{infoMessage || t("deviceState.lookAtClockBody")}</p>
        )}

        {(phase === "failed" || phase === "info") && (
          <div class={styles.actions}>
            <button type="button" onClick={() => onBackRef.current()}>
              {t("deviceState.dismiss")}
            </button>
            <Button variant="primary" onClick={startPolling}>
              {t("deviceState.retry")}
            </Button>
          </div>
        )}
      </div>
    </div>
  );
}
