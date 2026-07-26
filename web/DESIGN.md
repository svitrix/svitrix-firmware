# Svitrix Web UI — Design Spec (Liquid Glass)

The visual language of the Svitrix web control panel. Based on **Apple Liquid
Glass** (WWDC 2025) — translucent materials with background blur and optical
depth, over Apple's system colour palette (Human Interface Guidelines).

This document covers the visual layer: tokens, materials, components, motion.
**Status: implemented** — the language is in the whole SPA (`src/styles/global.css`
+ `components/ui/` + pages). The token values below are reconciled with the current
`global.css` (after a design-review pass: hierarchy inverted — in dark the surface
is _lighter_ than the background; field plates are opaque; squircle deferred).

> Scope: `web/` (Preact SPA, 6+ pages). Does **not** cover rendering on the 32×8
> LED matrix itself — that has its own constraints (see `src/effects/README.md`).

---

## 1. Liquid Glass principles

1. **Material, not colour.** Surfaces are glass: a translucent layer that blurs
   and picks up a tint from the content beneath it. The background shows through.
2. **Layers and depth.** Content lives at the bottom; controls (nav, cards,
   toasts) float above it as glass layers. Hierarchy comes from blur and shadow,
   not borders.
3. **Light and specular.** A thin specular highlight runs along the glass edge
   (bright rim top, soft shadow below). Glass reacts to light rather than being
   outlined by a line.
4. **Concentricity.** Nested radii agree: inner radius = outer − padding. Corners
   "flow" into each other.
5. **Restraint.** Accent colour is used sparingly (active state, one primary
   action). The base palette is neutral. Glass does the work; colour is the accent.
6. **Adaptivity.** All materials and colours are dynamic (light/dark). Nothing
   hardcoded; everything via CSS variables and `color-mix()`.
7. **State is a recipe, not a colour.** Focus / pressed / disabled / error /
   loading are *structural* changes (accent ring, deeper inset, opacity, red rim
   + supporting text, `aria-busy`) — recognisable without relying on one fragile
   colour cue. Energy lives in the interaction: the moving part (toggle knob,
   slider thumb) animates on touch; the resting state stays calm.

---

## 2. Colour palette (Apple System Colors)

Based on Apple HIG system colours. The Svitrix brand accent is **amber**
(`systemYellow`-like), replacing Apple's default blue as the signature colour.

### 2.1 Semantic tokens

Never use raw hex in components — only the semantic variables.

| Token | Role | Light | Dark |
|-------|------|-------|------|
| `--label` | Primary text | `rgba(0,0,0,.85)` | `rgba(255,255,255,.95)` |
| `--label-secondary` | Secondary text (AA) | `rgba(60,60,67,.75)` | `rgba(235,235,245,.62)` |
| `--label-tertiary` | Captions | `rgba(60,60,67,.45)` | `rgba(235,235,245,.35)` |
| `--separator` | Hairline dividers | `rgba(0,0,0,.12)` | `rgba(255,255,255,.12)` |
| `--fill` | Control fill | `rgba(120,120,128,.12)` | `rgba(130,132,145,.28)` |
| `--fill-secondary` | Hover | `rgba(120,120,128,.16)` | `rgba(130,132,145,.40)` |
| `--surface-tint` | Glass base (cards/nav) | `#ffffff` | `#383d4d` |
| `--field-plate` | Field plate (opaque) | `rgba(233,235,241,.96)` | `rgba(14,16,23,.88)` |
| `--placeholder` | Placeholder (AA) | `rgba(60,60,67,.70)` | `color-mix(--label 55%)` |

Apple's trick: neutral control layers (`--fill`) use **translucent grey** so they
pick up the background. Text tokens use Apple's real tints `#EBEBF5` (dark) /
`#3C3C43` (light) instead of pure black/white; values are tuned to pass **WCAG AA
(≥4.5:1)** on the card in both themes.

### 2.2 Accents (system)

| Token | Purpose | Light | Dark |
|-------|---------|-------|------|
| `--accent` | Brand, active, primary | `#E6A100` | `#FFC93C` |
| `--accent-pressed` | Pressed | `#C98A00` | `#E6A100` |
| `--blue` | Links, info (systemBlue) | `#007AFF` | `#0A84FF` |
| `--green` | Success (systemGreen) | `#34C759` | `#30D158` |
| `--red` | Danger **fills / rims** (non-text) | `#FF3B30` | `#FF453A` |
| `--danger-text` | Danger **text** (button, field error) — tuned for 4.5:1 | `#D70015` | `#FF8A80` |
| `--orange` | Warning (systemOrange) | `#FF9500` | `#FF9F0A` |

