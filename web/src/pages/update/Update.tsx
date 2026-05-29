import { useState } from "preact/hooks";
import { toast } from "../../components/Toast";
import { FileInput } from "../../components/ui";
import { useT } from "../../i18n";
import styles from "./Update.module.css";

export function UpdatePage(_props: { path?: string }) {
  const [uploading, setUploading] = useState(false);
  const [progress, setProgress] = useState("");
  const t = useT();

  async function handleUpload(file: File) {
    setUploading(true);
    setProgress(t.update.uploading);
    try {
      const form = new FormData();
      form.append("update", file);
      const res = await fetch("/update", { method: "POST", body: form });
      if (res.ok) {
        setProgress(t.update.uploadComplete);
        toast("OK!");
      } else {
        setProgress(t.update.uploadFailed);
        toast(t.update.uploadFailed);
      }
    } catch {
      setProgress(t.update.uploadFailed);
    }
    setUploading(false);
  }

  return (
    <div class={styles.page}>
      <h2>{t.update.title}</h2>

      <div class="card">
        <h3 class={styles.cardHeading}>{t.update.otaUpdate}</h3>
        <p class={styles.hint}>{t.update.hint}</p>
        <FileInput
          accept=".bin,.bin.gz"
          disabled={uploading}
          onChange={handleUpload}
        />
        {progress && (
          <p class={styles.progress}>{progress}</p>
        )}
      </div>
    </div>
  );
}
