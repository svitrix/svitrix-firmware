import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { uploadFile } from "../../../api/client";
import { toast } from "../../../components/Toast";
import { TextField, Card, FormRow, Button } from "../../../components/ui";
import styles from "./sections.module.css";

export function IconPickerSection() {
  const { t } = useTranslation();
  const [iconId, setIconId] = useState("");
  const [iconPreview, setIconPreview] = useState("");

  async function downloadIcon() {
    if (!iconId) { toast(t("settingsDisplay.iconPicker.enterIconId")); return; }
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
              toast(t("settingsDisplay.iconPicker.iconSaved"));
            }
          }, "image/jpeg", 1);
          URL.revokeObjectURL(url);
        };
        img.src = url;
      } else {
        await uploadFile(`/ICONS/${iconId}.gif`, blob);
        toast(t("settingsDisplay.iconPicker.iconSaved"));
      }
    } catch {
      toast(t("settingsDisplay.iconPicker.downloadFailed"));
    }
  }

  return (
    <Card title={t("settingsDisplay.iconPicker.title")}>
      <div class={styles.stack}>
        <FormRow>
          <TextField label={t("settingsDisplay.iconPicker.iconId")} value={iconId} onChange={setIconId} placeholder="13" />
          <div class={`form-group ${styles.formGroupEnd}`}>
            <div class={styles.btnGroup}>
              <Button onClick={() => {
                if (iconId) setIconPreview(`https://developer.lametric.com/content/apps/icon_thumbs/${iconId}`);
              }}>{t("settingsDisplay.iconPicker.preview")}</Button>
              <Button variant="primary" onClick={downloadIcon}>{t("settingsDisplay.iconPicker.download")}</Button>
            </div>
          </div>
        </FormRow>
        {iconPreview && (
          <div class={styles.iconPreview}>
            <img
              src={iconPreview}
              class={styles.iconPreviewImg}
              onError={() => { setIconPreview(""); toast(t("settingsDisplay.iconPicker.iconNotFound")); }}
            />
          </div>
        )}
      </div>
    </Card>
  );
}
