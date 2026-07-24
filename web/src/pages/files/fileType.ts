// File-type + policy helpers — one source of truth for icon, tint, editability
// and protection (see web/FILES-UX-PLAN.md §2.6 / §2.9).

export type FileKind = "folder" | "json" | "code" | "text" | "archive" | "image" | "binary";

const CODE = ["js", "css", "html", "htm", "ino", "cpp", "c", "h", "xml", "svg"];
const TEXT = ["txt", "md", "csv", "cfg", "ini", "log", "conf"];
const ARCHIVE = ["gz", "zip", "bin", "tar"];
const IMAGE = ["png", "jpg", "jpeg", "gif", "bmp", "webp"];

export function fileType(name: string, type: "file" | "dir"): FileKind {
  if (type === "dir") return "folder";
  const ext = name.split(".").pop()?.toLowerCase() ?? "";
  if (ext === "json") return "json";
  if (CODE.includes(ext)) return "code";
  if (TEXT.includes(ext)) return "text";
  if (ARCHIVE.includes(ext)) return "archive";
  if (IMAGE.includes(ext)) return "image";
  return "binary";
}

/** Only text kinds may be loaded into the editor — binary in a textarea corrupts. */
export function isEditable(kind: FileKind): boolean {
  return kind === "json" || kind === "code" || kind === "text";
}

/** Supplementary per-type tint; the glyph SHAPE carries the meaning (not colour). */
export function kindColor(kind: FileKind): string {
  switch (kind) {
    case "folder": return "var(--accent)";
    case "json":
    case "code": return "var(--blue)";
    case "archive": return "var(--orange)";
    case "image": return "var(--green)";
    default: return "var(--label-secondary)";
  }
}

const LOCKED = ["index.html.gz", "app.js.gz", "style.css.gz"];
const WARN = ["donottouch.json"];

/** 'lock' = no edit/delete (bricks UI); 'warn' = extra confirm; null = normal. */
export function protection(name: string): "lock" | "warn" | null {
  const n = name.toLowerCase();
  if (LOCKED.includes(n)) return "lock";
  if (WARN.includes(n)) return "warn";
  return null;
}

/** Fetch a file at its served path and save it via a temporary anchor.
 *  NOTE: relies on LittleFS files being GET-able at their path. VERIFY nested
 *  paths (/CUSTOMAPPS/...) aren't intercepted by the SPA fallback (§4). */
export async function downloadPath(path: string, name: string): Promise<void> {
  const res = await fetch(path);
  if (!res.ok) throw new Error("download failed");
  const blob = await res.blob();
  const url = URL.createObjectURL(blob);
  const a = document.createElement("a");
  a.href = url;
  a.download = name;
  document.body.appendChild(a);
  a.click();
  a.remove();
  URL.revokeObjectURL(url);
}
