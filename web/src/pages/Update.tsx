import { useState } from "preact/hooks";
import { toast } from "../components/Toast";

export function UpdatePage(_props: { path?: string }) {
  const [uploading, setUploading] = useState(false);
  const [progress, setProgress] = useState("");

  async function handleUpload(file: File) {
    setUploading(true);
    setProgress("Uploading firmware...");
    try {
      const form = new FormData();
      form.append("update", file);
      const res = await fetch("/update", { method: "POST", body: form });
      if (res.ok) {
        setProgress("Upload complete! Device is rebooting...");
        toast("Firmware uploaded!");
      } else {
        setProgress(`Upload failed: ${res.statusText}`);
        toast("Upload failed");
      }
    } catch {
      setProgress("Upload failed — connection lost (device may be rebooting)");
    }
    setUploading(false);
  }

  return (
    <div style={{ display: "flex", flexDirection: "column", gap: 16 }}>
      <h2>Firmware Update</h2>

      <div class="card">
        <h3 style={{ marginBottom: 12 }}>OTA Update</h3>
        <p style={{ color: "var(--text-dim)", marginBottom: 12, fontSize: 13 }}>
          Select a firmware .bin file to upload. The device will reboot automatically after a
          successful update.
        </p>
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
          <p style={{ marginTop: 12, color: "var(--accent)" }}>{progress}</p>
        )}
      </div>
    </div>
  );
}
