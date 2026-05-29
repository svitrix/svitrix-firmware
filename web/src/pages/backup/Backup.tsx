import { useState } from "preact/hooks";
import { listDir, uploadFile, reboot, exportSettings, importSettings } from "../../api/client";
import type { FileEntry } from "../../api/types";
import { toast } from "../../components/Toast";
import { FileInput } from "../../components/ui";
import styles from "./Backup.module.css";

interface BackupData {
  files: Record<string, string>;
  settings?: Record<string, unknown>;
  version?: number;
}

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

      const fileData: Record<string, string> = {};
      for (const f of files) {
        const buf = await f.blob.arrayBuffer();
        const bytes = new Uint8Array(buf);
        let binary = "";
        for (let i = 0; i < bytes.length; i++) binary += String.fromCharCode(bytes[i]);
        fileData[f.path] = btoa(binary);
      }

      setBusy("Exporting settings...");
      const settings = await exportSettings();

      const backup: BackupData = {
        version: 2,
        files: fileData,
        settings,
      };

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
      const raw = JSON.parse(text);

      // Support both old format (flat file map) and new format (with settings)
      const backup: BackupData = raw.version === 2
        ? raw
        : { files: raw, version: 1 };

      const paths = Object.keys(backup.files);
      let done = 0;
      for (const path of paths) {
        setBusy(`Restoring files ${++done}/${paths.length}...`);
        const binary = atob(backup.files[path]);
        const bytes = new Uint8Array(binary.length);
        for (let i = 0; i < binary.length; i++) bytes[i] = binary.charCodeAt(i);
        await uploadFile(path, new Blob([bytes]));
      }

      if (backup.settings) {
        setBusy("Restoring settings...");
        await importSettings(backup.settings);
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
          Download all files and settings from device as a JSON backup.
          Includes: icons, melodies, custom apps, alarms, and all configuration.
        </p>
        <button class="btn-primary" onClick={doBackup} disabled={!!busy}>
          {busy || "Download Backup"}
        </button>
      </div>

      <div class="card">
        <h3 class={styles.cardHeading}>Restore</h3>
        <p class={styles.hint}>
          Upload a previously downloaded backup file. Files and settings will be restored. Device will reboot after restore.
        </p>
        <FileInput
          accept=".json"
          disabled={!!busy}
          onChange={doRestore}
          buttonText="Choose File"
        />
      </div>
    </div>
  );
}
