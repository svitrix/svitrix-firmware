import { useState, useEffect, useRef } from "preact/hooks";
import { uploadFile, listDir, deleteFile, readFile, playRtttl } from "../../../api/client";
import { playRtttlLocal, stopRtttl, isRtttlPlaying } from "../../../utils/rtttlPlayer";
import type { FileEntry } from "../../../api/types";
import { toast } from "../../../components/Toast";
import { Card, Button, TextField } from "../../../components/ui";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function MelodiesSection() {
  const [melodies, setMelodies] = useState<FileEntry[]>([]);
  const [loadingMelodies, setLoadingMelodies] = useState(false);
  const [playingName, setPlayingName] = useState<string | null>(null);
  const [editorContent, setEditorContent] = useState("");
  const [editorName, setEditorName] = useState("");
  const [isPreviewPlaying, setIsPreviewPlaying] = useState(false);
  const [selectedFile, setSelectedFile] = useState<File | null>(null);
  const [fileContent, setFileContent] = useState("");
  const fileInputRef = useRef<HTMLInputElement>(null);
  const t = useT();

  async function loadMelodies() {
    setLoadingMelodies(true);
    try {
      const entries = await listDir("/MELODIES");
      const files = entries.filter(
        (e) => e.type === "file" && (e.name.endsWith(".txt") || e.name.endsWith(".rtttl"))
      );
      setMelodies(files);
    } catch {
      setMelodies([]);
    }
    setLoadingMelodies(false);
  }

  useEffect(() => {
    loadMelodies();
  }, []);

  async function handlePlayOnDevice(name: string, content?: string) {
    setPlayingName(name);
    try {
      let rtttl = content;
      if (!rtttl) {
        rtttl = await readFile(`/MELODIES/${name}`);
      }
      await playRtttl(rtttl.trim());
    } catch {
      toast(t.sound.playFailed || "Error playing melody");
    }
    setTimeout(() => setPlayingName(null), 1000);
  }

  async function handlePreviewLocal(rtttl: string) {
    if (isRtttlPlaying()) {
      stopRtttl();
      setIsPreviewPlaying(false);
      return;
    }
    if (!rtttl.trim()) {
      toast(t.sound.enterRtttl || "Enter RTTTL melody");
      return;
    }
    setIsPreviewPlaying(true);
    try {
      await playRtttlLocal(rtttl.trim());
    } catch {
      toast(t.sound.invalidRtttl || "Invalid RTTTL format");
    }
    setIsPreviewPlaying(false);
  }

  async function handleDelete(name: string) {
    if (!confirm(`${t.sound.confirmDelete || "Delete"} "${name}"?`)) return;
    try {
      await deleteFile(`/MELODIES/${name}`);
      toast(t.sound.melodyDeleted || "Melody deleted");
      loadMelodies();
    } catch {
      toast(t.sound.deleteFailed || "Delete failed");
    }
  }

  async function handleUploadFromEditor() {
    if (!editorContent.trim()) {
      toast(t.sound.enterRtttl || "Enter RTTTL melody");
      return;
    }
    const name = editorName.trim() || extractMelodyName(editorContent) || "melody";
    const filename = name.endsWith(".txt") ? name : `${name}.txt`;
    try {
      await uploadFile(`/MELODIES/${filename}`, editorContent.trim());
      toast(t.sound.melodySaved || "Melody saved");
      setEditorContent("");
      setEditorName("");
      loadMelodies();
    } catch {
      toast(t.sound.saveFailed || "Save failed");
    }
  }

  async function handleUploadFile() {
    if (!selectedFile || !fileContent.trim()) {
      toast(t.sound.selectFile || "Select a file first");
      return;
    }
    const filename = selectedFile.name.endsWith(".txt") || selectedFile.name.endsWith(".rtttl")
      ? selectedFile.name
      : `${selectedFile.name}.txt`;
    try {
      await uploadFile(`/MELODIES/${filename}`, fileContent.trim());
      toast(t.sound.melodySaved || "Melody saved");
      setSelectedFile(null);
      setFileContent("");
      if (fileInputRef.current) fileInputRef.current.value = "";
      loadMelodies();
    } catch {
      toast(t.sound.saveFailed || "Save failed");
    }
  }

  function handleFileSelect(e: Event) {
    const input = e.target as HTMLInputElement;
    const file = input.files?.[0];
    if (!file) return;
    setSelectedFile(file);
    const reader = new FileReader();
    reader.onload = (ev) => {
      setFileContent((ev.target?.result as string) || "");
    };
    reader.readAsText(file);
  }

  function extractMelodyName(rtttl: string): string {
    const match = rtttl.match(/^([^:]+):/);
    return match ? match[1].trim() : "";
  }

  function copyToEditor(content: string) {
    setEditorContent(content);
    setEditorName(extractMelodyName(content));
  }

  return (
    <>
      {/* Saved Melodies */}
      <Card title={t.sound.savedMelodies || "Saved Melodies"}>
        <div class={styles.stack}>
          <div class={styles.actions}>
            <Button onClick={loadMelodies} disabled={loadingMelodies}>
              {loadingMelodies ? t.loading : t.sound.refresh || "Refresh"}
            </Button>
          </div>
          {melodies.length === 0 ? (
            <p class={styles.hint}>{t.sound.noMelodies || "No saved melodies"}</p>
          ) : (
            <div class={styles.melodyList}>
              {melodies.map((melody) => (
                <div key={melody.name} class={styles.melodyItem}>
                  <span class={styles.melodyName}>
                    {melody.name.replace(/\.(txt|rtttl)$/, "")}
                  </span>
                  <div class={styles.melodyActions}>
                    <button
                      class={styles.melodyBtn}
                      onClick={() => handlePlayOnDevice(melody.name)}
                      disabled={playingName === melody.name}
                      title={t.sound.playOnDevice || "Play on device"}
                    >
                      {playingName === melody.name ? "..." : "▶"}
                    </button>
                    <button
                      class={styles.melodyBtn}
                      onClick={async () => {
                        const content = await readFile(`/MELODIES/${melody.name}`);
                        copyToEditor(content);
                      }}
                      title={t.sound.editMelody || "Edit"}
                    >
                      ✎
                    </button>
                    <button
                      class={`${styles.melodyBtn} ${styles.melodyBtnDanger}`}
                      onClick={() => handleDelete(melody.name)}
                      title={t.sound.deleteMelody || "Delete"}
                    >
                      ✕
                    </button>
                  </div>
                </div>
              ))}
            </div>
          )}
        </div>
      </Card>

      {/* RTTTL Editor */}
      <Card title={t.sound.rtttlEditor || "RTTTL Editor"}>
        <div class={styles.stack}>
          <TextField
            label={t.sound.melodyName || "Melody name"}
            value={editorName}
            onChange={setEditorName}
            placeholder="MyMelody"
          />
          <div class="form-group">
            <label>{t.sound.rtttlContent || "RTTTL content"}</label>
            <textarea
              class={styles.rtttlTextarea}
              value={editorContent}
              onInput={(e) => setEditorContent((e.target as HTMLTextAreaElement).value)}
              placeholder="name:d=4,o=5,b=140:c,e,g,c6"
              rows={4}
            />
          </div>
          <p class={styles.hint}>
            {t.sound.rtttlHint || "Format: name:d=duration,o=octave,b=bpm:notes"}
          </p>
          <div class={styles.actions}>
            <Button
              onClick={() => handlePreviewLocal(editorContent)}
              disabled={!editorContent.trim()}
            >
              {isPreviewPlaying ? "⏹ " + (t.sound.stop || "Stop") : "🔊 " + (t.sound.previewLocal || "Preview")}
            </Button>
            <Button
              onClick={() => handlePlayOnDevice("editor", editorContent)}
              disabled={!editorContent.trim() || playingName === "editor"}
            >
              {playingName === "editor" ? "..." : "▶ " + (t.sound.playOnDevice || "Play on device")}
            </Button>
            <Button variant="primary" onClick={handleUploadFromEditor} disabled={!editorContent.trim()}>
              {t.sound.saveToDevice || "Save to device"}
            </Button>
          </div>
        </div>
      </Card>

      {/* Upload from File */}
      <Card title={t.sound.uploadMelody || "Upload Melody"}>
        <div class={styles.stack}>
          <div class="form-group">
            <label>{t.sound.selectFile || "Select file"}</label>
            <input
              ref={fileInputRef}
              type="file"
              accept=".txt,.rtttl"
              onChange={handleFileSelect}
              class={styles.fileInput}
            />
          </div>
          {selectedFile && fileContent && (
            <>
              <div class={styles.filePreview}>
                <div class={styles.filePreviewHeader}>
                  <span class={styles.fileName}>{selectedFile.name}</span>
                </div>
                <pre class={styles.filePreviewContent}>{fileContent}</pre>
              </div>
              <div class={styles.actions}>
                <Button onClick={() => handlePreviewLocal(fileContent)}>
                  {isPreviewPlaying ? "⏹ " + (t.sound.stop || "Stop") : "🔊 " + (t.sound.previewLocal || "Preview")}
                </Button>
                <Button variant="primary" onClick={handleUploadFile}>
                  {t.sound.uploadToDevice || "Upload to device"}
                </Button>
              </div>
            </>
          )}
          {!selectedFile && (
            <p class={styles.hint}>{t.sound.uploadHint || "Select a .txt or .rtttl file with RTTTL melody"}</p>
          )}
        </div>
      </Card>
    </>
  );
}
