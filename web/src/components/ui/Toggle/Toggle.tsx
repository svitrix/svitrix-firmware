import { useId } from "preact/hooks";
import styles from "./Toggle.module.css";

export function Toggle({
  label,
  checked,
  onChange,
  helper,
}: {
  label: string;
  checked: boolean;
  onChange: (v: boolean) => void;
  /** Optional caption rendered under the control. */
  helper?: string;
}) {
  const id = useId();
  const helperId = `${id}-helper`;
  return (
    <div class={styles.wrap}>
      <label>
        <input
          type="checkbox"
          checked={checked}
          aria-describedby={helper ? helperId : undefined}
          onChange={(e) => onChange((e.target as HTMLInputElement).checked)}
        />
        {label}
      </label>
      {helper && (
        <p id={helperId} class={styles.helper}>
          {helper}
        </p>
      )}
    </div>
  );
}