Rule: use the accent only for **one** primary action per screen, the active tab,
and an on `Toggle`. Everything else is neutral glass.

`--red` vs `--danger-text`: `--red` is for surfaces (danger-button fill, invalid
field rim) where 3:1 non-text contrast suffices. Red **text** must use
`--danger-text` — system red only reaches ~3.9:1 on the card, below the 4.5:1 text
minimum, so a tuned darker/lighter red is used per theme.

### 2.3 The scene background — critical

Liquid Glass **takes its colour from the environment beneath it** and refracts it.
So the scene must be **alive and colourful**, otherwise the glass degrades into a
grey translucent card (plain glassmorphism). A flat dark background is the number
one reason an imitation fails to read as Apple. We use a mesh of coloured
radial-blooms over a dark/light base.

The blooms are **pulled inward** (x = 28/78/58%) so colour bleeds under the centred
content column, not just at the edges (otherwise the middle is flat and the glass
has nothing to refract):

```css
/* dark — current values */
--scene:
  radial-gradient(62% 50% at 28% 2%,   rgba(94, 92, 230, 0.30), transparent 62%),
  radial-gradient(58% 46% at 78% 8%,   rgba(48, 209, 209, 0.20), transparent 58%),
  radial-gradient(72% 62% at 58% 100%, rgba(255, 110, 90, 0.17), transparent 60%),
  radial-gradient(120% 120% at 50% 35%, #1a1d28, #0e1016);
```

Colour in the UI comes **from here**, through the glass — not from a brand fill.
Amber stays a point accent (§2.2), never a background.

### 2.4 Elevation (critical for dark) — "higher = lighter"

In dark mode, hierarchy is built by **lightening** surfaces, not darkening them
(like Apple Settings): background `#0e1016` → **card is lighter** (`--surface-tint
#383d4d`) → **field is a dark well** (`--field-plate`). The reverse (dark cards on a
dark background) collapses into "black-on-black". In light mode it's the usual
scheme (white cards, light-grey field wells).

---

## 3. Materials (Glass)

The core of the language. Three blur levels (`--glass-blur-*`) + a tint density
`--glass`.

| Level | blur token | blur | Density | Where |
|-------|-----------|------|---------|-------|
| Thin | `--glass-blur-thin` | 30px | nav 72% · toast 70% | Navigation, toasts |
| Regular | `--glass-blur-regular` | 40px | cards `--glass` (77% dark / 70% light) | Section cards (base) |
| Thick | `--glass-blur-thick` | 50px | ~92% | Modals `ConfirmDialog`/`PromptDialog` |

Card density (77/70%) is higher than the ~52% "glassy" value from an early version:
in dense content Apple trades transparency for legibility (Deference) — the scene
still tints the material, but text stays contrast-safe.

### 3.1 The glass surface recipe

Glass is a **tinted base (at `--glass` density) + backdrop-blur + a curved specular
rim**. The rim is not a flat `1px` line but a theme-aware gradient `--glass-shell`,
masked down to a 1px ring (reads as light catching a curved glass edge). Plus
`overflow: hidden` — otherwise the blurred backdrop pokes past the rounded corners.

```css
.card {
  position: relative;
  overflow: hidden; /* clip backdrop-filter to the radius */
  background:
    linear-gradient(180deg, rgba(255,255,255,.06), transparent 42%), /* top sheen */
    color-mix(in srgb, var(--surface-tint) var(--glass), transparent);
  backdrop-filter: blur(var(--glass-blur-regular)) saturate(180%);
  -webkit-backdrop-filter: blur(var(--glass-blur-regular)) saturate(180%);
  border: none;
  border-radius: var(--r-card);
  box-shadow: 0 1px 1px rgba(0,0,0,.14), 0 14px 34px rgba(0,0,0,.34);
}
/* curved specular rim (theme-aware --glass-shell), masked to a 1px ring */
.card::before {
  content: ""; position: absolute; inset: 0; border-radius: inherit; padding: 1px;
  background: var(--glass-shell);
  -webkit-mask: linear-gradient(#000 0 0) content-box, linear-gradient(#000 0 0);
  -webkit-mask-composite: xor; mask-composite: exclude; pointer-events: none;
}
```

