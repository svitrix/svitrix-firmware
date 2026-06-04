import { useState, useEffect } from "preact/hooks";
import {
  getDataSources,
  addDataSource,
  deleteDataSource,
  fetchDataSource,
} from "../../api/client";
import type { DataSource } from "../../api/types";
import { toast } from "../../components/Toast";
import { useT } from "../../i18n";
import styles from "./DataFetcher.module.css";

const empty: DataSource = {
  name: "",
  url: "",
  jsonPath: "",
  displayFormat: "",
  icon: "",
  color: "#f0b800",
  interval: 900,
  duration: 0,
};

export function DataFetcherPage(_props: { path?: string }) {
  const [sources, setSources] = useState<DataSource[]>([]);
  const [form, setForm] = useState<DataSource>({ ...empty });
  const [showForm, setShowForm] = useState(false);
  const [editing, setEditing] = useState(false);
  const t = useT();

  const load = () => getDataSources().then(setSources);
  useEffect(() => {
    load();
  }, []);

  function upd(patch: Partial<DataSource>) {
    setForm((f) => ({ ...f, ...patch }));
  }

  async function save() {
    if (!form.name || !form.url || !form.jsonPath) {
      toast(t.dataFetcher.required);
      return;
    }
    try {
      await addDataSource(form);
      toast(t.dataFetcher.saved);
      setShowForm(false);
      setForm({ ...empty });
      setEditing(false);
      load();
    } catch {
      toast(t.dataFetcher.errorSaving);
    }
  }

  async function remove(name: string) {
    if (!confirm(`${t.dataFetcher.confirmDelete} "${name}"?`)) return;
    await deleteDataSource(name);
    toast(t.dataFetcher.deleted);
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
        <h2>{t.dataFetcher.title}</h2>
        <button
          class="btn-primary"
          onClick={() => {
            setForm({ ...empty });
            setEditing(false);
            setShowForm(!showForm);
          }}
        >
          {showForm ? t.dataFetcher.cancel : t.dataFetcher.addSource}
        </button>
      </div>

      {showForm && (
        <div class="card">
          <h3 class={styles.formHeading}>{editing ? t.dataFetcher.editSource : t.dataFetcher.newSource}</h3>
          <div class={styles.formStack}>
            <div class="form-row">
              <div class="form-group">
                <label>{t.dataFetcher.name}</label>
                <input
                  type="text"
                  value={form.name}
                  disabled={editing}
                  onInput={(e) => upd({ name: (e.target as HTMLInputElement).value })}
                  placeholder="btc"
                />
              </div>
              <div class="form-group">
                <label>{t.dataFetcher.interval}</label>
                <input
                  type="number"
                  min={60}
                  value={form.interval}
                  onInput={(e) => upd({ interval: +(e.target as HTMLInputElement).value })}
                />
              </div>
            </div>
            <div class="form-group">
              <label>{t.dataFetcher.url}</label>
              <input
                type="text"
                value={form.url}
                onInput={(e) => upd({ url: (e.target as HTMLInputElement).value })}
                placeholder="https://api.example.com/data"
              />
            </div>
            <div class="form-row">
              <div class="form-group">
                <label>{t.dataFetcher.jsonPath}</label>
                <input
                  type="text"
                  value={form.jsonPath}
                  onInput={(e) => upd({ jsonPath: (e.target as HTMLInputElement).value })}
                  placeholder="data.price"
                />
              </div>
              <div class="form-group">
                <label>{t.dataFetcher.displayFormat}</label>
                <input
                  type="text"
                  value={form.displayFormat}
                  onInput={(e) => upd({ displayFormat: (e.target as HTMLInputElement).value })}
                  placeholder="$%.0f"
                />
              </div>
            </div>
            <div class="form-row-3">
              <div class="form-group">
                <label>{t.dataFetcher.duration}</label>
                <input
                  type="number"
                  min={0}
                  value={form.duration}
                  onInput={(e) => upd({ duration: +(e.target as HTMLInputElement).value })}
                  placeholder="0"
                />
              </div>
              <div class="form-group">
                <label>{t.dataFetcher.iconName}</label>
                <input
                  type="text"
                  value={form.icon}
                  onInput={(e) => upd({ icon: (e.target as HTMLInputElement).value })}
                  placeholder="16437"
                />
              </div>
              <div class="form-group">
                <label>{t.dataFetcher.color}</label>
                <input
                  type="color"
                  value={form.color}
                  onInput={(e) => upd({ color: (e.target as HTMLInputElement).value })}
                />
              </div>
            </div>
            <button class="btn-primary" onClick={save}>
              {editing ? t.dataFetcher.update : t.dataFetcher.add}
            </button>
          </div>
        </div>
      )}

      {sources.length === 0 && !showForm && (
        <p class={styles.empty}>{t.dataFetcher.noSources}</p>
      )}

      {sources.map((src) => (
        <div class="card" key={src.name}>
          <div class={styles.sourceHeader}>
            <strong class={styles.sourceName}>{src.name}</strong>
            <div class={styles.sourceBtns}>
              <button
                class={styles.btnSmall}
                onClick={() => fetchDataSource(src.name).then(() => toast(t.dataFetcher.fetched))}
              >
                {t.dataFetcher.fetch}
              </button>
              <button
                class={styles.btnSmall}
                onClick={() => edit(src)}
              >
                {t.dataFetcher.edit}
              </button>
              <button
                class={`btn-danger ${styles.btnSmall}`}
                onClick={() => remove(src.name)}
              >
                {t.dataFetcher.delete}
              </button>
            </div>
          </div>
          <div class={styles.sourceMeta}>
            <div>URL: {src.url}</div>
            <div>
              Path: {src.jsonPath} | Format: {src.displayFormat || "—"} | Every {src.interval}s
              {src.duration > 0 && ` | Display: ${src.duration}s`}
            </div>
            {src.icon && <div>Icon: {src.icon}</div>}
          </div>
        </div>
      ))}
    </div>
  );
}
