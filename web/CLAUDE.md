# Svitrix Web UI — AI build guide

Preact + Vite + TypeScript SPA (settings panel for the ESP32 clock), served from
LittleFS. This file is the **working guide for changing `web/`**: the rules and
recipes for adding UI without breaking the Liquid Glass language or WCAG 2.1 AA.

- **Design spec (what things look like / token values):** [DESIGN.md](DESIGN.md)
- **Run / build / structure:** [README.md](README.md)
- **Root patterns** (new config field, new page, cross-module impact): repo
  [`CLAUDE.md`](../CLAUDE.md) → "Common Change Patterns".

The SPA is audited against **WCAG 2.1 AA** and verified clean by an `axe-core`
scan (all routes × light/dark). Keep it that way.

## Golden rules (every change)

- **Tokens only — no hard-coded colours or px.** Use `--label`, `--surface-tint`,
  `--accent`, `--fill`, `--separator`, spacing `--space-*`, radii `--r-*`
  (DESIGN.md §9). Font sizes use the **rem** type tokens `--text-*` / `--lh-*` /
  `--weight-*` (never `px` — WCAG 1.4.4). Danger **text** uses `--danger-text`,
  not `--red` (red is fills/rims only).
- **Preact:** `class` not `className`; import from `preact` / `preact/hooks`;
  reactive state via `@preact/signals`.
- **CSS Modules** (`*.module.css`) for component/page styles. Global CSS lives in
  `src/styles/`, split into `tokens.css` (design tokens) · `reset.css` · `base.css`
  (element defaults) · `components.css` · `a11y.css`, aggregated by `global.css`
  (`@import`s, one import site in `main.tsx`). Style light **and** dark (test both).
- **Native CSS nesting** is the house style — nest states/pseudo-elements/media
  under their base selector with `&` (`&:hover`, `&::after`, `&[aria-invalid]`,
  `@media {...}`) instead of repeating the selector. esbuild flattens it at build
  for old browsers. Exception: never *group* vendor pseudo-elements
  (`&::-webkit-…, &::-moz-…`) in one list — a browser drops the whole list if one
  is unknown; keep them as separate nested rules.
- **Modern CSS in use** (prefer these): `text-wrap: balance` on headings /
  `pretty` on paragraphs (in `base.css`); **container queries** — `.card` is a
  `container: card / inline-size`, so inner layouts query the card width, not the
  viewport (`@container card (...)`), not `@media`; `@property` to register an
  animatable custom prop (see `--pct` in `Slider.module.css`); `:has()`,
  `color-mix()`. **Theming is `light-dark()` + a three-state
  toggle** (System/Light/Dark). Colour tokens are single `light-dark(<light>,
  <dark>)` declarations; `:root { color-scheme: light dark }` follows the OS by
  default, and an explicit `data-theme` (set by the toggle in `Nav.tsx`, persisted
  in localStorage, pre-applied by the inline script in `index.html`) forces a
  scheme. Add a new colour token as `light-dark(...)`, never two theme blocks; a
  new theme-varying value that isn't a colour → fold it into a `light-dark()`
  colour if possible. New responsive layout inside a card → use `@container card`,
  not a viewport media query.
- **i18n:** every user-facing string (incl. `aria-label`, error text, titles) goes
  through `t("key")`. Add the key to **all five** locale slices
  (`src/i18n/locales/{en,uk,es,de,nl}/…`) — `Dict = typeof en`, so a key missing
  in another locale fails `tsc`.
- **Reuse `components/ui/`** (`Button` `TextField` `Select` `Slider` `Toggle`
  `ColorField` `TimeField` `Card` `FormRow` `Dialog`) instead of raw elements —
  they already carry the glass recipe + a11y wiring. Form controls take an
  optional `helper?` caption; `Select`/`Slider`/`Toggle`/`ColorField`/`TimeField`
  also take `error?`. `ColorField` has two modes: default **number** (colour as a
  24-bit int) and `hex` (colour as `"#RRGGBB"`, for string fields like
  CCORRECTION/CTEMP). Use `TimeField` (not raw `<input type=time>`) for HH:MM.
  Larger reusable pieces live directly under `components/`: `LivePreview`
  (polls `GET /api/screen`, `page`|`compact` variants), `SaveIndicator`
  (renders the `saveState`), `RebootOverlay` (polls `/version` via `ping()`
  until the device is back).
- **Verify:** `cd web && npm run build` (tsc + vite) must pass after every change.

## Recipe — new UI component (`components/ui/`)

