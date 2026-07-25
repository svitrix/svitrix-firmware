import { useEffect, useId, useRef, useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { Button } from "../Button";
import styles from "./Dialog.module.css";

const FOCUSABLE =
  'a[href],button:not([disabled]),input:not([disabled]),select:not([disabled]),textarea:not([disabled]),[tabindex]:not([tabindex="-1"])';

/** Keep Tab / Shift+Tab focus cycling inside `container` (wraps at edges). */
function trapFocus(e: KeyboardEvent, container: HTMLElement | null) {
  if (!container) return;
  const nodes = Array.from(
    container.querySelectorAll<HTMLElement>(FOCUSABLE),
  ).filter((n) => n.offsetParent !== null || n === document.activeElement);
  if (nodes.length === 0) return;
  const first = nodes[0];
  const last = nodes[nodes.length - 1];
  const active = document.activeElement;
  if (e.shiftKey) {
    if (active === first || !container.contains(active)) {
      e.preventDefault();
      last.focus();
    }
  } else if (active === last || !container.contains(active)) {
    e.preventDefault();
    first.focus();
  }
}

/** Glass single-input modal — replaces native prompt() (blocked in some iOS
 *  webviews). Enter submits, Esc/backdrop cancels, empty is rejected. */
export function PromptDialog({
  open,
  title,
  label,
  initial = "",
  submitLabel,
  onSubmit,
  onCancel,
}: {
  open: boolean;
  title: string;
  label?: string;
  initial?: string;
  submitLabel: string;
  onSubmit: (value: string) => void;
  onCancel: () => void;
}) {
  const { t } = useTranslation();
  const titleId = useId();
  const [val, setVal] = useState(initial);
  const inputRef = useRef<HTMLInputElement>(null);
  const dialogRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (!open) return;
    const prevFocused = document.activeElement as HTMLElement | null;
    setVal(initial);
    const id = setTimeout(() => inputRef.current?.focus(), 0);
    const onKey = (e: KeyboardEvent) => {
      if (e.key === "Escape") {
        onCancel();
        return;
      }
      if (e.key === "Tab") trapFocus(e, dialogRef.current);
    };
    document.addEventListener("keydown", onKey);
    return () => {
      clearTimeout(id);
      document.removeEventListener("keydown", onKey);
      prevFocused?.focus?.();
    };
  }, [open]);

  if (!open) return null;
  const submit = () => {
    const v = val.trim();
    if (v) onSubmit(v);
  };

  return (
    <div class={styles.backdrop} onClick={onCancel}>
      <div
        ref={dialogRef}
        class={styles.dialog}
        role="dialog"
        aria-modal="true"
        aria-labelledby={titleId}
        onClick={(e) => e.stopPropagation()}
      >
        <div id={titleId} class={styles.title}>{title}</div>
        <div class={styles.field}>
          <input
            ref={inputRef}
            value={val}
            placeholder={label}
            aria-label={label ?? title}
            onInput={(e) => setVal((e.target as HTMLInputElement).value)}
            onKeyDown={(e) => e.key === "Enter" && submit()}
          />
        </div>
        <div class={styles.row}>
          <button onClick={onCancel}>{t("common.cancel")}</button>
          <Button variant="primary" onClick={submit}>{submitLabel}</Button>
        </div>
      </div>
    </div>
  );
}
