import { signal } from "@preact/signals";
import { useTranslation } from "react-i18next";

/** Options for {@link toast}. `error` toasts announce assertively and never
 *  auto-dismiss (they must be dismissed manually). */
export type ToastOptions = { error?: boolean; ms?: number };

const toastMsg = signal("");
const toastError = signal(false);
let timer: ReturnType<typeof setTimeout>;

/**
 * Show a transient toast message.
 *
 * Signatures (all still valid):
 *   toast("Saved")                       // success, auto-dismiss 2s
 *   toast("Saved", 4000)                 // success, custom duration
 *   toast("Failed", { error: true })     // error, assertive, no auto-dismiss
 *   toast("Note", { ms: 6000 })          // success, custom duration
 */
export function toast(msg: string, opts?: number | ToastOptions) {
  const o: ToastOptions = typeof opts === "number" ? { ms: opts } : (opts ?? {});
  const isError = o.error ?? false;
  const ms = o.ms ?? 2000;

  clearTimeout(timer);
  toastMsg.value = msg;
  toastError.value = isError;

  // Errors persist until manually dismissed so they are not missed.
  if (!isError) {
    timer = setTimeout(() => (toastMsg.value = ""), ms);
  }
}

function dismiss() {
  clearTimeout(timer);
  toastMsg.value = "";
}

export function ToastContainer() {
  const { t } = useTranslation();
  const msg = toastMsg.value;
  const isError = toastError.value;

  // The live region is ALWAYS mounted so assistive tech observes text changes.
  // Only the visible pill is conditional; the region node itself never unmounts.
  return (
    <div
      role={isError ? "alert" : "status"}
      aria-live={isError ? "assertive" : "polite"}
      aria-atomic="true"
    >
      {msg && (
        <div class="toast">
          <span>{msg}</span>
          {isError && (
            <button
              type="button"
              onClick={dismiss}
              aria-label={t("common.close")}
              style={{
                marginLeft: "12px",
                background: "none",
                border: "none",
                color: "inherit",
                font: "inherit",
                lineHeight: 1,
                cursor: "pointer",
                padding: 0,
              }}
            >
              <span aria-hidden="true">&times;</span>
            </button>
          )}
        </div>
      )}
    </div>
  );
}