**Liquid Glass anatomy** (DESIGN.md §1, §3, §6): a component = an outer glass
**shell** (rim/specular) + an inner **stabilized plate** (opaque enough that text
stays legible over any scene). Raised (convex) for buttons; recessed (concave,
inset shadow) for inputs. **State is a recipe, not a colour** — focus = accent
ring that ignores the background; pressed = deeper inset + `scale(0.97)`; disabled
= opacity ~0.5; error = red rim **+** text/icon; loading = `aria-busy`. Energy is
in the interaction (moving part animates on touch), the resting state is calm.
Honour `prefers-reduced-motion/-transparency/-contrast` (handled globally).

**WCAG AA checklist** (the required patterns — DESIGN.md §8.1):

- Native element where possible (`<button>`, `<input>`, `<select>`) → name/role/
  value for free. Custom widget → supply `role` + `aria-*` state + keyboard.
- Label every control (`<label htmlFor>` / `useId()`, or `aria-label`). Icon-only
  buttons need `aria-label`; loading buttons keep their name + `aria-busy`.
- Errors: use the `TextField`/`Select` `error?: string` prop → it wires
  `aria-invalid` + `aria-describedby` + a visible icon+text message (never colour
  alone). Forward `autocomplete` for user-data fields.
- Visible `:focus-visible` ring (accent) on every interactive element.
- Status/async feedback → `toast(msg, { error })` (live region), or a `role=status`
  node. Destructive/rebooting actions → gate behind `ConfirmDialog` (never native
  `confirm()`).
- Real semantics: `<h2>/<h3>` headings, `<ul>/<li>` lists, `role="img"` + label on
  state-bearing `<canvas>`/`<svg>`, `alt` on images. Touch targets ≥ 44px.
- Contrast ≥ 4.5:1 text / 3:1 non-text, in **both** themes. `axe-core` can't read
  contrast through the glass — verify tuned reds/greys by hand.

Add a `.mdx` doc + `.stories.tsx` if the component is reusable (Storybook only —
not shipped).

## Recipe — new page (`pages/<name>/`)

Directory-per-page with an `index.ts` barrel. Add the `<Route>` in `main.tsx` and
a title entry in its `ROUTE_TITLES` map (drives `document.title` / `useTitle`).
Compose from `Card` + `components/ui/`. Page-level heading is `<h2>`. Add the nav
link + i18n `nav.*` key. See README.md "Project Structure" / "Pages".

## Recipe — new settings field / section

`/settings` is a **master–detail** screen: `pages/settings/SettingsLayout.tsx`
renders a `SaveIndicator` header, then a vertical category rail
(`SettingsNav.tsx`, an ARIA `tablist` with keyboard nav) beside a content panel. Five categories (`CategoryId` in `SettingsNav.tsx`): **Appearance ·
Screens · Network · System · Tools**. Sections themselves are in
`pages/settings/sections/`; the `CATEGORY_SECTIONS` map in `SettingsLayout.tsx`
places each section under a category.

**Two save models — pick by field kind:**

- **Display fields auto-save.** Controls call `setSetting(patch, immediate?)`
  from `SettingsContext` — debounced 400 ms + coalescing, `immediate: true` for
  discrete inputs (toggles/selects/swatches). No per-section Save button (they
  and "Save All" were removed); `saveState` (`idle|saving|saved|error`) is shown
  by `SaveIndicator`. These sections live under **Appearance** (`DisplaySection`,
  `ClockFaceSection`, `NightModeSection`) and **Screens** (`AppsSection`).
- **Infra fields save explicitly.** Network/MQTT/NTP/Auth edit `config` via
  `updateConfig()` and persist through `InfraSaveBar` (a "Requires restart" badge
  + one Save button that fires `RebootOverlay`). These sit under **Network** /
  **System**.

The panel is wrapped in `<fieldset disabled={!online}>`, so controls disable
while the device is unreachable (heartbeat + `OfflineBanner`).

**Add a section:** create it in `sections/` (export from `sections/index.ts`),
then add it to the right category in `CATEGORY_SECTIONS`. A whole new category
also needs a `CategoryId` entry in `SettingsNav.tsx` (`CATEGORY_IDS`) + a
`settingsNav.*` i18n slice.

A new device-config field also touches firmware + API + types — follow the repo
`CLAUDE.md` "New config field" pattern (`ConfigTypes.h` → `Globals.cpp` →
ServerManager → `api/types.ts` → `SettingsContext.tsx` → the section UI).
