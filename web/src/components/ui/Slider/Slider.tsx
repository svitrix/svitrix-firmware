import { useId } from "preact/hooks";
import styles from "./Slider.module.css";

export function Slider({
  label,
  min,
  max,
  value,
  onChange,
  step,
  unit,
  helper,
}: {
  label: string;
  min: number;
  max: number;
  value: number;
  onChange: (v: number) => void;
  step?: number;
  unit?: string;
  /** Optional caption rendered under the control. */
  helper?: string;
}) {
  const id = useId();
  const helperId = `${id}-helper`;
  // Accent-fill percentage — drives the recessed track gradient cross-browser.
  const span = max - min;
  const pct = span > 0 ? ((value - min) / span) * 100 : 0;
  return (
    <div class="form-group">
      <label htmlFor={id}>
        {label}: {value}{unit ?? ""}
      </label>
      <input
        id={id}
        class={styles.range}
        type="range"
        min={min}
        max={max}
        step={step}
        value={value}
        style={{ "--pct": `${pct}%` } as any}
        aria-describedby={helper ? helperId : undefined}
        onInput={(e) => onChange(+(e.target as HTMLInputElement).value)}
      />
      {helper && (
        <p id={helperId} class={styles.helper}>
          {helper}
        </p>
      )}
    </div>
  );
}
