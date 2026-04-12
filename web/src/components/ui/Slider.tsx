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
  const id = label.toLowerCase().replace(/\s+/g, "-");
  return (
    <div class="form-group">
      <label htmlFor={id}>
        {label}: {value}{unit ?? ""}
      </label>
      <input
        id={id}
        type="range"
        min={min}
        max={max}
        step={step}
        value={value}
        onInput={(e) => onChange(+(e.target as HTMLInputElement).value)}
      />
    </div>
  );
}
