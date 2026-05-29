import { useState } from "preact/hooks";
import { uploadFile } from "../../../api/client";
import { toast } from "../../../components/Toast";
import { TextField, Card, FormRow, Button } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function IconPickerSection() {
  const [iconId, setIconId] = useState("");
  const [iconPreview, setIconPreview] = useState("");
  const t = useT();

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
            }
          }, "image/jpeg", 1);
          URL.revokeObjectURL(url);
        };
        img.src = url;
      } else {
        await uploadFile(`/ICONS/${iconId}.gif`, blob);
        toast(t.icons.iconSaved);
      }
    } catch {
      toast(t.icons.iconDownloadFailed);
    }
  }

  return (
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
  );
}
