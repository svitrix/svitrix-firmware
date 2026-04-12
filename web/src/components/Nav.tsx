import { route } from "preact-router";
import { signal } from "@preact/signals";

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
    <nav
      style={{
        display: "flex",
        gap: "4px",
        padding: "8px 12px",
        background: "var(--bg-card)",
        borderBottom: "1px solid var(--border)",
        flexWrap: "wrap",
        alignItems: "center",
      }}
    >
      <strong style={{ color: "var(--accent)", marginRight: 12 }}>Svitrix</strong>
      {links.map((l) => (
        <a
          key={l.href}
          href={l.href}
          onClick={(e: Event) => navigate(e, l.href)}
          style={{
            padding: "4px 12px",
            borderRadius: "var(--radius)",
            background: path === l.href ? "var(--bg-input)" : "transparent",
            color: path === l.href ? "var(--accent)" : "var(--text-dim)",
            fontSize: 13,
          }}
        >
          {l.label}
        </a>
      ))}
      <button
        class="theme-toggle"
        onClick={toggleTheme}
        title="Toggle theme"
        style={{ marginLeft: "auto" }}
      >
        {theme.value === "dark" ? "\u2600" : "\u263D"}
      </button>
    </nav>
  );
}
