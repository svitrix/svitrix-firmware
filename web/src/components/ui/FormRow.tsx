import type { ComponentChildren, JSX } from "preact";

export function FormRow({
  children,
  style,
}: {
  children: ComponentChildren;
  style?: JSX.CSSProperties;
}) {
  return <div class="form-row" style={style}>{children}</div>;
}
