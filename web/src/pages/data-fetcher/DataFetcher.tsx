import { useState, useEffect, useId } from "preact/hooks";
import { useTranslation } from "react-i18next";
import {
  getDataSources,
  addDataSource,
  deleteDataSource,
  fetchDataSource,
} from "../../api/client";
import type { DataSource } from "../../api/types";
import { toast } from "../../components/Toast";
import { TextField, ConfirmDialog } from "../../components/ui";
import styles from "./DataFetcher.module.css";

const empty: DataSource = {
  name: "",
  url: "",
  jsonPath: "",
  displayFormat: "",
  icon: "",
  color: "#f0b800",
  interval: 900,
};

const isValidUrl = (url: string) => /^https?:\/\//i.test(url.trim());

export function DataFetcherPage(_props: { path?: string }) {
  const { t } = useTranslation();
  const colorId = useId();
  const [sources, setSources] = useState<DataSource[]>([]);
  const [form, setForm] = useState<DataSource>({ ...empty });
  const [showForm, setShowForm] = useState(false);
  const [editing, setEditing] = useState(false);
  const [delTarget, setDelTarget] = useState<string | null>(null);

  const load = () => getDataSources().then(setSources);
  useEffect(() => {
    load();
  }, []);

  function upd(patch: Partial<DataSource>) {
    setForm((f) => ({ ...f, ...patch }));
  }

  // Inline URL error surfaces as soon as a non-empty, malformed URL is present.
  const urlError =
    form.url.trim() !== "" && !isValidUrl(form.url)
      ? t("dataFetcher.err.url")
      : undefined;

  async function save() {
    if (!form.name || !form.url || !form.jsonPath) {
      toast(t("dataFetcher.requiredFields"), { error: true });
      return;
    }
    if (!isValidUrl(form.url)) {
      toast(t("dataFetcher.err.url"), { error: true });
      return;
    }
    try {
      await addDataSource(form);
      toast(t("dataFetcher.saved"));
      setShowForm(false);
      setForm({ ...empty });
      setEditing(false);
      load();
    } catch {
      toast(t("dataFetcher.errorSaving"), { error: true });
    }
  }

  async function remove(name: string) {
    setDelTarget(null);
    await deleteDataSource(name);
    toast(t("dataFetcher.deleted"));
    load();
  }

  function edit(src: DataSource) {
    setForm({ ...src });
    setEditing(true);
    setShowForm(true);
  }

  return (
    <div class={styles.page}>
      <div class={styles.header}>
        <h2>{t("dataFetcher.title")}</h2>
        <button
          class="btn-primary"
          onClick={() => {
            setForm({ ...empty });
            setEditing(false);
            setShowForm(!showForm);
          }}
        >
          {showForm ? t("common.cancel") : t("dataFetcher.addSource")}
        </button>
      </div>

      {showForm && (
        <div class="card">
          <h3 class={styles.formHeading}>{editing ? t("dataFetcher.editSource") : t("dataFetcher.newSource")}</h3>
          <div class={styles.formStack}>
            <div class="form-row">
              <TextField
                label={t("dataFetcher.name")}
                value={form.name}
                onChange={(v) => upd({ name: v })}
                placeholder="btc"
              />
              <TextField
                label={t("dataFetcher.intervalSec")}
                type="number"
                value={form.interval}
                onChange={(v) => upd({ interval: +v })}
              />
            </div>
            <TextField
              label={t("dataFetcher.url")}
              value={form.url}
              onChange={(v) => upd({ url: v })}
              placeholder="https://api.example.com/data"
              autocomplete="url"
              error={urlError}
            />
            <div class="form-row">
              <div class="form-group">
                <TextField
                  label={t("dataFetcher.jsonPath")}
                  value={form.jsonPath}
                  onChange={(v) => upd({ jsonPath: v })}
                  placeholder="data.price"
                />
                <p class={styles.hint}>{t("dataFetcher.hint.jsonPath")}</p>
              </div>
              <div class="form-group">
                <TextField
                  label={t("dataFetcher.displayFormat")}
                  value={form.displayFormat}
                  onChange={(v) => upd({ displayFormat: v })}
                  placeholder="$%.0f"
                />
                <p class={styles.hint}>{t("dataFetcher.hint.displayFormat")}</p>
              </div>
            </div>
            <div class="form-row">
              <TextField
                label={t("dataFetcher.iconName")}
                value={form.icon}
                onChange={(v) => upd({ icon: v })}
                placeholder="bitcoin"
              />
              <div class="form-group">
                <label htmlFor={colorId}>{t("dataFetcher.color")}</label>
                <input
                  id={colorId}
                  type="color"
                  value={form.color}
                  onInput={(e) => upd({ color: (e.target as HTMLInputElement).value })}
                />
              </div>
            </div>
            <button class="btn-primary" onClick={save}>
              {editing ? t("dataFetcher.update") : t("common.add")}
            </button>
          </div>
        </div>
      )}

      {sources.length === 0 && !showForm && (
        <p class={styles.empty}>{t("dataFetcher.empty")}</p>
      )}

      {sources.map((src) => (
        <div class="card" key={src.name}>
          <div class={styles.sourceHeader}>
            <strong class={styles.sourceName}>{src.name}</strong>
            <div class={styles.sourceBtns}>
              <button
                class={styles.btnSmall}
                onClick={() => fetchDataSource(src.name).then(() => toast(t("dataFetcher.fetched")))}
              >
                {t("dataFetcher.fetch")}
              </button>
              <button
                class={styles.btnSmall}
                onClick={() => edit(src)}
              >
                {t("common.edit")}
              </button>
              <button
                class={`btn-danger ${styles.btnSmall}`}
                onClick={() => setDelTarget(src.name)}
              >
                {t("common.delete")}
              </button>
            </div>
          </div>
          <div class={styles.sourceMeta}>
            <div>{t("dataFetcher.metaUrl", { url: src.url })}</div>
            <div>
              {t("dataFetcher.metaLine", {
                path: src.jsonPath,
                format: src.displayFormat || "—",
                interval: src.interval,
              })}
            </div>
            {src.icon && <div>{t("dataFetcher.metaIcon", { icon: src.icon })}</div>}
          </div>
        </div>
      ))}

      <ConfirmDialog
        open={delTarget !== null}
        title={t("dataFetcher.deleteTitle")}
        body={t("dataFetcher.confirmDelete", { name: delTarget ?? "" })}
        confirmLabel={t("common.delete")}
        danger
        onConfirm={() => delTarget && remove(delTarget)}
        onCancel={() => setDelTarget(null)}
      />
    </div>
  );
}
