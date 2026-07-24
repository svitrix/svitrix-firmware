import type { FileKind } from "./fileType";
import { kindColor } from "./fileType";

/** Inline SVG glyph keyed by file kind, tinted via currentColor. No emoji, no
 *  icon font — theme-tintable and tiny. Shape carries meaning (colour-not-alone). */
export function FileIcon({ kind }: { kind: FileKind }) {
  return (
    <svg
      width="20"
      height="20"
      viewBox="0 0 24 24"
      fill="none"
      stroke={kindColor(kind)}
      stroke-width="1.7"
      stroke-linecap="round"
      stroke-linejoin="round"
      aria-hidden="true"
      style={{ flex: "0 0 auto" }}
    >
      {glyph(kind)}
    </svg>
  );
}

function glyph(kind: FileKind) {
  switch (kind) {
    case "folder":
      return <path d="M3 7a2 2 0 0 1 2-2h4l2 2h8a2 2 0 0 1 2 2v8a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2z" />;
    case "json":
      // document + braces
      return (
        <>
          <path d="M6 3h9l4 4v13a1 1 0 0 1-1 1H6a1 1 0 0 1-1-1V4a1 1 0 0 1 1-1z" />
          <path d="M10.5 10.5c-1 0-1 1-1 1.5s0 1.5-1 1.5c1 0 1 1 1 1.5s0 1.5 1 1.5" />
          <path d="M13.5 10.5c1 0 1 1 1 1.5s0 1.5 1 1.5c-1 0-1 1-1 1.5s0 1.5-1 1.5" />
        </>
      );
    case "code":
      return (
        <>
          <path d="M6 3h9l4 4v13a1 1 0 0 1-1 1H6a1 1 0 0 1-1-1V4a1 1 0 0 1 1-1z" />
          <path d="M10.5 11 9 13l1.5 2M13.5 11 15 13l-1.5 2" />
        </>
      );
    case "text":
      return (
        <>
          <path d="M6 3h9l4 4v13a1 1 0 0 1-1 1H6a1 1 0 0 1-1-1V4a1 1 0 0 1 1-1z" />
          <path d="M9 12h6M9 15h6M9 9h3" />
        </>
      );
    case "archive":
      return (
        <>
          <path d="M6 3h9l4 4v13a1 1 0 0 1-1 1H6a1 1 0 0 1-1-1V4a1 1 0 0 1 1-1z" />
          <path d="M11 3v3M13 6v3M11 9v3M13 12v2h-2v-2" />
        </>
      );
    case "image":
      return (
        <>
          <rect x="3.5" y="4.5" width="17" height="15" rx="2" />
          <circle cx="9" cy="9.5" r="1.6" />
          <path d="m4 17 4.5-4.5L13 17l3-3 4 4" />
        </>
      );
    default:
      return (
        <path d="M6 3h9l4 4v13a1 1 0 0 1-1 1H6a1 1 0 0 1-1-1V4a1 1 0 0 1 1-1z" />
      );
  }
}
