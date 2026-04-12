function intToHex(n: number): string {
  return "#" + (n & 0xffffff).toString(16).padStart(6, "0");
}

function hexToInt(hex: string): number {
  return parseInt(hex.replace("#", ""), 16);
}

export function ColorField({
  label,
  value,
  onChange,
}: {
  label: string;
  value: number;
  onChange: (v: number) => void;
}) {
  return (
    <div class="form-group">
      <label>
        <input
          type="color"
          value={intToHex(value)}
          onInput={(e) => onChange(hexToInt((e.target as HTMLInputElement).value))}
        />
        {label}
      </label>
    </div>
  );
}
