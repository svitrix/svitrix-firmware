import { useState } from "preact/hooks";
import { listDir, uploadFile, reboot } from "../api/client";
import type { FileEntry } from "../api/types";
import { toast } from "../components/Toast";
import styles from "./Backup.module.css";

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

      const backup: Record<string, string> = {};
      for (const f of files) {
        const buf = await f.blob.arrayBuffer();
        const bytes = new Uint8Array(buf);
        let binary = "";
        for (let i = 0; i < bytes.length; i++) binary += String.fromCharCode(bytes[i]);
        backup[f.path] = btoa(binary);
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
        const binary = atob(backup[path]);
        const bytes = new Uint8Array(binary.length);
        for (let i = 0; i < binary.length; i++) bytes[i] = binary.charCodeAt(i);
        await uploadFile(path, new Blob([bytes]));
      }
      toast("Restore complete! Rebooting...");
      await reboot();
    } catch {
      toast("Restore failed");
    }
    setBusy("");
  }

  return (
    <div class={styles.page}>
      <h2>Backup & Restore</h2>

      <div class="card">
        <h3 class={styles.cardHeading}>Backup</h3>
        <p class={styles.hint}>
          Download all files from device filesystem as a JSON backup.
        </p>
        <button class="btn-primary" onClick={doBackup} disabled={!!busy}>
          {busy || "Download Backup"}
        </button>
      </div>

      <div class="card">
        <h3 class={styles.cardHeading}>Restore</h3>
        <p class={styles.hint}>
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
