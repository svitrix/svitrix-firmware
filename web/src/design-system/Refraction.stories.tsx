import type { Meta } from "@storybook/preact-vite";
import type { ComponentChildren } from "preact";
import { useMemo } from "preact/hooks";

/**
 * Real Liquid Glass refraction (per kube.io deconstruction).
 * Chromium-only: SVG filter as backdrop-filter is not supported in Safari/Firefox
 * — there the panel falls back to plain blur. See web/DESIGN.md §3.3.
 *
 * NOTE: the displacement math is sound (rounded-box SDF + squircle surface slope),
 * but exact `scale`/sign/bezel want a visual tuning pass in Chrome — I can't render
 * here. Tweak SCALE and BEZEL below to taste.
 */

const meta: Meta = {
  title: "Design System/Refraction",
  parameters: { layout: "fullscreen" },
};
export default meta;

/* Build a bezel displacement map: R=X, G=Y, encoded 128 + comp*127. */
function makeDisplacementMap(w: number, h: number, radius: number, bezel: number): string {
  const canvas = document.createElement("canvas");
  canvas.width = w;
  canvas.height = h;
  const ctx = canvas.getContext("2d")!;
  const img = ctx.createImageData(w, h);
  const data = img.data;
  const hw = w / 2;
  const hh = h / 2;
  const r = Math.min(radius, hw, hh);

  // Signed distance to a rounded box (negative inside).
  const sd = (px: number, py: number): number => {
    const qx = Math.abs(px) - (hw - r);
    const qy = Math.abs(py) - (hh - r);
    const ox = Math.max(qx, 0);
    const oy = Math.max(qy, 0);
    return Math.hypot(ox, oy) + Math.min(Math.max(qx, qy), 0) - r;
  };

  // Squircle surface slope f(x)=⁴√(1-(1-x)⁴), stronger refraction toward the rim.
  const f = (t: number) => Math.pow(1 - Math.pow(1 - Math.min(Math.max(t, 0), 1), 4), 0.25);
  const slope = (x: number) => (f(x + 0.001) - f(x - 0.001)) / 0.002;

  for (let y = 0; y < h; y++) {
    for (let x = 0; x < w; x++) {
      const px = x - hw + 0.5;
      const py = y - hh + 0.5;
      const d = sd(px, py);
      const edge = -d; // distance inside from the boundary
      let rr = 128;
      let gg = 128;
      if (d < 0 && edge < bezel) {
        // Inward normal = -gradient(SDF).
        const nx = sd(px + 1, py) - sd(px - 1, py);
        const ny = sd(px, py + 1) - sd(px, py - 1);
        const len = Math.hypot(nx, ny) || 1;
        const inx = -nx / len;
        const iny = -ny / len;
        const t = 1 - edge / bezel; // 0 at inner edge → 1 at boundary
        const mag = Math.min(slope(t) / 3, 1);
        rr = 128 + inx * mag * 127;
        gg = 128 + iny * mag * 127;
      }
      const i = (y * w + x) * 4;
      data[i] = rr;
      data[i + 1] = gg;
      data[i + 2] = 128;
      data[i + 3] = 255;
    }
  }
  ctx.putImageData(img, 0, 0);
  return canvas.toDataURL();
}

/* Vivid backdrop with legible content behind, so refraction is visible. */
function Scene({ children }: { children: ComponentChildren }) {
  return (
    <div
      style={{
        minHeight: "100vh",
        display: "grid",
        placeItems: "center",
        gap: "24px",
        padding: "40px",
        background:
          "radial-gradient(70% 60% at 15% 10%, rgba(94,92,230,0.85), transparent 60%)," +
          "radial-gradient(60% 55% at 90% 15%, rgba(48,209,209,0.8), transparent 55%)," +
          "radial-gradient(80% 70% at 75% 100%, rgba(255,110,90,0.8), transparent 55%)," +
          "#0b0d13",
        position: "relative",
      }}
    >
      {/* grid of text behind the glass — refraction bends these lines at the rim */}
      <div
        style={{
          position: "absolute",
          inset: 0,
          fontSize: "22px",
          fontWeight: 700,
          lineHeight: "34px",
          letterSpacing: "2px",
          color: "rgba(255,255,255,0.14)",
          overflow: "hidden",
          userSelect: "none",
        }}
      >
        {Array.from({ length: 30 }, () => "SVITRIX · LIQUID · GLASS · ").join("")}
      </div>
      {children}
    </div>
  );
}

