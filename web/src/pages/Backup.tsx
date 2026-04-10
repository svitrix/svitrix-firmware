import { useState } from "preact/hooks";
import { listDir, uploadFile, reboot } from "../api/client";
import type { FileEntry } from "../api/types";
import { toast } from "../components/Toast";

async function collectFiles(
  dir: string,
  result: { path: string; blob: Blob }[],
) {
  const entries: FileEntry[] = await listDir(dir);
  for (const entry of entries) {
    const fullPath = dir === "/" ? `/${entry.name}` : `${dir}/${entry.name}`;
    if (entry.type === "dir") {
      await collectFiles(fullPath, result);
    } else {
      const res = await fetch(fullPath);
      result.push({ path: fullPath, blob: await res.blob() });
    }
  }
}

export function BackupPage(_props: { path?: string }) {
  const [busy, setBusy] = useState("");

  async function doBackup() {
    setBusy("Collecting files...");
    try {
      const files: { path: string; blob: Blob }[] = [];
      await collectFiles("/", files);

      // Build a simple tar-like JSON manifest + files as a zip using JSZip if available,
      // or fall back to downloading files individually
      // For simplicity, download as a JSON backup of the file listing
      const backup: Record<string, string> = {};
      for (const f of files) {
        backup[f.path] = await f.blob.text();
      }
      const blob = new Blob([JSON.stringify(backup, null, 2)], {
        type: "application/json",
      });
      const a = document.createElement("a");
      a.href = URL.createObjectURL(blob);
      a.download = "svitrix-backup.json";
      a.click();
      URL.revokeObjectURL(a.href);
      toast("Backup downloaded!");
    } catch {
      toast("Backup failed");
    }
    setBusy("");
  }

  async function doRestore(file: File) {
    setBusy("Restoring...");
    try {
      const text = await file.text();
      const backup: Record<string, string> = JSON.parse(text);
      const paths = Object.keys(backup);
      let done = 0;
      for (const path of paths) {
        setBusy(`Restoring ${++done}/${paths.length}...`);
        await uploadFile(path, backup[path]);
      }
      toast("Restore complete! Rebooting...");
      await reboot();
    } catch {
      toast("Restore failed");
    }
    setBusy("");
  }

  return (
    <div style={{ display: "flex", flexDirection: "column", gap: 16 }}>
      <h2>Backup & Restore</h2>

      <div class="card">
        <h3 style={{ marginBottom: 12 }}>Backup</h3>
        <p style={{ color: "var(--text-dim)", marginBottom: 12, fontSize: 13 }}>
          Download all files from device filesystem as a JSON backup.
        </p>
        <button class="btn-primary" onClick={doBackup} disabled={!!busy}>
          {busy || "Download Backup"}
        </button>
      </div>

      <div class="card">
        <h3 style={{ marginBottom: 12 }}>Restore</h3>
        <p style={{ color: "var(--text-dim)", marginBottom: 12, fontSize: 13 }}>
          Upload a previously downloaded backup file. Device will reboot after restore.
        </p>
        <input
          type="file"
          accept=".json"
          disabled={!!busy}
          onChange={(e) => {
            const file = (e.target as HTMLInputElement).files?.[0];
            if (file) doRestore(file);
          }}
        />
      </div>
    </div>
  );
}
