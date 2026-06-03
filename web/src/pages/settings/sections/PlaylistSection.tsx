import { useEffect, useState } from "preact/hooks";
import { getPlaylist, savePlaylist, getApps, getEffects } from "../../../api/client";
import type { PlaylistItem, PlaylistConfig, AppInfo, EffectInfo } from "../../../api/types";
import { Card, Toggle, Button } from "../../../components/ui";
import { toast } from "../../../components/Toast";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

const NATIVE_IDS = [
  "Time", "Date", "Temperature", "Humidity", "Battery",
  "OutdoorTemp", "OutdoorHum", "Pressure", "AirQuality", "UV",
];

export function PlaylistSection() {
  const t = useT();
  const [config, setConfig] = useState<PlaylistConfig | null>(null);
  const [apps, setApps] = useState<AppInfo[]>([]);
  const [effects, setEffects] = useState<EffectInfo[]>([]);
  const [dragIndex, setDragIndex] = useState<number | null>(null);
  const [overIndex, setOverIndex] = useState<number | null>(null);
  const [showModal, setShowModal] = useState(false);
  const [modalTab, setModalTab] = useState<"app" | "effect">("app");
  const [modalDuration, setModalDuration] = useState("");

  useEffect(() => {
    getPlaylist().then(setConfig).catch(() => {});
    getApps().then(setApps).catch(() => {});
    getEffects().then(setEffects).catch(() => {});
  }, []);

  function label(name: string): string {
    const a = t.apps;
    switch (name) {
      case "Time": return a.time;
      case "Date": return a.date;
      case "Temperature": return a.temperature;
      case "Humidity": return a.humidity;
      case "Battery": return a.battery;
      case "OutdoorTemp": return a.outdoorTemp;
      case "OutdoorHum": return a.outdoorHum;
      case "Pressure": return a.pressure;
      case "AirQuality": return a.airQuality;
      case "UV": return a.uvIndex;
      default: return name;
    }
  }

  async function toggleMode(enabled: boolean) {
    if (!config) return;
    const updated = { ...config, enabled };
    setConfig(updated);
    try {
      await savePlaylist({ enabled });
      toast(t.apps.playlistSaved);
    } catch {
      toast(t.errorSaving);
    }
  }

  async function commitItems(items: PlaylistItem[]) {
    if (!config) return;
    const updated = { ...config, items };
    setConfig(updated);
    try {
      await savePlaylist({ items });
      toast(t.apps.playlistSaved);
    } catch {
      toast(t.errorSaving);
    }
  }

  function handleDrop(target: number) {
    const from = dragIndex;
    setDragIndex(null);
    setOverIndex(null);
    if (!config || from === null || from === target) return;
    const next = config.items.slice();
    const [moved] = next.splice(from, 1);
    next.splice(target, 0, moved);
    commitItems(next);
  }

  function deleteItem(index: number) {
    if (!config) return;
    const next = config.items.filter((_, i) => i !== index);
    commitItems(next);
  }

  function addItem(type: "app" | "effect", name: string) {
    if (!config) return;
    const duration = modalDuration ? parseInt(modalDuration, 10) : 0;
    const item: PlaylistItem = { type, name, duration };
    const next = [...config.items, item];
    commitItems(next);
    setShowModal(false);
    setModalDuration("");
  }

  function getTotalDuration(): number {
    if (!config) return 0;
    return config.items.reduce((sum, item) => sum + (item.duration || 7), 0);
  }

  if (!config) return null;

  return (
    <>
      <Card title={t.apps.playlist}>
        <div class={styles.stack}>
          <Toggle
            label={t.apps.playlistMode}
            checked={config.enabled}
            onChange={toggleMode}
          />
          <p class={styles.hint}>
            {config.enabled ? t.apps.playlistModePlaylist : t.apps.playlistModeSimple}
          </p>

          {config.enabled && (
            <>
              {config.items.length === 0 ? (
                <p class={styles.hint}>{t.apps.playlistEmpty}</p>
              ) : (
                <>
                  <div class={styles.orderList}>
                    {config.items.map((item, i) => (
                      <div
                        key={`${item.type}-${item.name}-${i}`}
                        class={`${styles.orderItem} ${styles.playlistItem} ${dragIndex === i ? styles.orderItemDragging : ""} ${overIndex === i ? styles.orderItemOver : ""}`}
                        draggable
                        onDragStart={() => setDragIndex(i)}
                        onDragOver={(e) => { e.preventDefault(); setOverIndex(i); }}
                        onDragEnd={() => { setDragIndex(null); setOverIndex(null); }}
                        onDrop={(e) => { e.preventDefault(); handleDrop(i); }}
                      >
                        <span class={styles.dragHandle}>☰</span>
                        <span class={styles.orderIndex}>{i + 1}</span>
                        <span class={styles.orderName}>
                          {item.type === "effect" ? `✨ ${item.name}` : label(item.name)}
                        </span>
                        {item.type === "effect" && (
                          <span class={styles.playlistEffectBadge}>{t.apps.playlistEffectBadge}</span>
                        )}
                        {!NATIVE_IDS.includes(item.name) && item.type === "app" && (
                          <span class={styles.orderCustomBadge}>custom</span>
                        )}
                        <span class={styles.playlistDuration}>
                          {item.duration || 7}{t.apps.playlistSeconds}
                        </span>
                        <button
                          class={styles.playlistDeleteBtn}
                          onClick={() => deleteItem(i)}
                          title={t.apps.playlistDelete}
                        >
                          ✕
                        </button>
                      </div>
                    ))}
                  </div>
                  <p class={styles.hint}>
                    {t.apps.playlistTotalCycle}: ~{getTotalDuration()}{t.apps.playlistSeconds}
                  </p>
                </>
              )}

              <Button onClick={() => setShowModal(true)}>
                ➕ {t.apps.playlistAdd}
              </Button>
              <p class={styles.hintMt}>{t.apps.playlistHint}</p>
            </>
          )}
        </div>
      </Card>

      {showModal && (
        <div class={styles.modalOverlay} onClick={() => setShowModal(false)}>
          <div class={styles.modal} onClick={(e) => e.stopPropagation()}>
            <div class={styles.modalHeader}>
              <h3>{t.apps.playlistAddTitle}</h3>
              <button class={styles.modalClose} onClick={() => setShowModal(false)}>✕</button>
            </div>
            <div class={styles.modalBody}>
              <div class={styles.tabRow}>
                <button
                  class={`${styles.tab} ${modalTab === "app" ? styles.tabActive : ""}`}
                  onClick={() => setModalTab("app")}
                >
                  {t.apps.playlistTypeApp}
                </button>
                <button
                  class={`${styles.tab} ${modalTab === "effect" ? styles.tabActive : ""}`}
                  onClick={() => setModalTab("effect")}
                >
                  {t.apps.playlistTypeEffect}
                </button>
              </div>

              <div class={styles.itemList}>
                {modalTab === "app" ? (
                  apps.map((app) => (
                    <button
                      key={app.name}
                      class={styles.itemOption}
                      onClick={() => addItem("app", app.name)}
                    >
                      {label(app.name)}
                      {!NATIVE_IDS.includes(app.name) && (
                        <span class={styles.orderCustomBadge}>custom</span>
                      )}
                    </button>
                  ))
                ) : (
                  effects.map((effect, index) => (
                    <button
                      key={effect.name}
                      class={styles.itemOption}
                      onClick={() => addItem("effect", effect.name)}
                    >
                      {index + 1}. ✨ {effect.name}
                    </button>
                  ))
                )}
              </div>

              <div class={styles.durationRow}>
                <label>{t.apps.playlistDuration}:</label>
                <input
                  type="number"
                  class={styles.durationInput}
                  value={modalDuration}
                  onChange={(e) => setModalDuration((e.target as HTMLInputElement).value)}
                  placeholder="7"
                  min="1"
                  max="300"
                />
                <span class={styles.hint}>{t.apps.playlistDurationHint}</span>
              </div>
            </div>
          </div>
        </div>
      )}
    </>
  );
}
