import type { ComponentChildren } from "preact";
import { useTranslation } from "react-i18next";
import styles from "./stats.module.css";
import type { Band } from "./format";
import { bandColor, pct, wifiInfo, batBand } from "./format";

/** A stat tile: label + value(+unit), optional footer (meter / bars / sparkline /
 *  status). Value text always wears --label ink; hue lives only in the mark. */
export function StatTile({
  label,
  value,
  unit,
  mono,
  children,
}: {
  label: string;
  value: ComponentChildren;
  unit?: string;
  mono?: boolean;
  children?: ComponentChildren;
}) {
  return (
    <div class={styles.tile}>
      <div class={styles.label}>{label}</div>
      <div class={styles.valueRow}>
        <span class={`${styles.value}${mono ? " " + styles.mono : ""}`}>{value}</span>
        {unit && <span class={styles.unit}>{unit}</span>}
      </div>
      {children && <div class={styles.footer}>{children}</div>}
    </div>
  );
}

/** status dot + word — so state is never conveyed by colour alone. */
export function StatusRow({ status, word }: { status: Band; word: string }) {
  return (
    <span class={styles.statusRow}>
      <span class={styles.dot} style={{ background: bandColor(status) }} />
      {word}
    </span>
  );
}

/** Pure-CSS horizontal meter (no SVG). */
export function Meter({
  value,
  max,
  min = 0,
  status = "none",
  ariaLabel,
}: {
  value: number;
  max: number;
  min?: number;
  status?: Band;
  ariaLabel?: string;
}) {
  return (
    <div
      class={styles.meterTrack}
      role="meter"
      aria-valuenow={value}
      aria-valuemin={min}
      aria-valuemax={max}
      aria-label={ariaLabel}
    >
      <div class={styles.meterFill} style={{ width: pct(value, max, min) + "%", background: bandColor(status) }} />
    </div>
  );
}

/** WiFi signal bars (4). Active count + hue from wifiInfo(dbm). */
export function SignalBars({ dbm }: { dbm: number }) {
  const { band, bars } = wifiInfo(dbm);
  const active = bandColor(band);
  const heights = [5, 8, 11, 14];
  return (
    <svg width="20" height="14" viewBox="0 0 20 16" aria-hidden="true">
      {heights.map((h, i) => (
        <rect key={i} x={i * 5} y={16 - h} width="3.4" height={h} rx="1" fill={i < bars ? active : "var(--label-tertiary)"} />
      ))}
    </svg>
  );
}

/** Battery glyph filled to %, hue by battery band. */
export function BatteryGlyph({ percent }: { percent: number }) {
  const w = Math.max(1.5, (Math.max(0, Math.min(100, percent)) / 100) * 17);
  return (
    <svg width="26" height="14" viewBox="0 0 26 14" aria-hidden="true">
      <rect x="0.6" y="1.6" width="21" height="10.8" rx="2.6" fill="none" stroke="var(--label-tertiary)" stroke-width="1.2" />
      <rect x="23" y="5" width="2.4" height="4" rx="1" fill="var(--label-tertiary)" />
      <rect x="2.2" y="3.2" width={w} height="7.6" rx="1.4" fill={bandColor(batBand(percent))} />
    </svg>
  );
}

/** Full-width trend sparkline: area fill + line. Responsive via preserveAspect
 *  none + non-scaling stroke (keeps 1.5px line crisp at any width). `minSpan`
 *  guards against amplifying tiny noise on a near-flat series. Null if <2 pts. */
export function Sparkline({
  data,
  height = 40,
  minSpan = 0,
  color = "var(--accent)",
}: {
  data: number[];
  height?: number;
  minSpan?: number;
  color?: string;
}) {
  if (!data || data.length < 2) return null;
  const n = data.length;
  let min = Math.min(...data);
  let max = Math.max(...data);
  if (max - min < minSpan) {
    const mid = (min + max) / 2;
    min = mid - minSpan / 2;
    max = mid + minSpan / 2;
  }
  const range = max - min || 1;
  const W = 100;
  const X = (i: number) => (i * W) / (n - 1);
  const Y = (v: number) => height - 3 - ((v - min) / range) * (height - 6);
  const line = data.map((v, i) => `${X(i).toFixed(2)},${Y(v).toFixed(2)}`).join(" ");
  const area = `0,${height} ${line} ${W},${height}`;
  return (
    <svg
      class={styles.spark}
      width="100%"
      height={height}
      viewBox={`0 0 ${W} ${height}`}
      preserveAspectRatio="none"
      aria-hidden="true"
    >
      <polygon points={area} fill={color} opacity="0.13" />
      <polyline
        points={line}
        fill="none"
        stroke={color}
        stroke-width="1.5"
        vector-effect="non-scaling-stroke"
        stroke-linecap="round"
        stroke-linejoin="round"
      />
    </svg>
  );
}

/** On/off state pill (word + dot). */
export function StatePill({ on, onLabel, offLabel }: { on: boolean; onLabel?: string; offLabel?: string }) {
  const { t } = useTranslation();
  return (
    <span class={styles.statusRow}>
      <span class={styles.dot} style={{ background: on ? "var(--green)" : "var(--label-tertiary)" }} />
      {on ? onLabel ?? t("common.on") : offLabel ?? t("common.off")}
    </span>
  );
}
