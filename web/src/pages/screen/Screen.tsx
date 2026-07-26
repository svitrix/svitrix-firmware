import { LivePreview } from "../../components/LivePreview";

export function ScreenPage(_props: { path?: string; default?: boolean }) {
  return <LivePreview variant="page" showControls />;
}