const W = 380;
const H = 240;
const RADIUS = 30;
const BEZEL = 26;
const SCALE = 42; // px of max displacement — tune in Chrome

export const RefractingPanel = {
  render: () => {
    const id = "lg-refraction";
    const map = useMemo(() => makeDisplacementMap(W, H, RADIUS, BEZEL), []);
    return (
      <Scene>
        <svg width="0" height="0" style={{ position: "absolute" }} aria-hidden="true">
          <filter id={id} color-interpolation-filters="sRGB">
            <feImage href={map} x="0" y="0" width={W} height={H} result="map" />
            <feDisplacementMap
              in="SourceGraphic"
              in2="map"
              scale={SCALE}
              xChannelSelector="R"
              yChannelSelector="G"
            />
          </filter>
        </svg>

        <div
          style={{
            position: "relative",
            width: `${W}px`,
            height: `${H}px`,
            borderRadius: `${RADIUS}px`,
            // Chromium: refraction. Safari/FF: url() ignored → keep a blur fallback.
            backdropFilter: `url(#${id})`,
            WebkitBackdropFilter: `url(#${id})`,
            background: "color-mix(in srgb, var(--surface-tint) 26%, transparent)",
            boxShadow:
              "0 1px 1px rgba(0,0,0,0.2), 0 20px 50px rgba(0,0,0,0.45)",
            display: "grid",
            placeItems: "center",
            color: "#fff",
            fontWeight: 650,
            fontSize: "17px",
          }}
        >
          {/* curved specular rim */}
          <span
            style={{
              position: "absolute",
              inset: 0,
              borderRadius: "inherit",
              padding: "1px",
              background:
                "linear-gradient(135deg, rgba(255,255,255,0.8), rgba(255,255,255,0.1) 30%, transparent 55%, rgba(255,255,255,0.3))",
              WebkitMask:
                "linear-gradient(#000 0 0) content-box, linear-gradient(#000 0 0)",
              WebkitMaskComposite: "xor",
              maskComposite: "exclude",
              pointerEvents: "none",
            }}
          />
          Liquid Glass · refraction
        </div>

        <p style={{ color: "rgba(255,255,255,0.7)", fontSize: "13px", maxWidth: `${W}px`, textAlign: "center" }}>
          Refraction renders in <b>Chromium only</b>. In Safari/Firefox this panel
          shows the blur fallback (see DESIGN.md §3.3).
        </p>
      </Scene>
    );
  },
};

/* Corrected TextField: two-layer anatomy — outer glass shell + inner stabilized
   plate that keeps the value/placeholder legible over any scene. */
export const LiquidGlassTextField = {
  render: () => (
    <Scene>
      <div style={{ width: "320px", display: "flex", flexDirection: "column", gap: "6px" }}>
        <label style={{ color: "rgba(255,255,255,0.85)", fontSize: "13px", fontWeight: 600 }}>
          Device name
        </label>
        <div
          style={{
            position: "relative",
            borderRadius: "14px",
            padding: "1px",
            // outer shell
            background:
              "linear-gradient(135deg, rgba(255,255,255,0.5), rgba(255,255,255,0.06) 40%, transparent 60%, rgba(255,255,255,0.2))",
          }}
        >
          <input
            type="text"
            value="Svitrix"
            style={{
              width: "100%",
              border: "none",
              borderRadius: "13px",
              padding: "12px 16px",
              fontSize: "15px",
              color: "#fff",
              // inner stabilized plate — dense enough for legibility over the scene
              background: "rgba(20,22,32,0.62)",
              backdropFilter: "blur(20px) saturate(160%)",
              WebkitBackdropFilter: "blur(20px) saturate(160%)",
              boxShadow: "inset 0 1px 2px rgba(0,0,0,0.28)",
              outline: "none",
            }}
          />
        </div>
        <span style={{ color: "rgba(255,255,255,0.55)", fontSize: "12px" }}>
          Outer glass shell + inner stabilized plate (readable on any background).
        </span>
      </div>
    </Scene>
  ),
};
