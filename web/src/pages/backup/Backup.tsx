import { useState } from "preact/hooks";
import { listDir, uploadFile, reboot, exportSettings, importSettings } from "../../api/client";
import type { FileEntry } from "../../api/types";
import { toast } from "../../components/Toast";
import { FileInput } from "../../components/ui";
import { useT } from "../../i18n";
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
  const t = useT();

  async function doBackup() {
    setBusy("...");
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
      toast(t.ok);
    } catch {
      toast(t.error);
    }
    setBusy("");
  }

  async function doRestore(file: File) {
    setBusy("...");
    try {
      const text = await file.text();
      const raw = JSON.parse(text);

      const backup: BackupData = raw.version === 2
        ? raw
        : { files: raw, version: 1 };

      const paths = Object.keys(backup.files);
      let done = 0;
      for (const path of paths) {
        setBusy(`${++done}/${paths.length}...`);
        const binary = atob(backup.files[path]);
        const bytes = new Uint8Array(binary.length);
        for (let i = 0; i < binary.length; i++) bytes[i] = binary.charCodeAt(i);
        await uploadFile(path, new Blob([bytes]));
      }

      if (backup.settings) {
        await importSettings(backup.settings);
      }

      toast(t.ok);
      await reboot();
    } catch {
      toast(t.error);
    }
    setBusy("");
  }

  return (
    <div class={styles.page}>
      <h2>{t.backup.title}</h2>

      <div class="card">
        <h3 class={styles.cardHeading}>{t.backup.backup}</h3>
        <p class={styles.hint}>{t.backup.backupHint}</p>
        <button class="btn-primary" onClick={doBackup} disabled={!!busy}>
          {busy || t.backup.downloadBackup}
        </button>
      </div>

      <div class="card">
        <h3 class={styles.cardHeading}>{t.backup.restore}</h3>
        <p class={styles.hint}>{t.backup.restoreHint}</p>
        <FileInput
          accept=".json"
          disabled={!!busy}
          onChange={doRestore}
          buttonText={t.chooseFile}
        />
      </div>
    </div>
  );
}
