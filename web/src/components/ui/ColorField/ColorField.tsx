import { useId } from "preact/hooks";
import styles from "./ColorField.module.css";

function intToHex(n: number): string {
  return "#" + (n & 0xffffff).toString(16).padStart(6, "0");
}

function hexToInt(hex: string): number {
  return parseInt(hex.replace("#", ""), 16);
}

type Base = {
  label: string;
  /** Optional caption rendered under the control. */
  helper?: string;
  /** When non-empty, marks the field invalid and shows a described message. */
  error?: string;
};

/** Default contract: colour stored as a 24-bit integer (number in, number out). */
type NumberMode = Base & {
  value: number;
  onChange: (v: number) => void;
  hex?: false;
};

/** Opt-in contract: colour stored as a "#RRGGBB" string (e.g. CCORRECTION/CTEMP). */
type HexMode = Base & {
  value: string;
  onChange: (v: string) => void;
  hex: true;
};

export function ColorField(props: NumberMode | HexMode) {
  const { label, helper, error } = props;
  const id = useId();
  const errorId = `${id}-error`;
  const helperId = `${id}-helper`;
  const invalid = !!error;
  const describedBy =
    [invalid ? errorId : null, helper ? helperId : null].filter(Boolean).join(" ") || undefined;

  // The native <input type="color"> always speaks "#rrggbb"; adapt at the edge.
  const hexValue = props.hex ? props.value : intToHex(props.value);
  const emit = (rawHex: string) => {
    if (props.hex) props.onChange(rawHex);
    else props.onChange(hexToInt(rawHex));
  };

  return (
    <div class="form-group">
      <label htmlFor={id}>{label}</label>
      <div class={styles.shell}>
        <input
          class={styles.swatch}
          id={id}
          type="color"
          value={hexValue}
          aria-invalid={invalid ? "true" : undefined}
          aria-describedby={describedBy}
          onInput={(e) => emit((e.target as HTMLInputElement).value)}
        />
      </div>
      {invalid && (
        <p id={errorId} class={styles.error}>
          <span class={styles.errorIcon} aria-hidden="true">
            ⚠
          </span>
          {error}
        </p>
      )}
      {helper && (
        <p id={helperId} class={styles.helper}>
          {helper}
        </p>
      )}
    </div>
  );
}
