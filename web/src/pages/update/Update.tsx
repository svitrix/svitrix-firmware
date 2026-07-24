import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { toast } from "../../components/Toast";
import styles from "./Update.module.css";

export function UpdatePage(_props: { path?: string }) {
  const { t } = useTranslation();
  const [uploading, setUploading] = useState(false);
  const [progress, setProgress] = useState("");

  async function handleUpload(file: File) {
    setUploading(true);
    setProgress(t("update.uploading"));
    try {
      const form = new FormData();
      form.append("update", file);
      const res = await fetch("/update", { method: "POST", body: form });
      if (res.ok) {
        setProgress(t("update.uploadComplete"));
        toast(t("update.toastUploaded"));
      } else {
        setProgress(t("update.uploadFailedStatus", { status: res.statusText }));
        toast(t("update.toastFailed"));
      }
    } catch {
      setProgress(t("update.uploadFailedConnection"));
    }
    setUploading(false);
  }

  return (
    <div class={styles.page}>
      <h2>{t("update.title")}</h2>

      <div class="card">
        <h3 class={styles.cardHeading}>{t("update.otaHeading")}</h3>
        <p class={styles.hint}>{t("update.otaHint")}</p>
        <input
          type="file"
          accept=".bin,.bin.gz"
          disabled={uploading}
          onChange={(e) => {
            const file = (e.target as HTMLInputElement).files?.[0];
            if (file) handleUpload(file);
          }}
        />
        {progress && (
          <p class={styles.progress}>{progress}</p>
        )}
      </div>
    </div>
  );
}
