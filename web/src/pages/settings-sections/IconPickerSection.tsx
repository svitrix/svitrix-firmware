import { useState } from "preact/hooks";
import { uploadFile } from "../../api/client";
import { toast } from "../../components/Toast";
import { TextField, Card, FormRow, Button } from "../../components/ui";

export function IconPickerSection() {
  const [iconId, setIconId] = useState("");
  const [iconPreview, setIconPreview] = useState("");

  async function downloadIcon() {
    if (!iconId) { toast("Enter icon ID"); return; }
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
              toast("Icon saved!");
            }
          }, "image/jpeg", 1);
          URL.revokeObjectURL(url);
        };
        img.src = url;
      } else {
        await uploadFile(`/ICONS/${iconId}.gif`, blob);
        toast("Icon saved!");
      }
    } catch {
      toast("Icon download failed");
    }
  }

  return (
    <Card title="Icon Picker">
      <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
        <FormRow>
          <TextField label="LaMetric Icon ID" value={iconId} onChange={setIconId} placeholder="13" />
          <div class="form-group" style={{ justifyContent: "flex-end" }}>
            <div style={{ display: "flex", gap: 6 }}>
              <Button onClick={() => {
                if (iconId) setIconPreview(`https://developer.lametric.com/content/apps/icon_thumbs/${iconId}`);
              }}>Preview</Button>
              <Button variant="primary" onClick={downloadIcon}>Download</Button>
            </div>
          </div>
        </FormRow>
        {iconPreview && (
          <div style={{
            width: 150, height: 150, background: "#000",
            borderRadius: "var(--radius)", border: "1px solid var(--border)",
            display: "flex", alignItems: "center", justifyContent: "center",
          }}>
            <img
              src={iconPreview}
              style={{ imageRendering: "pixelated", maxWidth: 150, maxHeight: 150 }}
              onError={() => { setIconPreview(""); toast("Icon not found"); }}
            />
          </div>
        )}
      </div>
    </Card>
  );
}
