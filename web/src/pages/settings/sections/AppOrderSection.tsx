import { useEffect, useState } from "preact/hooks";
import { useSettings } from "../../../context/SettingsContext";
import { getApps, reorderApps } from "../../../api/client";
import type { AppInfo } from "../../../api/types";
import { Card } from "../../../components/ui";
import { toast } from "../../../components/Toast";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

// Native/weather app ids that map to a friendly i18n label. Anything not listed
// here is treated as a custom app and shown by its raw name.
const NATIVE_IDS = [
  "Time", "Date", "Temperature", "Humidity", "Battery",
  "OutdoorTemp", "OutdoorHum", "Pressure", "AirQuality", "UV",
];

export function AppOrderSection() {
  const { appsVersion } = useSettings();
  const t = useT();
  const [apps, setApps] = useState<AppInfo[]>([]);
  const [dragIndex, setDragIndex] = useState<number | null>(null);
  const [overIndex, setOverIndex] = useState<number | null>(null);

  // Fetch the live loop on mount and whenever an app is toggled+saved elsewhere
  // (appsVersion bumps after the device has already run loadNativeApps()).
  useEffect(() => {
    let active = true;
    getApps()
      .then((list) => { if (active) setApps(Array.isArray(list) ? list : []); })
      .catch(() => {});
    return () => { active = false; };
  }, [appsVersion]);

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

  async function commit(next: AppInfo[]) {
    setApps(next);
    try {
      await reorderApps(next.map((a) => a.name));
      toast(t.apps.appOrderSaved);
    } catch {
      toast(t.errorSaving);
    }
  }

  function handleDrop(target: number) {
    const from = dragIndex;
    setDragIndex(null);
    setOverIndex(null);
    if (from === null || from === target) return;
    const next = apps.slice();
    const [moved] = next.splice(from, 1);
    next.splice(target, 0, moved);
    commit(next);
  }

  return (
    <Card title={t.apps.appOrder}>
      {apps.length === 0 ? (
        <p class={styles.hint}>{t.apps.appOrderEmpty}</p>
      ) : (
        <>
          <div class={styles.orderList}>
            {apps.map((app, i) => (
              <div
                key={app.name}
                class={`${styles.orderItem} ${dragIndex === i ? styles.orderItemDragging : ""} ${overIndex === i ? styles.orderItemOver : ""}`}
                draggable
                onDragStart={() => setDragIndex(i)}
                onDragOver={(e) => { e.preventDefault(); setOverIndex(i); }}
                onDragEnd={() => { setDragIndex(null); setOverIndex(null); }}
                onDrop={(e) => { e.preventDefault(); handleDrop(i); }}
              >
                <span class={styles.dragHandle}>☰</span>
                <span class={styles.orderIndex}>{i + 1}</span>
                <span class={styles.orderName}>{label(app.name)}</span>
                {!NATIVE_IDS.includes(app.name) && (
                  <span class={styles.orderCustomBadge}>custom</span>
                )}
              </div>
            ))}
          </div>
          <p class={styles.hintMt}>{t.apps.appOrderHint}</p>
        </>
      )}
    </Card>
  );
}
