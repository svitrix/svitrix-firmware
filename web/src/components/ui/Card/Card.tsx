import type { ComponentChildren } from "preact";
import styles from "./Card.module.css";

export function Card({
  title,
  subtitle,
  children,
}: {
  title: string;
  subtitle?: string;
  children: ComponentChildren;
}) {
  return (
    <div class="card">
      <h3 class={styles.title}>{title}</h3>
      {subtitle && (
        <p class={styles.subtitle}>{subtitle}</p>
      )}
      {children}
    </div>
  );
}
