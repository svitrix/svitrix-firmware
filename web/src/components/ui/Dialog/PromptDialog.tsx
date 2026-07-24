import { useEffect, useRef, useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { Button } from "../Button";
import styles from "./Dialog.module.css";

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
  const [val, setVal] = useState(initial);
  const inputRef = useRef<HTMLInputElement>(null);

  useEffect(() => {
    if (!open) return;
    setVal(initial);
    const id = setTimeout(() => inputRef.current?.focus(), 0);
    const onKey = (e: KeyboardEvent) => e.key === "Escape" && onCancel();
    document.addEventListener("keydown", onKey);
    return () => {
      clearTimeout(id);
      document.removeEventListener("keydown", onKey);
    };
  }, [open]);

  if (!open) return null;
  const submit = () => {
    const v = val.trim();
    if (v) onSubmit(v);
  };

  return (
    <div class={styles.backdrop} role="dialog" aria-modal="true" onClick={onCancel}>
      <div class={styles.dialog} onClick={(e) => e.stopPropagation()}>
        <div class={styles.title}>{title}</div>
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
