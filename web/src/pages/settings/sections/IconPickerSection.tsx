import { useState, useEffect } from "preact/hooks";
import { uploadFile, listDir, deleteFile } from "../../../api/client";
import type { FileEntry } from "../../../api/types";
import { toast } from "../../../components/Toast";
import { TextField, Card, FormRow, Button } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function IconPickerSection() {
  const [iconId, setIconId] = useState("");
  const [iconPreview, setIconPreview] = useState("");
  const [localIcons, setLocalIcons] = useState<FileEntry[]>([]);
  const [loadingIcons, setLoadingIcons] = useState(false);
  const t = useT();

  async function loadLocalIcons() {
    setLoadingIcons(true);
    try {
      const entries = await listDir("/ICONS");
      const icons = entries.filter(e =>
        e.type === "file" && (e.name.endsWith(".jpg") || e.name.endsWith(".gif"))
      );
      setLocalIcons(icons);
    } catch {
      setLocalIcons([]);
    }
    setLoadingIcons(false);
  }

  useEffect(() => {
    loadLocalIcons();
  }, []);

  async function downloadIcon() {
    if (!iconId) { toast(t.icons.enterIconId); return; }
    try {
      const res = await fetch(
        `https://developer.lametric.com/content/apps/icon_thumbs/${iconId}`
      );
      const blob = await res.blob();
      const ct = res.headers.get("content-type") || "";
      let ext = ".jpg";
      if (ct.includes("gif")) ext = ".gif";

      if (ext === ".jpg" && (ct.includes("jpeg") || ct.includes("png"))) {
        const img = new Image();
        const url = URL.createObjectURL(blob);
        img.onload = async () => {
          const canvas = document.createElement("canvas");
          canvas.width = img.width;
          canvas.height = img.height;
          canvas.getContext("2d")!.drawImage(img, 0, 0);
          canvas.toBlob(async (jpgBlob) => {
            if (jpgBlob) {
              await uploadFile(`/ICONS/${iconId}.jpg`, jpgBlob);
              toast(t.icons.iconSaved);
              loadLocalIcons();
            }
          }, "image/jpeg", 1);
          URL.revokeObjectURL(url);
        };
        img.src = url;
      } else {
        await uploadFile(`/ICONS/${iconId}.gif`, blob);
        toast(t.icons.iconSaved);
        loadLocalIcons();
      }
    } catch {
      toast(t.icons.iconDownloadFailed);
    }
  }

  async function handleDeleteIcon(name: string) {
    if (!confirm(`${t.icons.confirmDelete} "${name}"?`)) return;
    try {
      await deleteFile(`/ICONS/${name}`);
      toast(t.icons.iconDeleted);
      loadLocalIcons();
    } catch {
      toast(t.icons.deleteFailed);
    }
  }

  return (
    <>
      <Card title={t.icons.title}>
        <div class={styles.stack}>
          <FormRow>
            <TextField label={t.icons.lametricId} value={iconId} onChange={setIconId} placeholder="13" />
            <div class={`form-group ${styles.formGroupEnd}`}>
              <div class={styles.btnGroup}>
                <Button onClick={() => {
                  if (iconId) setIconPreview(`https://developer.lametric.com/content/apps/icon_thumbs/${iconId}`);
                }}>{t.icons.preview}</Button>
                <Button variant="primary" onClick={downloadIcon}>{t.icons.download}</Button>
              </div>
            </div>
          </FormRow>
          {iconPreview && (
            <div class={styles.iconPreview}>
              <img
                src={iconPreview}
                class={styles.iconPreviewImg}
                onError={() => { setIconPreview(""); toast(t.icons.iconNotFound); }}
              />
            </div>
          )}
        </div>
      </Card>

      <Card title={t.icons.localIcons}>
        <div class={styles.stack}>
          <div class={styles.actions}>
            <Button onClick={loadLocalIcons} disabled={loadingIcons}>
              {loadingIcons ? t.loading : t.icons.refresh}
            </Button>
          </div>
          {localIcons.length === 0 ? (
            <p class={styles.hint}>{t.icons.noLocalIcons}</p>
          ) : (
            <div class={styles.iconGrid}>
              {localIcons.map(icon => (
                <div key={icon.name} class={styles.iconGridItem}>
                  <img
                    src={`/ICONS/${icon.name}?t=${Date.now()}`}
                    alt={icon.name}
                    class={styles.iconGridImg}
                  />
                  <div class={styles.iconGridName}>{icon.name.replace(/\.(jpg|gif)$/, "")}</div>
                  <button
                    class={styles.iconGridDelete}
                    onClick={() => handleDeleteIcon(icon.name)}
                    title={t.icons.delete}
                  >
                    &times;
                  </button>
                </div>
              ))}
            </div>
          )}
        </div>
      </Card>
    </>
  );
}
