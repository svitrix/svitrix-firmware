import { route } from "preact-router";
import { signal } from "@preact/signals";
import { useTranslation } from "react-i18next";
import { supportedLanguages } from "../i18n";
import styles from "./Nav.module.css";

const currentPath = signal(window.location.pathname);

/* Theme has three states. "system" (the default when nothing is stored) sets NO
   data-theme, so the root's `color-scheme: light dark` follows the OS — the
   browser tracks OS changes live, no matchMedia listener needed. An explicit
   "light"/"dark" writes data-theme, which forces the scheme. */
type ThemeMode = "system" | "light" | "dark";
const stored = localStorage.getItem("theme");
const themeMode = signal<ThemeMode>(
  stored === "light" || stored === "dark" ? stored : "system"
);

function applyTheme(mode: ThemeMode) {
  const root = document.documentElement;
  if (mode === "system") root.removeAttribute("data-theme");
  else root.setAttribute("data-theme", mode);
}
applyTheme(themeMode.value); // idempotent with the anti-FOUC script in index.html

/** Cycle System → Light → Dark → System. */
export function cycleTheme() {
  const order: ThemeMode[] = ["system", "light", "dark"];
  const next = order[(order.indexOf(themeMode.value) + 1) % order.length];
  themeMode.value = next;
  applyTheme(next);
  if (next === "system") localStorage.removeItem("theme");
  else localStorage.setItem("theme", next);
}

window.addEventListener("popstate", () => {
  currentPath.value = window.location.pathname;
});

const links = [
  { href: "/", key: "nav.screen" },
  { href: "/stats", key: "nav.stats" },
  { href: "/settings", key: "nav.settings" },
  { href: "/datafetcher", key: "nav.data" },
  { href: "/files", key: "nav.files" },
  { href: "/backup", key: "nav.backup" },
  { href: "/update", key: "nav.update" },
] as const;

export function Nav() {
  const { t, i18n } = useTranslation();
  const path = currentPath.value;
  const mode = themeMode.value;
  const themeIcon = mode === "system" ? "◐" : mode === "light" ? "☀" : "☽";
  const themeLabel = `${t("nav.theme")}: ${t(`nav.theme_${mode}`)}`;

  function navigate(e: Event, href: string) {
    e.preventDefault();
    route(href);
    currentPath.value = href;
  }

  return (
    <nav class={styles.nav}>
      <strong class={styles.logo}>{t("nav.logo")}</strong>
      {links.map((l) => (
        <a
          key={l.href}
          href={l.href}
          onClick={(e: Event) => navigate(e, l.href)}
          class={`${styles.link}${path === l.href ? ` ${styles.linkActive}` : ""}`}
          aria-current={path === l.href ? "page" : undefined}
        >
          {t(l.key)}
        </a>
      ))}
      <div class={styles.navRight}>
        <select
          id="nav-language"
          name="language"
          class={styles.langSelect}
          value={i18n.resolvedLanguage}
          onChange={(e: Event) =>
            i18n.changeLanguage((e.target as HTMLSelectElement).value)
          }
          title={t("nav.language")}
          aria-label={t("nav.language")}
        >
          {supportedLanguages.map((l) => (
            <option key={l.code} value={l.code} lang={l.code}>
              {l.label}
            </option>
          ))}
        </select>
        <button
          class="theme-toggle"
          onClick={cycleTheme}
          title={themeLabel}
          aria-label={themeLabel}
        >
          {themeIcon}
        </button>
      </div>
    </nav>
  );
}
