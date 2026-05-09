export function Slider({
  label,
  min,
  max,
  value,
  onChange,
  step,
  unit,
  compact,
}: {
  label: string;
  min: number;
  max: number;
  value: number;
  onChange: (v: number) => void;
  step?: number;
  unit?: string;
  compact?: boolean;
}) {
  const id = label.toLowerCase().replace(/\s+/g, "-");
  const style = compact ? { maxWidth: "105px", minWidth: "85px" } : undefined;
  return (
    <div class="form-group" style={style}>
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
