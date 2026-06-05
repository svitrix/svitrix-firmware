import { useEffect, useState } from "preact/hooks";
import { getRotation, saveRotation, getEffects } from "../../../api/client";
import type { RotationItem, RotationConfig, EffectInfo } from "../../../api/types";
import { Card, Toggle, Button } from "../../../components/ui";
import { toast } from "../../../components/Toast";
import { useT } from "../../../i18n";
import { useSettings } from "../../../context/SettingsContext";
import styles from "../../settings/sections/sections.module.css";

// Native apps - original first, then weather
const NATIVE_APPS_ORIGINAL = [
  "Time", "Date", "Temperature", "Humidity", "Battery",
];
const NATIVE_APPS_WEATHER = [
  "OutdoorTemp", "OutdoorHum", "Pressure", "AirQuality", "UV",
];
const ALL_NATIVE_APPS = [...NATIVE_APPS_ORIGINAL, ...NATIVE_APPS_WEATHER];

function generateId(): string {
  const chars = "0123456789abcdef";
  let id = "";
  for (let i = 0; i < 8; i++) {
    id += chars[Math.floor(Math.random() * 16)];
  }
  return id;
}

export function UnifiedRotationSection() {
  const t = useT();
  const { settings, updateSettings } = useSettings();
  const [config, setConfig] = useState<RotationConfig | null>(null);
  const [effects, setEffects] = useState<EffectInfo[]>([]);
  const [dragIndex, setDragIndex] = useState<number | null>(null);
  const [overIndex, setOverIndex] = useState<number | null>(null);
  const [expandedId, setExpandedId] = useState<string | null>(null);
  const [showModal, setShowModal] = useState(false);
  const [modalTab, setModalTab] = useState<"app" | "effect">("app");

  useEffect(() => {
    getRotation().then((data) => {
      // If empty, initialize with default native apps
      if (!data.items || data.items.length === 0) {
        const defaultItems: RotationItem[] = NATIVE_APPS_ORIGINAL.map(name => ({
          id: generateId(),
          type: "app",
          name,
          enabled: true,
          duration: 0,
          color: 0,
          icon: "",
        }));
        setConfig({ items: defaultItems });
        saveRotation({ items: defaultItems }).catch(() => {});
      } else {
        setConfig(data);
      }
    }).catch(() => {});
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

  async function commitItems(items: RotationItem[]) {
    if (!config) return;
    const updated = { ...config, items };
    setConfig(updated);
    try {
      await saveRotation({ items });
      toast(t.saved || "Saved");
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

  function toggleItem(id: string, enabled: boolean) {
    if (!config) return;
    const next = config.items.map(item =>
      item.id === id ? { ...item, enabled } : item
    );
    commitItems(next);
  }

  function updateItem(id: string, patch: Partial<RotationItem>) {
    if (!config) return;
    const next = config.items.map(item =>
      item.id === id ? { ...item, ...patch } : item
    );
    commitItems(next);
  }

  function deleteItem(id: string) {
    if (!config) return;
    const next = config.items.filter(item => item.id !== id);
    commitItems(next);
  }

  function addItem(type: "app" | "effect", name: string) {
    if (!config) return;
    const item: RotationItem = {
      id: generateId(),
      type,
      name,
      enabled: true,
      duration: 0,
      color: 0,
      icon: "",
    };
    const next = [...config.items, item];
    commitItems(next);
    setShowModal(false);
  }

  function toggleExpand(id: string) {
    setExpandedId(expandedId === id ? null : id);
  }

  function isNative(name: string): boolean {
    return ALL_NATIVE_APPS.includes(name);
  }

  async function toggleCelsius(celsius: boolean) {
    await updateSettings({ CEL: celsius });
  }

  if (!config) return null;

  return (
    <>
      <Card title={t.apps.title}>
        <div class={styles.stack}>
          {config.items.length === 0 ? (
            <p class={styles.hint}>{t.apps.playlistEmpty}</p>
          ) : (
            <div class={styles.orderList}>
              {config.items.map((item, i) => (
                <div key={item.id}>
                  <div
                    class={`${styles.orderItem} ${styles.playlistItem} ${dragIndex === i ? styles.orderItemDragging : ""} ${overIndex === i ? styles.orderItemOver : ""} ${!item.enabled ? styles.orderItemDisabled : ""}`}
                    draggable
                    onDragStart={() => setDragIndex(i)}
                    onDragOver={(e) => { e.preventDefault(); setOverIndex(i); }}
                    onDragEnd={() => { setDragIndex(null); setOverIndex(null); }}
                    onDrop={(e) => { e.preventDefault(); handleDrop(i); }}
                  >
                    <span class={styles.dragHandle}>☰</span>
                    <Toggle
                      checked={item.enabled}
                      onChange={(v) => toggleItem(item.id, v)}
                      compact
                    />
                    <span class={styles.orderName} style={{ opacity: item.enabled ? 1 : 0.5 }}>
                      {item.type === "effect" ? `✨ ${item.name}` : label(item.name)}
                    </span>
                    {item.type === "effect" && (
                      <span class={styles.playlistEffectBadge}>{t.apps.playlistEffectBadge}</span>
                    )}
                    {!isNative(item.name) && item.type === "app" && (
                      <span class={styles.orderCustomBadge}>custom</span>
                    )}
                    {item.duration > 0 && (
                      <span class={styles.playlistDuration}>
                        {item.duration}s
                      </span>
                    )}
                    <button
                      class={styles.expandBtn}
                      onClick={() => toggleExpand(item.id)}
                      title="Settings"
                    >
                      {expandedId === item.id ? "▲" : "▼"}
                    </button>
                    <button
                      class={styles.playlistDeleteBtn}
                      onClick={() => deleteItem(item.id)}
                      title={t.apps.playlistDelete}
                    >
                      ✕
                    </button>
                  </div>

                  {expandedId === item.id && (
                    <div class={styles.rotationExpanded}>
                      <div class={styles.rotationSlider}>
                        <label>{t.apps.playlistDuration}: {item.duration || 7}s</label>
                        <input
                          type="range"
                          min={0}
                          max={300}
                          value={item.duration}
                          onInput={(e) => updateItem(item.id, { duration: parseInt((e.target as HTMLInputElement).value) })}
                        />
                      </div>
                      <div class={styles.rotationRow}>
                        <div class={styles.rotationField}>
                          <label>Color:</label>
                          <input
                            type="color"
                            value={item.color ? "#" + item.color.toString(16).padStart(6, "0") : "#ffffff"}
                            onInput={(e) => updateItem(item.id, { color: parseInt((e.target as HTMLInputElement).value.replace("#", ""), 16) })}
                          />
                          {item.color === 0 && <span class={styles.hint}>(default)</span>}
                        </div>
                        <div class={styles.rotationField}>
                          <label>Icon:</label>
                          <input
                            type="text"
                            class={styles.iconInput}
                            value={item.icon}
                            onChange={(e) => updateItem(item.id, { icon: (e.target as HTMLInputElement).value })}
                            placeholder="default"
                          />
                        </div>
                        {item.name === "Temperature" && settings && (
                          <>
                            <div class={styles.rotationField}>
                              <Toggle
                                label={t.apps.celsius}
                                checked={settings.CEL}
                                onChange={toggleCelsius}
                              />
                            </div>
                            <div class={styles.rotationField}>
                              <label>{t.apps.offset}:</label>
                              <input
                                type="number"
                                class={styles.offsetInput}
                                value={settings.TOFF ?? -9}
                                min={-15}
                                max={5}
                                onChange={(e) => updateSettings({ TOFF: parseInt((e.target as HTMLInputElement).value) || 0 })}
                              />
                              <span class={styles.hint}>°</span>
                            </div>
                          </>
                        )}
                      </div>
                    </div>
                  )}
                </div>
              ))}
            </div>
          )}

          <Button onClick={() => setShowModal(true)}>
            ➕ {t.apps.playlistAdd}
          </Button>
          <p class={styles.hintMt}>{t.apps.playlistHint}</p>
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
                  <>
                    {NATIVE_APPS_ORIGINAL.map((name) => (
                      <button
                        key={name}
                        class={styles.itemOption}
                        onClick={() => addItem("app", name)}
                      >
                        {label(name)}
                      </button>
                    ))}
                    <div class={styles.itemSeparator}>{t.apps.weatherApps}</div>
                    {NATIVE_APPS_WEATHER.map((name) => (
                      <button
                        key={name}
                        class={styles.itemOption}
                        onClick={() => addItem("app", name)}
                      >
                        {label(name)}
                      </button>
                    ))}
                  </>
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
            </div>
          </div>
        </div>
      )}
    </>
  );
}
