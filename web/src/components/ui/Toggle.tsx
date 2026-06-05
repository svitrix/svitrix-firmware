export function Toggle({
  label,
  checked,
  onChange,
  compact,
}: {
  label?: string;
  checked: boolean;
  onChange: (v: boolean) => void;
  compact?: boolean;
}) {
  return (
    <label style={compact ? { display: "flex", alignItems: "center" } : undefined}>
      <input
        type="checkbox"
        checked={checked}
        onChange={(e) => onChange((e.target as HTMLInputElement).checked)}
      />
      {label}
    </label>
  );
}
