import { useState, useEffect } from "preact/hooks";
import {
  getDataSources,
  addDataSource,
  deleteDataSource,
  fetchDataSource,
} from "../../api/client";
import type { DataSource } from "../../api/types";
import { toast } from "../../components/Toast";
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

export function DataFetcherPage(_props: { path?: string }) {
  const [sources, setSources] = useState<DataSource[]>([]);
  const [form, setForm] = useState<DataSource>({ ...empty });
  const [showForm, setShowForm] = useState(false);
  const [editing, setEditing] = useState(false);

  const load = () => getDataSources().then(setSources);
  useEffect(() => {
    load();
  }, []);

  function upd(patch: Partial<DataSource>) {
    setForm((f) => ({ ...f, ...patch }));
  }

  async function save() {
    if (!form.name || !form.url || !form.jsonPath) {
      toast("Name, URL, and JSON Path are required");
      return;
    }
    try {
      await addDataSource(form);
      toast("Saved!");
      setShowForm(false);
      setForm({ ...empty });
      setEditing(false);
      load();
    } catch {
      toast("Error saving");
    }
  }

  async function remove(name: string) {
    if (!confirm(`Delete "${name}"?`)) return;
    await deleteDataSource(name);
    toast("Deleted");
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
        <h2>Data Sources</h2>
        <button
          class="btn-primary"
          onClick={() => {
            setForm({ ...empty });
            setEditing(false);
            setShowForm(!showForm);
          }}
        >
          {showForm ? "Cancel" : "+ Add Source"}
        </button>
      </div>

      {showForm && (
        <div class="card">
          <h3 class={styles.formHeading}>{editing ? "Edit Source" : "New Source"}</h3>
          <div class={styles.formStack}>
            <div class="form-row">
              <div class="form-group">
                <label>Name</label>
                <input
                  type="text"
                  value={form.name}
                  disabled={editing}
                  onInput={(e) => upd({ name: (e.target as HTMLInputElement).value })}
                  placeholder="btc"
                />
              </div>
              <div class="form-group">
                <label>Interval (sec)</label>
                <input
                  type="number"
                  min={60}
                  value={form.interval}
                  onInput={(e) => upd({ interval: +(e.target as HTMLInputElement).value })}
                />
              </div>
            </div>
            <div class="form-group">
              <label>URL</label>
              <input
                type="text"
                value={form.url}
                onInput={(e) => upd({ url: (e.target as HTMLInputElement).value })}
                placeholder="https://api.example.com/data"
              />
            </div>
            <div class="form-row">
              <div class="form-group">
                <label>JSON Path</label>
                <input
                  type="text"
                  value={form.jsonPath}
                  onInput={(e) => upd({ jsonPath: (e.target as HTMLInputElement).value })}
                  placeholder="data.price"
                />
              </div>
              <div class="form-group">
                <label>Display Format</label>
                <input
                  type="text"
                  value={form.displayFormat}
                  onInput={(e) => upd({ displayFormat: (e.target as HTMLInputElement).value })}
                  placeholder="$%.0f"
                />
              </div>
            </div>
            <div class="form-row">
              <div class="form-group">
                <label>Icon Name</label>
                <input
                  type="text"
                  value={form.icon}
                  onInput={(e) => upd({ icon: (e.target as HTMLInputElement).value })}
                  placeholder="bitcoin"
                />
              </div>
              <div class="form-group">
                <label>Color</label>
                <input
                  type="color"
                  value={form.color}
                  onInput={(e) => upd({ color: (e.target as HTMLInputElement).value })}
                />
              </div>
            </div>
            <button class="btn-primary" onClick={save}>
              {editing ? "Update" : "Add"}
            </button>
          </div>
        </div>
      )}

      {sources.length === 0 && !showForm && (
        <p class={styles.empty}>No data sources configured.</p>
      )}

      {sources.map((src) => (
        <div class="card" key={src.name}>
          <div class={styles.sourceHeader}>
            <strong class={styles.sourceName}>{src.name}</strong>
            <div class={styles.sourceBtns}>
              <button
                class={styles.btnSmall}
                onClick={() => fetchDataSource(src.name).then(() => toast("Fetched!"))}
              >
                Fetch
              </button>
              <button
                class={styles.btnSmall}
                onClick={() => edit(src)}
              >
                Edit
              </button>
              <button
                class={`btn-danger ${styles.btnSmall}`}
                onClick={() => remove(src.name)}
              >
                Delete
              </button>
            </div>
          </div>
          <div class={styles.sourceMeta}>
            <div>URL: {src.url}</div>
            <div>
              Path: {src.jsonPath} | Format: {src.displayFormat || "—"} | Every {src.interval}s
            </div>
            {src.icon && <div>Icon: {src.icon}</div>}
          </div>
        </div>
      ))}
    </div>
  );
}
