import { useEffect, useRef } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { Button } from "../Button";
import styles from "./Dialog.module.css";

/** Glass confirm modal — focus-trapped-ish (default focus on Cancel = safe),
 *  Esc + backdrop cancel. Replaces native confirm() app-wide. */
export function ConfirmDialog({
  open,
  title,
  body,
  confirmLabel,
  danger,
  onConfirm,
  onCancel,
}: {
  open: boolean;
  title: string;
  body?: string;
  confirmLabel: string;
  danger?: boolean;
  onConfirm: () => void;
  onCancel: () => void;
}) {
  const { t } = useTranslation();
  const cancelRef = useRef<HTMLButtonElement>(null);

  useEffect(() => {
    if (!open) return;
    cancelRef.current?.focus();
    const onKey = (e: KeyboardEvent) => e.key === "Escape" && onCancel();
    document.addEventListener("keydown", onKey);
    return () => document.removeEventListener("keydown", onKey);
  }, [open]);

  if (!open) return null;
  return (
    <div class={styles.backdrop} role="dialog" aria-modal="true" onClick={onCancel}>
      <div class={styles.dialog} onClick={(e) => e.stopPropagation()}>
        <div class={styles.title}>{title}</div>
        {body && <div class={styles.body}>{body}</div>}
        <div class={styles.row}>
          <button ref={cancelRef} onClick={onCancel}>{t("common.cancel")}</button>
          <Button variant={danger ? "danger" : "primary"} onClick={onConfirm}>{confirmLabel}</Button>
        </div>
      </div>
    </div>
  );
}
