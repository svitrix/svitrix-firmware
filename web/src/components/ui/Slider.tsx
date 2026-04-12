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
  return (
    <div class="form-group">
      <label>
        {label}: {value}{unit ?? ""}
        <input
          type="range"
          min={min}
          max={max}
          step={step}
          value={value}
          onInput={(e) => onChange(+(e.target as HTMLInputElement).value)}
        />
      </label>
    </div>
  );
}
