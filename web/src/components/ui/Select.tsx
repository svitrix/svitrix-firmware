export function Select({
  label,
  value,
  options,
  onChange,
}: {
  label: string;
  value: string | number;
  options: Array<{ value: string | number; label: string }>;
  onChange: (v: number | string) => void;
}) {
  const id = label.toLowerCase().replace(/\s+/g, "-");
  return (
    <div class="form-group">
      <label htmlFor={id}>{label}</label>
      <select
        id={id}
        value={value}
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
  );
}
