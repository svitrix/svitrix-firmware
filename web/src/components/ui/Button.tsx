import type { ComponentChildren } from "preact";

export function Button({
  children,
  variant,
  onClick,
  disabled,
  loading,
}: {
  children: ComponentChildren;
  variant?: "primary" | "danger" | "default";
  onClick?: () => void;
  disabled?: boolean;
  loading?: boolean;
}) {
  const cls =
    variant === "primary" ? "btn-primary" :
    variant === "danger" ? "btn-danger" :
    undefined;

  return (
    <button class={cls} onClick={onClick} disabled={disabled || loading}>
      {loading ? "..." : children}
    </button>
  );
}
