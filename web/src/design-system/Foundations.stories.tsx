import type { Meta } from "@storybook/preact-vite";

const meta: Meta = {
  title: "Design System/Foundations",
  parameters: { layout: "fullscreen" },
};
export default meta;

/* ---- Colors -------------------------------------------------------------- */

function Swatch({ name, varName }: { name: string; varName: string }) {
  return (
    <div style={{ display: "flex", flexDirection: "column", gap: "6px" }}>
      <div
        style={{
          width: "88px",
          height: "56px",
          borderRadius: "var(--r-control)",
          background: `var(${varName})`,
          border: "0.5px solid var(--separator)",
          boxShadow: "inset 0 1px 0 rgba(255,255,255,0.15)",
        }}
      />
      <div style={{ fontSize: "12px", color: "var(--label)" }}>{name}</div>
      <code style={{ fontSize: "11px", color: "var(--label-tertiary)" }}>
        {varName}
      </code>
    </div>
  );
}

const row = { display: "flex", gap: "16px", flexWrap: "wrap" as const };

export const Colors = {
  render: () => (
    <div style={{ display: "flex", flexDirection: "column", gap: "24px" }}>
      <div>
        <h3 style={{ color: "var(--label)", marginBottom: "12px" }}>Accents</h3>
        <div style={row}>
          <Swatch name="Accent" varName="--accent" />
          <Swatch name="Accent pressed" varName="--accent-pressed" />
          <Swatch name="Blue" varName="--blue" />
          <Swatch name="Green" varName="--green" />
          <Swatch name="Red" varName="--red" />
          <Swatch name="Orange" varName="--orange" />
        </div>
      </div>
      <div>
        <h3 style={{ color: "var(--label)", marginBottom: "12px" }}>
          Neutrals &amp; fills
        </h3>
        <div style={row}>
          <Swatch name="Surface tint" varName="--surface-tint" />
          <Swatch name="Fill" varName="--fill" />
          <Swatch name="Fill secondary" varName="--fill-secondary" />
          <Swatch name="Separator" varName="--separator" />
        </div>
      </div>
    </div>
  ),
};

/* ---- Typography ---------------------------------------------------------- */

const styles: Array<[string, string, number, string]> = [
  ["Large Title", "28px", 700, "-0.02em"],
  ["Title", "20px", 650, "-0.01em"],
  ["Headline", "15px", 600, "0"],
  ["Body", "15px", 400, "0"],
  ["Subhead", "13px", 400, "0"],
  ["Footnote", "12px", 400, "0"],
];

export const Typography = {
  render: () => (
    <div style={{ display: "flex", flexDirection: "column", gap: "16px" }}>
      {styles.map(([name, size, weight, tracking]) => (
        <div
          style={{
            fontSize: size,
            fontWeight: weight,
            letterSpacing: tracking,
            color: "var(--label)",
          }}
        >
          {name} — Svitrix 12:34
        </div>
      ))}
    </div>
  ),
};

/* ---- Materials ----------------------------------------------------------- */

function Glass({ label, blur }: { label: string; blur: string }) {
  return (
    <div
      style={{
        flex: "1 1 160px",
        minHeight: "120px",
        padding: "20px",
        borderRadius: "var(--r-card)",
        border: "0.5px solid var(--separator)",
        background: "color-mix(in srgb, var(--surface-tint) 52%, transparent)",
        backdropFilter: `blur(${blur}) saturate(180%)`,
        WebkitBackdropFilter: `blur(${blur}) saturate(180%)`,
        boxShadow:
          "inset 0 1px 0 rgba(255,255,255,0.18), 0 8px 24px rgba(0,0,0,0.28)",
        color: "var(--label)",
        fontWeight: 600,
      }}
    >
      {label}
    </div>
  );
}

export const Materials = {
  render: () => (
    <div
      style={{
        position: "relative",
        padding: "24px",
        borderRadius: "var(--r-card)",
        background:
          "radial-gradient(120% 120% at 20% 0%, var(--accent), transparent 55%), radial-gradient(120% 120% at 90% 100%, var(--blue), transparent 55%)",
      }}
    >
      <div style={{ display: "flex", gap: "16px", flexWrap: "wrap" }}>
        <Glass label="Thin · 30px" blur="30px" />
        <Glass label="Regular · 40px" blur="40px" />
        <Glass label="Thick · 50px" blur="50px" />
      </div>
    </div>
  ),
};

/* ---- Toast --------------------------------------------------------------- */

export const Toast = {
  render: () => (
    <div style={{ position: "relative", height: "120px" }}>
      <div class="toast" style={{ position: "static", transform: "none" }}>
        Settings saved
      </div>
    </div>
  ),
};
