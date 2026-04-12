import { route } from "preact-router";
import { signal } from "@preact/signals";
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
  { href: "/", label: "Screen" },
  { href: "/settings", label: "Settings" },
  { href: "/datafetcher", label: "Data" },
  { href: "/files", label: "Files" },
  { href: "/backup", label: "Backup" },
  { href: "/update", label: "Update" },
];

export function Nav() {
  const path = currentPath.value;

  function navigate(e: Event, href: string) {
    e.preventDefault();
    route(href);
    currentPath.value = href;
  }

  return (
    <nav class={styles.nav}>
      <strong class={styles.logo}>Svitrix</strong>
      {links.map((l) => (
        <a
          key={l.href}
          href={l.href}
          onClick={(e: Event) => navigate(e, l.href)}
          class={`${styles.link}${path === l.href ? ` ${styles.linkActive}` : ""}`}
        >
          {l.label}
        </a>
      ))}
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
