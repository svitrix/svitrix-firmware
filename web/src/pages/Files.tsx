import { useState, useEffect } from "preact/hooks";
import {
  listDir,
  readFile,
  uploadFile,
  deleteFile,
  createDir,
} from "../api/client";
import type { FileEntry } from "../api/types";
import { toast } from "../components/Toast";

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
    <div style={{ display: "flex", flexDirection: "column", gap: 12 }}>
      <div style={{ display: "flex", justifyContent: "space-between", alignItems: "center" }}>
        <h2>Files</h2>
        <div style={{ display: "flex", gap: 6 }}>
          <button onClick={handleNewDir}>New Dir</button>
          <label class="btn-primary" style={{ display: "inline-flex", alignItems: "center", cursor: "pointer", padding: "8px 16px", borderRadius: "var(--radius)" }}>
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
      <div style={{ fontSize: 13, color: "var(--text-dim)" }}>
        <span
          style={{ cursor: "pointer", color: "var(--accent)" }}
          onClick={() => load("/")}
        >
          /
        </span>
        {cwd
          .split("/")
          .filter(Boolean)
          .map((part, i, arr) => {
            const path = "/" + arr.slice(0, i + 1).join("/");
            return (
              <span key={path}>
                <span
                  style={{ cursor: "pointer", color: "var(--accent)" }}
                  onClick={() => load(path)}
                >
                  {part}
                </span>
                {i < arr.length - 1 && " / "}
              </span>
            );
          })}
      </div>

      {/* File list */}
      <div class="card" style={{ padding: 0 }}>
        {cwd !== "/" && (
          <div
            style={{
              padding: "8px 16px",
              borderBottom: "1px solid var(--border)",
              cursor: "pointer",
              color: "var(--text-dim)",
            }}
            onClick={goUp}
          >
            ..
          </div>
        )}
        {entries.map((e) => (
          <div
            key={e.name}
            style={{
              padding: "8px 16px",
              borderBottom: "1px solid var(--border)",
              display: "flex",
              justifyContent: "space-between",
              alignItems: "center",
            }}
          >
            <span
              style={{ cursor: "pointer", color: e.type === "dir" ? "var(--accent)" : "var(--text)" }}
              onClick={() => (e.type === "dir" ? openDir(e.name) : openFile(e.name))}
            >
              {e.type === "dir" ? "📁 " : "📄 "}
              {e.name}
            </span>
            <button
              class="btn-danger"
              style={{ padding: "2px 8px", fontSize: 12 }}
              onClick={() => handleDelete(e.name, e.type)}
            >
              Del
            </button>
          </div>
        ))}
        {entries.length === 0 && (
          <div style={{ padding: "16px", color: "var(--text-dim)", textAlign: "center" }}>
            Empty directory
          </div>
        )}
      </div>

      {/* Editor */}
      {content !== null && (
        <div class="card">
          <div
            style={{
              display: "flex",
              justifyContent: "space-between",
              alignItems: "center",
              marginBottom: 8,
            }}
          >
            <span style={{ fontSize: 13, color: "var(--text-dim)" }}>
              {editPath} {modified && "*"}
            </span>
            <div style={{ display: "flex", gap: 6 }}>
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
            style={{
              width: "100%",
              minHeight: 300,
              fontFamily: "monospace",
              fontSize: 13,
              resize: "vertical",
            }}
          />
        </div>
      )}
    </div>
  );
}
