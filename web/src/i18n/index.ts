// Lightweight i18n — preact-i18n's tiny `translate()` engine (dotted keys +
// {{var}} templates) driven by signals. Replaces i18next / react-i18next /
// langdetector (~18 KB runtime). The build is a single IIFE (no code-splitting),
// so locales are imported statically — the win here is the dropped runtime, not
// lazy chunks. `useTranslation` mirrors react-i18next's API and is aliased to
// `react-i18next` (vite.config.ts / tsconfig) so existing call sites don't change.
import { signal } from "@preact/signals";
import { translate } from "preact-i18n";
import en from "./locales/en";
import uk from "./locales/uk";
import es from "./locales/es";
import de from "./locales/de";
import nl from "./locales/nl";

export const supportedLanguages = [
  { code: "en", label: "English" },
  { code: "uk", label: "Українська" },
  { code: "es", label: "Español" },
  { code: "de", label: "Deutsch" },
  { code: "nl", label: "Nederlands" },
] as const;

export type LanguageCode = (typeof supportedLanguages)[number]["code"];
type Dict = typeof en;

const dicts: Record<LanguageCode, Dict> = { en, uk, es, de, nl };

const STORAGE_KEY = "lang";
function detect(): LanguageCode {
  const saved = localStorage.getItem(STORAGE_KEY) as LanguageCode | null;
  if (saved && saved in dicts) return saved;
  const nav = navigator.language?.slice(0, 2) as LanguageCode;
  if (nav && nav in dicts) return nav;
  return "en";
}

const initial = detect();
const langSig = signal<LanguageCode>(initial);
const dictSig = signal<Dict>(dicts[initial]);
document.documentElement.lang = initial;

export function changeLanguage(code: string) {
  if (!(code in dicts)) return;
  const c = code as LanguageCode;
  dictSig.value = dicts[c];
  langSig.value = c;
  localStorage.setItem(STORAGE_KEY, c);
  document.documentElement.lang = c;
}

type Fields = Record<string, string | number>;

/** Imperative translate for use outside components (helpers, context, toasts).
 *  Reads the current dictionary at call time — non-reactive, but on-demand so it
 *  always reflects the active language. Mirrors i18next's `i18n.t(...)`. */
export function t(key: string, fields?: Fields): string {
  return translate(key, "", dictSig.value, fields, undefined, key);
}

/** Drop-in for react-i18next's useTranslation. Reading the signals here
 *  subscribes the calling component so it re-renders on language change. */
export function useTranslation() {
  const dict = dictSig.value;
  const lang = langSig.value;
  const tt = (key: string, fields?: Fields): string =>
    translate(key, "", dict, fields, undefined, key);
  return { t: tt, i18n: { changeLanguage, resolvedLanguage: lang } };
}

export default { useTranslation, changeLanguage, t };
