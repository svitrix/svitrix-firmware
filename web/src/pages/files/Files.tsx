import { useState, useEffect } from "preact/hooks";
import {
  listDir,
  readFile,
  uploadFile,
  deleteFile,
  createDir,
} from "../../api/client";
import type { FileEntry } from "../../api/types";
import { toast } from "../../components/Toast";
import styles from "./Files.module.css";

export function FilesPage(_props: { path?: string }) {
  const [cwd, setCwd] = useState("/");
  const [entries, setEntries] = useState<FileEntry[]>([]);
  const [content, setContent] = useState<string | null>(null);
  const [editPath, setEditPath] = useState("");
  const [modified, setModified] = useState(false);

  function load(dir: string) {
    setCwd(dir);
    setContent(null);
    setEditPath("");
    setModified(false);
    listDir(dir).then(setEntries);
  }

  useEffect(() => {
    load("/");
  }, []);

  function openDir(name: string) {
    const next = cwd === "/" ? `/${name}` : `${cwd}/${name}`;
    load(next);
  }

  function goUp() {
    const parts = cwd.split("/").filter(Boolean);
    parts.pop();
    load(parts.length ? `/${parts.join("/")}` : "/");
  }

  async function openFile(name: string) {
    const path = cwd === "/" ? `/${name}` : `${cwd}/${name}`;
    try {
      const text = await readFile(path);
      setContent(text);
      setEditPath(path);
      setModified(false);
    } catch {
      toast("Cannot read file");
    }
  }

  async function saveFile() {
    if (!editPath || content === null) return;
    try {
      await uploadFile(editPath, content);
      toast("Saved!");
      setModified(false);
    } catch {
      toast("Save failed");
    }
  }

  async function handleDelete(name: string, type: string) {
    const path = cwd === "/" ? `/${name}` : `${cwd}/${name}`;
    if (!confirm(`Delete ${type} "${path}"?`)) return;
    await deleteFile(path);
    toast("Deleted");
    load(cwd);
  }

  async function handleNewDir() {
    const name = prompt("Directory name:");
    if (!name) return;
    const path = cwd === "/" ? `/${name}` : `${cwd}/${name}`;
    await createDir(path);
    load(cwd);
  }

  async function handleUpload(file: File) {
    const path = cwd === "/" ? `/${file.name}` : `${cwd}/${file.name}`;
    await uploadFile(path, file);
    toast("Uploaded!");
    load(cwd);
  }

  return (
    <div class={styles.page}>
      <div class={styles.header}>
        <h2>Files</h2>
        <div class={styles.headerBtns}>
          <button onClick={handleNewDir}>New Dir</button>
          <label class={`btn-primary ${styles.uploadLabel}`}>
            Upload
            <input
              type="file"
              style={{ display: "none" }}
              onChange={(e) => {
                const f = (e.target as HTMLInputElement).files?.[0];
                if (f) handleUpload(f);
              }}
            />
          </label>
        </div>
      </div>

      {/* Breadcrumb */}
      <div class={styles.breadcrumb}>
        <span class={styles.breadcrumbLink} onClick={() => load("/")}>
          /
        </span>
        {cwd
          .split("/")
          .filter(Boolean)
          .map((part, i, arr) => {
            const path = "/" + arr.slice(0, i + 1).join("/");
            return (
              <span key={path}>
                <span class={styles.breadcrumbLink} onClick={() => load(path)}>
                  {part}
                </span>
                {i < arr.length - 1 && " / "}
              </span>
            );
          })}
      </div>

      {/* File list */}
      <div class={`card ${styles.fileList}`}>
        {cwd !== "/" && (
          <div class={styles.fileListRowNav} onClick={goUp}>
            ..
          </div>
        )}
        {entries.map((e) => (
          <div key={e.name} class={styles.fileListRow}>
            <span
              class={`${styles.fileName} ${e.type === "dir" ? styles.fileNameDir : styles.fileNameFile}`}
              onClick={() => (e.type === "dir" ? openDir(e.name) : openFile(e.name))}
            >
              {e.type === "dir" ? "📁 " : "📄 "}
              {e.name}
            </span>
            <button
              class={`btn-danger ${styles.btnDel}`}
              onClick={() => handleDelete(e.name, e.type)}
            >
              Del
            </button>
          </div>
        ))}
        {entries.length === 0 && (
          <div class={styles.emptyDir}>
            Empty directory
          </div>
        )}
      </div>

      {/* Editor */}
      {content !== null && (
        <div class="card">
          <div class={styles.editorHeader}>
            <span class={styles.editorPath}>
              {editPath} {modified && "*"}
            </span>
            <div class={styles.editorBtns}>
              <button class="btn-primary" onClick={saveFile} disabled={!modified}>
                Save
              </button>
              <button
                onClick={() => {
                  setContent(null);
                  setEditPath("");
                }}
              >
                Close
              </button>
            </div>
          </div>
          <textarea
            value={content}
            onInput={(e) => {
              setContent((e.target as HTMLTextAreaElement).value);
              setModified(true);
            }}
            class={styles.textarea}
          />
        </div>
      )}
    </div>
  );
}
