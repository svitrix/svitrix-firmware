import { useId } from "preact/hooks";
import styles from "./TimeField.module.css";

export function TimeField({
  label,
  value,
  onChange,
  helper,
  error,
}: {
  label: string;
  /** Time string in "HH:MM" (24-hour) form, as the native <input type="time">. */
  value: string;
  onChange: (v: string) => void;
  /** Optional caption rendered under the control. */
  helper?: string;
  /** When non-empty, marks the field invalid and shows a described message. */
  error?: string;
}) {
  const id = useId();
  const errorId = `${id}-error`;
  const helperId = `${id}-helper`;
  const invalid = !!error;
  const describedBy =
    [invalid ? errorId : null, helper ? helperId : null].filter(Boolean).join(" ") || undefined;
  return (
    <div class="form-group">
      <label htmlFor={id}>{label}</label>
      <div class={styles.shell}>
        <input
          class={styles.input}
          id={id}
          type="time"
          value={value}
          aria-invalid={invalid ? "true" : undefined}
          aria-describedby={describedBy}
          onInput={(e) => onChange((e.target as HTMLInputElement).value)}
        />
      </div>
      {invalid && (
        <p id={errorId} class={styles.error}>
          <span class={styles.errorIcon} aria-hidden="true">
            ⚠
          </span>
          {error}
        </p>
      )}
      {helper && (
        <p id={helperId} class={styles.helper}>
          {helper}
        </p>
      )}
    </div>
  );
}
