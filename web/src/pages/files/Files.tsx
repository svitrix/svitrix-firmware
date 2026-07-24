import { useState, useEffect } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { listDir, readFile, uploadFile, deleteFile, createDir } from "../../api/client";
import type { FileEntry } from "../../api/types";
import { toast } from "../../components/Toast";
import { ConfirmDialog, PromptDialog } from "../../components/ui";
import { FileIcon } from "./FileIcon";
import { Breadcrumb } from "./Breadcrumb";
import { RowActions } from "./RowActions";
import { DropZone } from "./DropZone";
import { fileType, isEditable, protection, downloadPath } from "./fileType";
import styles from "./Files.module.css";

export function FilesPage(_props: { path?: string }) {
  const { t } = useTranslation();
  const [cwd, setCwd] = useState("/");
  const [entries, setEntries] = useState<FileEntry[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(false);
  const [content, setContent] = useState<string | null>(null);
  const [editPath, setEditPath] = useState("");
  const [modified, setModified] = useState(false);
  const [promptOpen, setPromptOpen] = useState(false);
  const [delTarget, setDelTarget] = useState<FileEntry | null>(null);
  const [confirmClose, setConfirmClose] = useState(false);

  function load(dir: string) {
    setCwd(dir);
    setContent(null);
    setEditPath("");
    setModified(false);
    setLoading(true);
    setError(false);
    listDir(dir)
      .then((e) => { setEntries(e); setLoading(false); })
      .catch(() => { setError(true); setLoading(false); });
  }

  useEffect(() => { load("/"); }, []);

  const full = (name: string) => (cwd === "/" ? `/${name}` : `${cwd}/${name}`);

  // Folders first, then alpha within each group.
  const sorted = [...entries].sort((a, b) =>
    a.type === b.type ? a.name.localeCompare(b.name) : a.type === "dir" ? -1 : 1
  );

  function goUp() {
    const parts = cwd.split("/").filter(Boolean);
    parts.pop();
    load(parts.length ? `/${parts.join("/")}` : "/");
  }

  async function download(name: string) {
    try {
      await downloadPath(full(name), name);
    } catch {
      toast(t("files.downloadFailed"));
    }
  }

  async function openFile(name: string) {
    const kind = fileType(name, "file");
    if (!isEditable(kind)) return download(name); // binary → download, never textarea
    try {
      const text = await readFile(full(name));
      setContent(text);
      setEditPath(full(name));
      setModified(false);
    } catch {
      toast(t("files.cannotRead"));
    }
  }

  async function saveFile() {
    if (!editPath || content === null) return;
    try {
      await uploadFile(editPath, content);
      toast(t("files.saved"));
      setModified(false);
    } catch {
      toast(t("files.saveFailed"));
    }
  }

  function closeEditor() {
    setContent(null);
    setEditPath("");
    setModified(false);
    setConfirmClose(false);
  }
  function requestClose() {
    if (modified) setConfirmClose(true);
    else closeEditor();
  }

  async function doDelete() {
    if (!delTarget) return;
    try {
      await deleteFile(full(delTarget.name));
      toast(t("files.deleted"));
    } catch {
      toast(t("files.saveFailed"));
    }
    setDelTarget(null);
    load(cwd);
  }

  async function createFolder(name: string) {
    setPromptOpen(false);
    await createDir(full(name));
    load(cwd);
  }

  async function handleUpload(files: FileList) {
    for (const f of Array.from(files)) {
      try {
        await uploadFile(full(f.name), f);
      } catch {
        /* keep going, report at the end */
      }
    }
    toast(t("files.uploaded"));
    load(cwd);
  }

  return (
    <div class={styles.page}>
      <div class={styles.header}>
        <h2>{t("files.title")}</h2>
        <div class={styles.headerBtns}>
          <button onClick={() => setPromptOpen(true)}>{t("files.newFolder")}</button>
          <label class={`btn-primary ${styles.uploadLabel}`}>
            {t("common.upload")}
            <input
              type="file"
              multiple
              style={{ display: "none" }}
              onChange={(e) => {
                const files = (e.target as HTMLInputElement).files;
                if (files && files.length) handleUpload(files);
              }}
            />
          </label>
        </div>
      </div>

      <Breadcrumb path={cwd} onNavigate={load} />

      <DropZone onFiles={handleUpload} label={t("files.dropHere")}>
        <div class={`card ${styles.fileList}`}>
          {loading && <div class={styles.state}>{t("common.loading")}</div>}
          {error && (
            <div class={styles.state}>
              {t("files.loadError")}{" "}
              <button onClick={() => load(cwd)}>{t("files.retry")}</button>
            </div>
          )}
          {!loading && !error && (
            <>
              {cwd !== "/" && (
                <button class={styles.rowNav} onClick={goUp}>‹ ..</button>
              )}
              {sorted.map((e) => {
                const kind = fileType(e.name, e.type);
                const prot = protection(e.name);
                const locked = prot === "lock";
                return (
                  <div key={e.name} class={styles.row}>
                    <button
                      class={styles.nameBtn}
                      onClick={() => (e.type === "dir" ? load(full(e.name)) : openFile(e.name))}
                    >
                      <FileIcon kind={kind} />
                      <span class={styles.name}>{e.name}</span>
                      {prot && (
                        <svg class={styles.lock} width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" aria-label={t("files.protected")}>
                          <rect x="5" y="11" width="14" height="9" rx="2" />
                          <path d="M8 11V8a4 4 0 0 1 8 0v3" />
                        </svg>
                      )}
                      {e.type === "dir" && <span class={styles.chev}>›</span>}
                    </button>
                    <RowActions
                      canDownload={e.type === "file"}
                      canEdit={e.type === "file" && isEditable(kind) && !locked}
                      canDelete={!locked}
                      onDownload={() => download(e.name)}
                      onEdit={() => openFile(e.name)}
                      onDelete={() => setDelTarget(e)}
                    />
                  </div>
                );
              })}
              {sorted.length === 0 && (
                <div class={styles.state}>
                  {t("files.emptyDir")} · {t("files.uploadHint")}
                </div>
              )}
            </>
          )}
        </div>
      </DropZone>

      {content !== null && (
        <div class="card">
          <div class={styles.editorHeader}>
            <span class={styles.editorPath}>
              {editPath} {modified && "*"}
            </span>
            <div class={styles.editorBtns}>
              <button class="btn-primary" onClick={saveFile} disabled={!modified}>
                {t("common.save")}
              </button>
              <button onClick={requestClose}>{t("common.close")}</button>
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

      <ConfirmDialog
        open={!!delTarget}
        title={t("files.deleteTitle", {
          type: delTarget?.type === "dir" ? t("files.typeDir") : t("files.typeFile"),
        })}
        body={
          t("files.deleteBody", { name: delTarget?.name ?? "" }) +
          (protection(delTarget?.name ?? "") === "warn" ? " " + t("files.protectedWarn") : "")
        }
        confirmLabel={t("common.delete")}
        danger
        onConfirm={doDelete}
        onCancel={() => setDelTarget(null)}
      />

      <ConfirmDialog
        open={confirmClose}
        title={t("files.unsavedTitle")}
        body={t("files.unsavedBody")}
        confirmLabel={t("files.discard")}
        danger
        onConfirm={closeEditor}
        onCancel={() => setConfirmClose(false)}
      />

      <PromptDialog
        open={promptOpen}
        title={t("files.newFolder")}
        label={t("files.folderName")}
        submitLabel={t("files.create")}
        onSubmit={createFolder}
        onCancel={() => setPromptOpen(false)}
      />
    </div>
  );
}
