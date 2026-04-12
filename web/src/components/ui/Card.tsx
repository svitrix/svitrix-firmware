import type { ComponentChildren } from "preact";

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
      <h3 style={{ marginBottom: 12 }}>{title}</h3>
      {subtitle && (
        <p style={{ color: "var(--text-dim)", marginBottom: 12, fontSize: 13 }}>{subtitle}</p>
      )}
      {children}
    </div>
  );
}
