import { route } from "preact-router";
import { signal } from "@preact/signals";
import { useTranslation } from "../i18n";
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

const linkKeys = [
  { href: "/", key: "settings" },
  { href: "/mqtt", key: "mqtt" },
  { href: "/display", key: "display" },
  { href: "/apps", key: "apps" },
  { href: "/datetime", key: "datetime" },
  { href: "/sound", key: "sound" },
  { href: "/screen", key: "screen" },
  { href: "/datafetcher", key: "data" },
  { href: "/autonomous", key: "alarms" },
  { href: "/files", key: "files" },
  { href: "/icons", key: "icons" },
  { href: "/backup", key: "backup" },
  { href: "/system", key: "system" },
  { href: "/update", key: "update" },
] as const;

export function Nav() {
  const path = currentPath.value;
  const { lang, setLang, t } = useTranslation();

  function navigate(e: Event, href: string) {
    e.preventDefault();
    route(href);
    currentPath.value = href;
  }

  function toggleLang() {
    setLang(lang === "es" ? "en" : "es");
  }

  return (
    <nav class={styles.nav}>
      <strong class={styles.logo}>Svitrix XE1E</strong>
      {linkKeys.map((l) => (
        <a
          key={l.href}
          href={l.href}
          onClick={(e: Event) => navigate(e, l.href)}
          class={`${styles.link}${path === l.href ? ` ${styles.linkActive}` : ""}`}
        >
          {t.nav[l.key]}
        </a>
      ))}
      <button
        class={`theme-toggle ${styles.themeToggle}`}
        onClick={toggleLang}
        title={lang === "es" ? "Switch to English" : "Cambiar a Espa\u00F1ol"}
      >
        {lang.toUpperCase()}
      </button>
      <button
        class={`theme-toggle ${styles.themeToggle}`}
        onClick={toggleTheme}
        title="Toggle theme"
      >
        {theme.value === "dark" ? "\u2600" : "\u263D"}
      </button>
    </nav>
  );
}
