import { createContext } from "preact";
import { useContext, useState, useEffect, useCallback, useRef } from "preact/hooks";
import i18n from "../i18n";
import type { Settings, Stats, TransitionInfo, EffectInfo, InfraConfig } from "../api/types";
import {
  getSettings,
  saveSettings,
  getStats,
  getTransitions,
  getEffects,
  getConfig,
  saveConfig,
  ping,
} from "../api/client";
import { toast } from "../components/Toast";
import type { ComponentChildren } from "preact";

const COLOR_KEYS = [
  "TCOL", "CHCOL", "CTCOL", "CBCOL", "WDCA", "WDCI",
  "TIME_COL", "DATE_COL", "TEMP_COL", "HUM_COL", "BAT_COL", "NCOL",
];

function prepareSettingsForSave(fields: Partial<Settings>): Record<string, unknown> {
  const out: Record<string, unknown> = { ...fields } as unknown as Record<string, unknown>;
  for (const key of COLOR_KEYS) {
    if (key in out) {
      const v = out[key];
      if (typeof v === "number") {
        out[key] = "#" + (v & 0xffffff).toString(16).padStart(6, "0");
      }
    }
  }
  return out;
}

type SaveState = "idle" | "saving" | "saved" | "error";

interface SettingsContextValue {
  settings: Settings | null;
  config: InfraConfig | null;
  stats: Stats | null;
  transitions: TransitionInfo[];
  effects: EffectInfo[];
  loading: boolean;
  updateSettings: (patch: Partial<Settings>) => void;
  setSetting: (patch: Partial<Settings>, immediate?: boolean) => void;
  saveState: SaveState;
  updateConfig: <K extends keyof InfraConfig>(key: K, val: InfraConfig[K]) => void;
  saveDisplaySettings: (fields: Partial<Settings>) => Promise<void>;
  saveInfraConfig: () => Promise<void>;
  reload: () => void;
  apiAvailable: boolean;
  /** Live reachability of the device, driven by a background heartbeat. */
  online: boolean;
  /** Timestamp (ms) of the last successful heartbeat, or null if never seen. */
  lastSeen: number | null;
}

const SettingsContext = createContext<SettingsContextValue>(null!);

export function SettingsProvider({ children }: { children: ComponentChildren }) {
  const [settings, setSettings] = useState<Settings | null>(null);
  const [config, setConfig] = useState<InfraConfig | null>(null);
  const [stats, setStats] = useState<Stats | null>(null);
  const [transitions, setTransitions] = useState<TransitionInfo[]>([]);
  const [effects, setEffects] = useState<EffectInfo[]>([]);
  const [apiAvailable, setApiAvailable] = useState(true);
  const [loading, setLoading] = useState(true);
  const [saveState, setSaveState] = useState<SaveState>("idle");
  const [online, setOnline] = useState(true);
  const [lastSeen, setLastSeen] = useState<number | null>(null);
  const pending = useRef<Partial<Settings>>({});
  const flushTimer = useRef<ReturnType<typeof setTimeout>>();

  const load = useCallback(() => {
    setLoading(true);
    Promise.allSettled([
      getSettings()
        .then((s) => {
          setSettings(s);
          setOnline(true);
          setLastSeen(Date.now());
        })
        .catch(() => setApiAvailable(false)),
      getStats().then(setStats).catch(() => {}),
      getTransitions().then(setTransitions).catch(() => {}),
      getEffects().then(setEffects).catch(() => {}),
      getConfig().then((c) => setConfig(c as unknown as InfraConfig)).catch(() => {}),
    ]).finally(() => setLoading(false));
  }, []);

  useEffect(() => { load(); }, [load]);

  // Lightweight heartbeat: probe the cheapest endpoint (`/version`) every ~10s
  // so the UI can show an honest "unreachable" banner instead of silently
  // failing. Distinguishes browser-offline from device-down.
  useEffect(() => {
    let active = true;
    const beat = async () => {
      if (typeof navigator !== "undefined" && navigator.onLine === false) {
        if (active) setOnline(false);
        return;
      }
      try {
        await ping();
        if (!active) return;
        setOnline(true);
        setLastSeen(Date.now());
      } catch {
        if (active) setOnline(false);
      }
    };
    const id = setInterval(beat, 10000);
    const onNavChange = () => beat();
    window.addEventListener("online", onNavChange);
    window.addEventListener("offline", onNavChange);
    return () => {
      active = false;
      clearInterval(id);
      window.removeEventListener("online", onNavChange);
      window.removeEventListener("offline", onNavChange);
    };
  }, []);

  function updateSettings(patch: Partial<Settings>) {
    setSettings((prev) => (prev ? { ...prev, ...patch } : prev));
  }

  const flush = useCallback(async () => {
    const patch = pending.current;
    pending.current = {};
    if (!Object.keys(patch).length) return;
    setSaveState("saving");
    try {
      const res = await saveSettings(prepareSettingsForSave(patch) as Partial<Settings>);
      if (!res.ok) throw new Error(`HTTP ${res.status}`);
      setSaveState("saved");
      setTimeout(() => setSaveState((s) => (s === "saved" ? "idle" : s)), 1400);
    } catch {
      // Re-queue the failed fields (under any newer edits) so a later save retries them.
      pending.current = { ...patch, ...pending.current };
      setSaveState("error");
      toast(i18n.t("common.errorSaving"), { error: true });
    }
  }, []);

  // immediate=true for toggles/selects/swatches; debounced for sliders/text
  const setSetting = useCallback(
    (patch: Partial<Settings>, immediate = false) => {
      updateSettings(patch); // optimistic local update
      pending.current = { ...pending.current, ...patch };
      clearTimeout(flushTimer.current);
      if (immediate) flush();
      else flushTimer.current = setTimeout(flush, 400);
    },
    [flush],
  );

  function updateConfig<K extends keyof InfraConfig>(key: K, val: InfraConfig[K]) {
    setConfig((prev) => (prev ? { ...prev, [key]: val } : prev));
  }

  async function saveDisplaySettings(fields: Partial<Settings>) {
    try {
      await saveSettings(prepareSettingsForSave(fields) as Partial<Settings>);
      toast(i18n.t("common.displaySettingsSaved"));
    } catch {
      toast(i18n.t("common.errorSaving"));
    }
  }

  async function handleSaveInfraConfig() {
    if (!config) return;
    try {
      await saveConfig(config as unknown as Record<string, unknown>);
      toast(i18n.t("common.configSaved"));
    } catch (e) {
      toast(i18n.t("common.errorSavingConfig"), { error: true });
      throw e; // let InfraSaveBar skip the reboot overlay on a failed save
    }
  }

  return (
    <SettingsContext.Provider
      value={{
        settings,
        config,
        stats,
        transitions,
        effects,
        loading,
        updateSettings,
        setSetting,
        saveState,
        updateConfig,
        saveDisplaySettings,
        saveInfraConfig: handleSaveInfraConfig,
        reload: load,
        apiAvailable,
        online,
        lastSeen,
      }}
    >
      {children}
    </SettingsContext.Provider>
  );
}

export function useSettings() {
  return useContext(SettingsContext);
}
