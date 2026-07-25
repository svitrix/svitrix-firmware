import { useId } from "preact/hooks";
import styles from "./Select.module.css";

export function Select({
  label,
  value,
  options,
  onChange,
  autocomplete,
  error,
}: {
  label: string;
  value: string | number;
  options: Array<{ value: string | number; label: string }>;
  onChange: (v: number | string) => void;
  /** Forwarded to the native <select autocomplete> (WCAG 1.3.5). */
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
        <select
          class={styles.select}
          id={id}
          value={value}
          autocomplete={autocomplete}
          aria-invalid={invalid ? "true" : undefined}
          aria-describedby={invalid ? errorId : undefined}
          onChange={(e) => {
            const raw = (e.target as HTMLSelectElement).value;
            const opt = options.find((o) => String(o.value) === raw);
            onChange(opt && typeof opt.value === "number" ? Number(raw) : raw);
          }}
        >
          {options.map((opt) => (
            <option key={opt.value} value={opt.value}>
              {opt.label}
            </option>
          ))}
        </select>
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
