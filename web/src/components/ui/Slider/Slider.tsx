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
}: {
  label: string;
  min: number;
  max: number;
  value: number;
  onChange: (v: number) => void;
  step?: number;
  unit?: string;
}) {
  const id = useId();
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
        onInput={(e) => onChange(+(e.target as HTMLInputElement).value)}
      />
    </div>
  );
}
