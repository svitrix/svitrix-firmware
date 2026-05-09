import type { ComponentChildren, JSX } from "preact";

export function FormRow({
  children,
  style,
  centered,
}: {
  children: ComponentChildren;
  style?: JSX.CSSProperties;
  centered?: boolean;
}) {
  return <div class={centered ? "form-row-centered" : "form-row"} style={style}>{children}</div>;
}