- `--glass-shell`: dark → white specular; light → dark edge (a white rim is
  invisible on the pale plate). Same token on fields / `Select`.
- `saturate(180%)` boosts the scene colour bleeding through the glass.
- **Corners:** plain `border-radius`. Squircle via `corner-shape` is **deferred** —
  the `superellipse()` parameter is logarithmic (`≈2^k`), easy to overshoot into a
  square.
- **Fallback:** no `backdrop-filter` → opaque `--surface-tint`
  (+ `prefers-reduced-transparency`).

### 3.2 What is glass, what isn't

- **Glass:** Nav (thin), Card sections (regular), Toast (thin), modals
  `ConfirmDialog`/`PromptDialog` (thick).
- **Not glass (solid):** the field plate (`--field-plate`, **always** opaque — for
  legibility and so the rim gradient doesn't "leak" as a diagonal metallic sheen
  across the body), the native `<select>` dropdown, the LED-preview canvas on
  `/screen`, and icons.

### 3.3 Refraction (Apple's differentiator) — recipe

What separates **true** Liquid Glass from blur is **refraction**: content behind
the glass is lensed at the edges. Implementation (per the
[kube.io](https://kube.io/blog/liquid-glass-css-svg/) deconstruction):

**Step 1 — displacement map (runtime canvas).** For each pixel of the `bezel`-wide
edge: a squircle surface function `f(x) = ⁴√(1 − (1 − x)⁴)`, its derivative →
normal, refraction via Snell's law (`n₁=1`, `n₂=1.5`), magnitude normalized to
[0,1]. Encode into RGBA (R = X, G = Y):

```js
const r = 128 + x * 127;   // X displacement
const g = 128 + y * 127;   // Y displacement
const b = 128, a = 255;    // each channel range: −128..127 px
```

**Step 2 — SVG filter** (map into `feImage`, displace the backdrop):

```html
<svg width="0" height="0"><filter id="liquid" color-interpolation-filters="sRGB">
  <feImage href="{displacementMapDataUrl}" x="0" y="0" width="{w}" height="{h}" result="map"/>
  <feDisplacementMap in="SourceGraphic" in2="map" scale="{maxDisplacementPx}"
                     xChannelSelector="R" yChannelSelector="G"/>
</filter></svg>
```

**Step 3 — CSS:** `backdrop-filter: url(#liquid);` + our tint, rim (§3.1) and
shadow layers on top. Specular and chromatic aberration are separate `feImage` /
`feBlend mode="screen"` passes. `feImage` is bound to a fixed size → the filter is
built per element size (rebuilt on resize).

**Limitation (important):** an SVG filter in `backdrop-filter` is **Chromium only**.
Safari/Firefox (including **iOS Safari**, a frequent client of the device panel)
don't apply it → there the base layer §3.1 (blur + rim) is used. So refraction is
strictly a **progressive enhancement** under `@supports`/feature detection, not the
base. Live POC: Storybook → `Design System / Refraction`.

---

## 4. Shape and geometry

### 4.1 Radii (concentric)

```css
--r-card: 20px;     /* section card */
--r-control: 12px;  /* button, input, select */
--r-inner: 8px;     /* nested chip, tag */
--r-pill: 999px;    /* toggle, segmented control, avatar */
```

Concentricity rule: if a control sits inside a card with `padding: 16px` and radius
`20px`, its radius ≈ `20 − 16`, rounded to `--r-control: 12px`. Corners must not
"fight".

### 4.2 Spacing (4pt grid)

All sizes are multiples of 4: `4 · 8 · 12 · 16 · 20 · 24 · 32`.

```css
--space-1: 4px;  --space-2: 8px;  --space-3: 12px;
--space-4: 16px; --space-5: 20px; --space-6: 24px; --space-8: 32px;
```

- Card inner padding: `--space-5` (20px).
- Gap between sections: `--space-4`.
- Gap in `FormRow`: `--space-3`.

### 4.3 Line weight

Dividers are **hairline** `0.5px` (1 physical pixel on retina), colour
`--separator`. Hard `1px solid --separator` is almost absent in the target design:
the edge is defined by light (specular), not a line.

---

## 5. Typography

Apple's system font — SF Pro via the `-apple-system` stack. Names map to Apple
text styles, but the **sizing follows web norms**, which differ from native iOS:

- **`rem`, not `px`** — sizes are relative to the browser's root font-size, so
  text honours the user's font-size setting / zoom (WCAG 1.4.4). The root is left
  at the browser default (`html { font-size: 100% }`); the compact base is scoped
  to `body`.
- **Compact base = 14px** (`--text-body`), one step below Apple Body (17px) — a
  deliberate density choice for a controls-heavy settings UI, not Apple-strict.
- **Line-height ≥ 1.5 for prose** (`--lh-body`, per WCAG 1.4.12) — looser than
  iOS Body (~1.29); tighter (`--lh-tight`, `--lh-solid`) only for dense UI labels
  and large numerals.
- **Weights: standard SF ladder only** — 400 / 500 / 600 / 700 (no off-scale 650).

```css
--font: -apple-system, BlinkMacSystemFont, "SF Pro Text", "Segoe UI", Roboto, sans-serif;
```

| Token | rem / px | Apple style | Use |
|-------|----------|-------------|-----|
| `--text-title`    | 1.25rem / 20 | Title 3 | Card title, stat value |
| `--text-headline` | 1.0625rem / 17 | Headline | Dialog title |
| `--text-subhead`  | 0.9375rem / 15 | Subheadline | Mono stat value |
| `--text-body`     | 0.875rem / 14 | (compact base) | Body text, inputs, buttons |
| `--text-footnote` | 0.8125rem / 13 | Footnote | Labels, hints, nav, `--label-secondary` |
| `--text-caption`  | 0.75rem / 12 | Caption 1 | Units, captions, `--label-tertiary` |

Line-height: `--lh-body` 1.5 · `--lh-tight` 1.3 · `--lh-solid` 1.1.
Weights: `--weight-regular` 400 · `--weight-medium` 500 · `--weight-semibold` 600 · `--weight-bold` 700.

- Negative tracking on large titles: `letter-spacing: -0.01em` (card/stat titles).
- Numbers in statuses/stats — `font-variant-numeric: tabular-nums` (no "jumping").
- No `<b>`/bold for emphasis — only via the weight token.

---

## 6. Components (`components/ui/`)

Target behaviour of each, mapped to the existing files.

### `Card.tsx` — glass section
The main container. Regular glass, radius `--r-card`, padding `--space-5`. Title is
Title style, subtitle Subhead/`--label-secondary`. A float shadow lifts it off the
scene; hairline rim.

### `Nav.tsx` — floating navigation
Sticky at top, thin glass, blur always on (content scrolls under it). Active tab is
a `--fill` pill + `--accent` text. Logo is accent. Right-side group: language
dropdown + theme toggle. On mobile it can move to a bottom tab-bar (safe-area
inset).

### `Button.tsx` — three variants
- **Primary:** `--accent` fill, contrast text, top specular highlight, radius
  `--r-control`. Press — `scale(0.97)` + `--accent-pressed`.
- **Plain/glass:** translucent `--fill`, glassy, for secondary actions.
- **Danger:** `--red` text/edge, soft red fill on hover.

### `Toggle.tsx` — switch
Pill `--r-pill`. Off — `--fill` (recessed track); On — `--green` fill (iOS switch).
Knob is a white circle with a soft shadow and a spring transition (see §7).

### `Select.tsx` — dropdown (two-layer)
Trigger is field-like: `.shell` (rim) + native `<select>` on `--field-plate`. The
chevron is a **CSS-border** glyph on `.shell::after` (not emoji/image → can't tile,
theme-aware). The `<option>` list is native (can't be styled cross-browser). Focus
— solid `--surface-tint` + accent ring.

### `Slider.tsx` — range
Track is a `--fill` capsule; fill up to the value is `--accent`. Thumb is a glass
circle with a specular highlight, scales up while dragging. Value is tabular-nums.

### `TextField.tsx` — two-layer field (stabilized plate)
An **outer shell** `.shell` (1px `--glass-shell` rim) + an **inner plate** `.input`
(`--field-plate`, opaque) — the plate keeps the value and placeholder
(`--placeholder`, AA) legible over any scene. Recessed inset shadow ("carved into
the glass"); on focus the plate → `--surface-tint` + ring
`0 0 0 3px color-mix(--accent 25%, transparent)`. The `ColorField` swatch is
`--fill` with a top specular. Min height 44px.

### `ConfirmDialog.tsx` / `PromptDialog.tsx` — modals (in `components/ui/`)
Thick glass over a scrim; focus on the safe button (Cancel), Esc/backdrop click
cancels; spring entrance. They replace native `confirm()`/`prompt()` app-wide.

### `FormRow.tsx` — layout
2 columns on desktop, 1 at `≤600px`. Gap `--space-3`. Label is Headline, control
right/below.

### `Toast.tsx` — notification
A thin-glass capsule near the bottom edge, icon colour by type (green/red/orange),
spring + fade entrance. It is a live region (`role=status`/`alert`, `aria-live`);
success auto-dismisses, errors persist. `toast(msg, { error })`.

### Segmented control — not implemented (future)
Candidate to replace `Select` for short lists (2–4 options, e.g. a TMODE picker):
a material container, radius `--r-control`, 4px inner padding; the selected segment
is `accent 15%` fill + `accent 30%` rim, others transparent; the selection slides
to the new segment with a spring (`--ease-spring`). Each segment ≥ 44×32px.

---

## 7. Motion

Liquid Glass reacts "fluidly": springy, not linear.

```css
--ease-spring: linear(0, 0.2, 0.6, 0.9, 1.02, 1);  /* soft overshoot */
--ease-out: cubic-bezier(0.22, 1, 0.36, 1);
--dur-fast: 0.18s;   /* hover */
--dur-med: 0.28s;    /* toggle, popover entrance */
--dur-slow: 0.4s;    /* page transition */
```

- **Hover/focus:** `--dur-fast`, `--ease-out`.
- **Toggle, slider, popover:** `--dur-med`, `--ease-spring` (light knob bounce).
- **Page change:** content leaves with blur+fade, the new one rises 8px from below.
  The glass nav stays put.
- **Press:** `transform: scale(0.97)`, instant.
- **`prefers-reduced-motion`:** all transform/spring → plain `opacity` fade;
  overshoot off.

---

## 8. Accessibility

- **Contrast:** text on glass is at least WCAG AA (4.5:1). The tinted glass base is
  dense enough (cards 77% dark / 70% light; field plates opaque) to guarantee it;
  verify against the lightest/darkest possible background under the layer.
- **Focus:** a visible `--accent` ring on **all** interactive elements, including
  keyboard nav (`:focus-visible`) — buttons, inputs, select, toggle.
- **`prefers-reduced-transparency`:** glass → solid `--surface-tint`, blur off.
- **`prefers-contrast: more`:** hairline dividers → full `1px`, text strengthened.
- **Touch targets:** at least `44×44px` (Apple HIG), especially tab-bar and toggle.

### 8.1 WCAG 2.1 AA patterns (required)

The SPA was audited against WCAG 2.1 AA (all four principles) and verified with an
automated `axe-core` scan across every route in both themes. Keep these patterns
when adding UI:

- **Danger text uses `--danger-text`, not `--red`.** `--red` is for fills/rims
  (non-text). Red *text* (danger button, field error message) must use
  `--danger-text` — a tuned red that hits 4.5:1 on the card (`#ff8a80` dark /
  `#d70015` light; system red fails at ~3.9:1).
- **Toasts are a live region.** `toast(msg)` / `toast(msg, { error: true })` —
  the container is always mounted with `role="status"`/`alert` + `aria-live`, so
  screen readers announce results. Errors don't auto-dismiss. Never surface an
  error only visually.
- **Field errors are associated.** `TextField`/`Select` take `error?: string` →
  they set `aria-invalid` + `aria-describedby` and show an icon+text message
  (colour is never the only cue). Route validation through this, not toast-only.
- **Destructive/rebooting actions confirm first.** WiFi connect, backup restore,
  reset, reboot, deletes go through `ConfirmDialog` (never native `confirm()`).
  Dialogs trap + restore focus and are named via `aria-labelledby`.
- **Skip link + `<main id="main">`** in `main.tsx`; per-route `document.title`
  via `useTitle`. Nav marks the active link with `aria-current="page"`; the theme
  toggle has `aria-label` + `aria-pressed`.
- **Semantics:** real headings (`<h2>`/`<h3>`), real lists (`<ul>/<li>`),
  `role="img"` + label on state-bearing `<canvas>`/`<svg>`, `alt` on images.
- **Icon-only buttons** need an `aria-label`; loading buttons keep their name and
  set `aria-busy`.
- **Type in `rem`** (never `px` font-size) so text honours the browser setting
  (WCAG 1.4.4); see §5.

---

## 9. `global.css` tokens (current)

Reconciled with `src/styles/tokens.css` (the token block; `global.css` is a thin
aggregator that `@import`s tokens → reset → base → components → a11y). The legacy
alias set (`--bg`, `--bg-card`, `--border`, `--text`, `--radius`…) has been removed
— components consume the tokens below directly.

```css
:root { /* DARK */
  --scene:
    radial-gradient(62% 50% at 28% 2%,   rgba(94,92,230,.30), transparent 62%),
    radial-gradient(58% 46% at 78% 8%,   rgba(48,209,209,.20), transparent 58%),
    radial-gradient(72% 62% at 58% 100%, rgba(255,110,90,.17), transparent 60%),
    radial-gradient(120% 120% at 50% 35%, #1a1d28, #0e1016);
  --surface-tint: #383d4d;  --glass: 77%;   /* dark: surface LIGHTER than bg */

  --label:           rgba(255,255,255,.95);
  --label-secondary: rgba(235,235,245,.62); /* Apple #EBEBF5 · AA */
  --label-tertiary:  rgba(235,235,245,.35);
  --separator:       rgba(255,255,255,.12);
  --fill:            rgba(130,132,145,.28);
  --fill-secondary:  rgba(130,132,145,.40);
  --field-plate:     rgba(14,16,23,.88);     /* opaque well */
  --placeholder:     color-mix(in srgb, var(--label) 55%, transparent);
  --glass-shell: linear-gradient(135deg,
    rgba(255,255,255,.5), rgba(255,255,255,.06) 40%, transparent 60%, rgba(255,255,255,.2));

  --accent: #ffc93c; --accent-pressed: #e6a100; --accent-hover: #ffd65c;
  --blue: #0a84ff; --green: #30d158; --red: #ff453a; --orange: #ff9f0a;
  --danger-text: #ff8a80; /* red *text* — 5.1:1 on card (--red fails at 3.9:1) */

  --glass-blur-thin: 30px; --glass-blur-regular: 40px; --glass-blur-thick: 50px;
  --r-card: 20px; --r-control: 12px; --r-inner: 8px; --r-pill: 999px;
  --space-1:4px; --space-2:8px; --space-3:12px; --space-4:16px; --space-5:20px;
  --space-6:24px; --space-8:32px;
  --ease-spring: linear(0,.2,.6,.9,1.02,1); --ease-out: cubic-bezier(.22,1,.36,1);
  --dur-fast:.18s; --dur-med:.28s; --dur-slow:.4s;
  --font: -apple-system, BlinkMacSystemFont, "SF Pro Text", "Segoe UI", Roboto, sans-serif;

  /* type — rem so text scales with the browser setting (WCAG 1.4.4) */
  --text-title:1.25rem; --text-headline:1.0625rem; --text-subhead:.9375rem;
  --text-body:.875rem; --text-footnote:.8125rem; --text-caption:.75rem;
  --lh-body:1.5; --lh-tight:1.3; --lh-solid:1.1;
  --weight-regular:400; --weight-medium:500; --weight-semibold:600; --weight-bold:700;
}

[data-theme="light"] {
  --scene:
    radial-gradient(70% 58% at 22% 4%,   rgba(120,118,255,.34), transparent 62%),
    radial-gradient(64% 50% at 80% 8%,   rgba(60,200,220,.30), transparent 58%),
    radial-gradient(80% 68% at 60% 100%, rgba(255,150,120,.28), transparent 60%),
    radial-gradient(120% 120% at 50% 50%, #eef1f8, #dde2ec);
  --surface-tint: #ffffff; --glass: 70%;
  --label: rgba(0,0,0,.85); --label-secondary: rgba(60,60,67,.75); /* #3C3C43 · AA */
  --label-tertiary: rgba(60,60,67,.45); --separator: rgba(0,0,0,.12);
  --fill: rgba(120,120,128,.12); --fill-secondary: rgba(120,120,128,.16);
  --field-plate: rgba(233,235,241,.96); /* ≈systemGray6 */
  --placeholder: rgba(60,60,67,.70);
  --glass-shell: linear-gradient(135deg,
    rgba(255,255,255,.85), rgba(0,0,0,.05) 40%, transparent 60%, rgba(0,0,0,.2));
  --accent: #e6a100; --accent-pressed: #c98a00; --accent-hover: #c98a00;
  --blue:#007aff; --green:#34c759; --red:#ff3b30; --orange:#ff9500;
  --danger-text: #d70015; /* 4.9:1 on the light card */
}
```

---

## 10. Migration — complete ✅

All steps landed + a design-review pass was applied (P0/P1: AA contrast,
scene-under-content, elevation, opaque field plates).

1. ✅ **Tokens** §9 in `global.css` (legacy aliases fully removed; components use new tokens directly).
2. ✅ **Material** — glass `Card`/`Nav`/`Toast` + `@supports` fallback.
3. ✅ **Shape** — radii, hairlines, 4pt.
4. ✅ **Controls** — `Button`/`Toggle`/`Select`/`Slider`/`TextField` + specular
   (raised/recessed model, two-layer fields).
5. ✅ **Motion** — spring, page transition (`glassIn`), `reduced-motion`.
6. ✅ **Audit** — WCAG AA, `reduced-transparency`/`prefers-contrast`, touch targets.

**Deferred:** refraction (§3.3, Chromium-only, POC in Storybook) · squircle corners
(`corner-shape`).

**Verify** (always build): `cd web && npm run build` — measure app.js/style.css
gzip. The **< 30 KB gzip** budget is currently exceeded because of i18n (5 languages
bundled into a single IIFE); the glass/tokens are pure CSS and weigh almost nothing.

---

## 11. Design decisions (rationale)

Why the non-obvious choices, distilled from the design-review pass so the reasoning
survives (the review doc itself is retired):

- **The scene bleeds *under* the content, not just at the edges.** Liquid Glass
  takes its colour from what's beneath it; `saturate(180%)` needs colour to
  amplify. The blooms are pulled inward (28% / 78% / 58%) so the centred column
  sits over colour, not flat grey — otherwise the glass reads as a flat dark theme.
- **Field plates are opaque, not translucent.** A deliberate trade: the plate
  gives up scene-transmission through the field body in exchange for guaranteed
  value/placeholder legibility over any background (16.6:1 dark / 13.2:1 light),
  and it confines the `--glass-shell` gradient to the 1px rim (a translucent plate
  leaks the rim as a metallic diagonal sheen across the field).
- **We do not copy Apple's sub-AA `secondaryLabel`/placeholder.** Apple's own
  `#3C3C43@60%` is ~3.4:1 — fine on iOS, but on the web WCAG governs, so secondary
  labels and placeholders are tuned to ≥4.5:1 (see `--label-secondary`,
  `--placeholder`, `--danger-text`).
- **The primary button has no outer amber glow** — a crisp specular + tight shadow
  + a hint of warmth (`accent 22%`), not a gamer-glow halo. Restraint reads premium.
- **The content column is 720px, controls are dense.** Apple settings surfaces are
  narrow; a single-control card over-set to full width reads as empty.
- **Elevation inverts in dark mode:** the surface is *lighter* than the background
  (elevated = lighter), matching Apple's base/elevated model.

---

## References

**Apple**
- HIG — Materials: <https://developer.apple.com/design/human-interface-guidelines/materials>
- HIG — Color: <https://developer.apple.com/design/human-interface-guidelines/color>
- HIG — Typography: <https://developer.apple.com/design/human-interface-guidelines/typography>
- HIG — Dark Mode (base vs elevated): <https://developer.apple.com/design/human-interface-guidelines/dark-mode>
- Meet Liquid Glass — WWDC25 (219): <https://developer.apple.com/videos/play/wwdc2025/219/>
- Build a SwiftUI app with the new design — WWDC25 (323): <https://developer.apple.com/videos/play/wwdc2025/323/>

**Accessibility / usability**
- WCAG 2.1 — Understanding SC 1.4.3 Contrast: <https://www.w3.org/TR/UNDERSTANDING-WCAG20/visual-audio-contrast-contrast.html>
- Liquid Glass Is Cracked, and Usability Suffers — NN/g: <https://www.nngroup.com/articles/liquid-glass/>

**Practitioner references**
- liquid-glass-guide (giorgio-a11y): <https://github.com/giorgio-a11y/liquid-glass-guide/blob/main/LIQUID-GLASS-GUIDE.md>
- Apple semantic colour values (Sarunw dark cheat sheet): <https://sarunw.com/posts/dark-color-cheat-sheet/>

**Implementation:** [`web/CLAUDE.md`](CLAUDE.md) (build guide) · [`web/README.md`](README.md) · [`src/styles/global.css`](src/styles/global.css)
