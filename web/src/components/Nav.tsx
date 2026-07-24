import { route } from "preact-router";
import { signal } from "@preact/signals";
import { useTranslation } from "react-i18next";
import { supportedLanguages } from "../i18n";
import styles from "./Nav.module.css";

const currentPath = signal(window.location.pathname);
const theme = signal<"dark" | "light">(
  (localStorage.getItem("theme") as "dark" | "light") || "dark"
);
document.documentElement.setAttribute("data-theme", theme.value);

export function toggleTheme() {
  theme.value = theme.value === "dark" ? "light" : "dark";
  document.documentElement.setAttribute("data-theme", theme.value);
  localStorage.setItem("theme", theme.value);
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
        >
          {t(l.key)}
        </a>
      ))}
      <div class={styles.navRight}>
        <select
          class={styles.langSelect}
          value={i18n.resolvedLanguage}
          onChange={(e: Event) =>
            i18n.changeLanguage((e.target as HTMLSelectElement).value)
          }
          title={t("nav.language")}
          aria-label={t("nav.language")}
        >
          {supportedLanguages.map((l) => (
            <option key={l.code} value={l.code}>
              {l.label}
            </option>
          ))}
        </select>
        <button
          class="theme-toggle"
          onClick={toggleTheme}
          title={t("nav.toggleTheme")}
        >
          {theme.value === "dark" ? "\u2600" : "\u263D"}
        </button>
      </div>
    </nav>
  );
}
