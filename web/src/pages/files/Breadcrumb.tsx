import { useTranslation } from "react-i18next";
import styles from "./Files.module.css";

/** Home icon + chevron-separated focusable segments; the strip scrolls, never wraps. */
export function Breadcrumb({ path, onNavigate }: { path: string; onNavigate: (p: string) => void }) {
  const { t } = useTranslation();
  const parts = path.split("/").filter(Boolean);

  return (
    <nav class={styles.breadcrumb} aria-label={t("files.breadcrumb")}>
      <button
        class={styles.crumb}
        onClick={() => onNavigate("/")}
        aria-label={t("files.home")}
        aria-current={parts.length === 0 ? "page" : undefined}
      >
        <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">
          <path d="M4 11 12 4l8 7" />
          <path d="M6 10v9h12v-9" />
        </svg>
      </button>
      {parts.map((part, i) => {
        const p = "/" + parts.slice(0, i + 1).join("/");
        const last = i === parts.length - 1;
        return (
          <span class={styles.crumbGroup} key={p}>
            <svg class={styles.chevron} width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">
              <path d="m9 6 6 6-6 6" />
            </svg>
            {last ? (
              <span class={`${styles.crumb} ${styles.crumbCurrent}`} aria-current="page">{part}</span>
            ) : (
              <button class={styles.crumb} onClick={() => onNavigate(p)}>{part}</button>
            )}
          </span>
        );
      })}
    </nav>
  );
}
