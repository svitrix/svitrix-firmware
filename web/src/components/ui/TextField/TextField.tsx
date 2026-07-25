import { useId } from "preact/hooks";
import styles from "./TextField.module.css";

export function TextField({
  label,
  value,
  onChange,
  type = "text",
  placeholder,
  autocomplete,
  error,
}: {
  label: string;
  value: string | number;
  onChange: (v: string) => void;
  type?: string;
  placeholder?: string;
  /** Forwarded to the native <input autocomplete> (WCAG 1.3.5). */
  autocomplete?: string;
  /** When non-empty, marks the field invalid and shows a described message. */
  error?: string;
}) {
  const id = useId();
  const errorId = `${id}-error`;
  const invalid = !!error;
  return (
    <div class="form-group">
      <label htmlFor={id}>{label}</label>
      <div class={styles.shell}>
        <input
          class={styles.input}
          id={id}
          type={type}
          value={value}
          placeholder={placeholder}
          autocomplete={autocomplete}
          aria-invalid={invalid ? "true" : undefined}
          aria-describedby={invalid ? errorId : undefined}
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
    </div>
  );
}
