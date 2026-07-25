import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { listDir, uploadFile, reboot } from "../../api/client";
import type { FileEntry } from "../../api/types";
import { toast } from "../../components/Toast";
import { ConfirmDialog } from "../../components/ui";
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
  const { t } = useTranslation();
  const [busy, setBusy] = useState("");
  const [pendingRestore, setPendingRestore] = useState<File | null>(null);

  async function doBackup() {
    setBusy(t("backup.collecting"));
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
      toast(t("backup.backupDownloaded"));
    } catch {
      toast(t("backup.backupFailed"));
    }
    setBusy("");
  }

  async function doRestore(file: File) {
    setBusy(t("backup.restoring"));
    try {
      const text = await file.text();
      const backup: Record<string, string> = JSON.parse(text);
      const paths = Object.keys(backup);
      let done = 0;
      for (const path of paths) {
        setBusy(
          t("backup.restoringProgress", { done: ++done, total: paths.length }),
        );
        const binary = atob(backup[path]);
        const bytes = new Uint8Array(binary.length);
        for (let i = 0; i < binary.length; i++) bytes[i] = binary.charCodeAt(i);
        await uploadFile(path, new Blob([bytes]));
      }
      toast(t("backup.restoreComplete"));
      await reboot();
    } catch {
      toast(t("backup.restoreFailed"), { error: true });
    }
    setBusy("");
  }

  return (
    <div class={styles.page}>
      <h2>{t("backup.title")}</h2>

      <div class="card">
        <h3 class={styles.cardHeading}>{t("backup.backupHeading")}</h3>
        <p class={styles.hint}>{t("backup.backupHint")}</p>
        <button class="btn-primary" onClick={doBackup} disabled={!!busy}>
          {busy || t("backup.downloadBackup")}
        </button>
      </div>

      <div class="card">
        <h3 class={styles.cardHeading}>{t("backup.restoreHeading")}</h3>
        <p class={styles.hint}>{t("backup.restoreHint")}</p>
        <input
          type="file"
          accept=".json"
          aria-label={t("backup.fileLabel")}
          disabled={!!busy}
          onChange={(e) => {
            const input = e.target as HTMLInputElement;
            const file = input.files?.[0];
            if (file) setPendingRestore(file);
            // Reset so re-selecting the same file re-triggers change.
            input.value = "";
          }}
        />
      </div>

      <ConfirmDialog
        open={!!pendingRestore}
        title={t("backup.confirmRestoreTitle")}
        body={t("backup.confirmRestoreBody")}
        confirmLabel={t("backup.confirmRestore")}
        danger
        onConfirm={() => {
          const file = pendingRestore;
          setPendingRestore(null);
          if (file) doRestore(file);
        }}
        onCancel={() => setPendingRestore(null)}
      />
    </div>
  );
}
