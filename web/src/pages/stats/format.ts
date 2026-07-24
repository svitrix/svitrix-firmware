// Formatting + status-band helpers for the device stats viz (see web/STATS-VIZ-PLAN.md).

import i18n from "../../i18n";

export type Band = "good" | "warn" | "crit" | "none";

/** good/warn/crit → a design-system status token; none → brand accent. */
export function bandColor(b: Band): string {
  return b === "good"
    ? "var(--green)"
    : b === "warn"
    ? "var(--orange)"
    : b === "crit"
    ? "var(--red)"
    : "var(--accent)";
}

export function pct(v: number, max: number, min = 0): number {
  if (max === min) return 0;
  return Math.max(0, Math.min(100, ((v - min) / (max - min)) * 100));
}

/** Free heap in KB → KB / MB string. */
export function formatBytes(bytes: number): string {
  const kb = bytes / 1024;
  return kb >= 1024 ? (kb / 1024).toFixed(1) + " MB" : Math.round(kb) + " KB";
}

export function formatCount(n: number): string {
  return n.toLocaleString("en-US");
}

/** Uptime: API returns seconds (as a string). Format "Nd Nh Nm"; pass through if
 *  it's already a formatted (non-numeric) string. */
export function formatUptime(u: string): string {
  const s = Number(u);
  if (!Number.isFinite(s)) return u;
  const d = Math.floor(s / 86400);
  const h = Math.floor((s % 86400) / 3600);
  const m = Math.floor((s % 3600) / 60);
  const parts: string[] = [];
  if (d) parts.push(d + "d");
  if (h || d) parts.push(h + "h");
  parts.push(m + "m");
  return parts.join(" ");
}

// ---- Status bands ---------------------------------------------------------

export function ramBand(freeKb: number): Band {
  if (freeKb >= 120) return "good";
  if (freeKb >= 60) return "warn";
  return "crit";
}

export function humBand(h: number): Band {
  if (h >= 30 && h <= 60) return "good";
  if ((h >= 20 && h < 30) || (h > 60 && h <= 70)) return "warn";
  return "crit";
}

export function tempBand(t: number): Band {
  if (t >= 18 && t <= 27) return "good";
  if ((t >= 10 && t < 18) || (t > 27 && t <= 32)) return "warn";
  return "crit";
}

export function batBand(p: number): Band {
  if (p > 50) return "good";
  if (p >= 20) return "warn";
  return "crit";
}

export interface WifiInfo {
  band: Band;
  bars: number; // 1..4
  word: string;
}
export function wifiInfo(dbm: number): WifiInfo {
  if (dbm >= -67)
    return {
      band: "good",
      bars: dbm >= -55 ? 4 : 3,
      word: dbm >= -55 ? i18n.t("stats.wifiExcellent") : i18n.t("stats.wifiGood"),
    };
  if (dbm >= -85)
    return {
      band: "warn",
      bars: dbm >= -75 ? 2 : 1,
      word: dbm >= -75 ? i18n.t("stats.wifiFair") : i18n.t("stats.wifiWeak"),
    };
  return { band: "crit", bars: 1, word: i18n.t("stats.wifiPoor") };
}
