import { createContext } from "preact";
import { useContext, useState, useEffect, useCallback } from "preact/hooks";
import type { Settings, Stats, TransitionInfo, InfraConfig } from "../api/types";
import {
  getSettings,
  saveSettings,
  getStats,
  getTransitions,
  getConfig,
  saveConfig,
} from "../api/client";
import { toast } from "../components/Toast";
import type { ComponentChildren } from "preact";

const COLOR_KEYS = [
  "TCOL", "CHCOL", "CTCOL", "CBCOL", "WDCA", "WDCI",
  "TIME_COL", "DATE_COL", "TEMP_COL", "HUM_COL", "BAT_COL",
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

interface SettingsContextValue {
  settings: Settings | null;
  config: InfraConfig | null;
  stats: Stats | null;
  transitions: TransitionInfo[];
  updateSettings: (patch: Partial<Settings>) => void;
  updateConfig: (key: string, val: unknown) => void;
  saveDisplaySettings: (fields: Partial<Settings>) => Promise<void>;
  saveInfraConfig: () => Promise<void>;
  reload: () => void;
  apiAvailable: boolean;
}

const SettingsContext = createContext<SettingsContextValue>(null!);

export function SettingsProvider({ children }: { children: ComponentChildren }) {
  const [settings, setSettings] = useState<Settings | null>(null);
  const [config, setConfig] = useState<InfraConfig | null>(null);
  const [stats, setStats] = useState<Stats | null>(null);
  const [transitions, setTransitions] = useState<TransitionInfo[]>([]);
  const [apiAvailable, setApiAvailable] = useState(true);

  const load = useCallback(() => {
    getSettings()
      .then(setSettings)
      .catch(() => setApiAvailable(false));
    getStats().then(setStats).catch(() => {});
    getTransitions().then(setTransitions).catch(() => {});
    getConfig().then((c) => setConfig(c as InfraConfig)).catch(() => {});
  }, []);

  useEffect(() => { load(); }, [load]);

  function updateSettings(patch: Partial<Settings>) {
    setSettings((prev) => (prev ? { ...prev, ...patch } : prev));
  }

  function updateConfig(key: string, val: unknown) {
    setConfig((prev) => (prev ? { ...prev, [key]: val } : prev));
  }

  async function saveDisplaySettings(fields: Partial<Settings>) {
    try {
      await saveSettings(prepareSettingsForSave(fields) as Partial<Settings>);
      toast("Display settings saved!");
    } catch {
      toast("Error saving");
    }
  }

  async function handleSaveInfraConfig() {
    if (!config) return;
    try {
      await saveConfig(config);
      toast("Config saved & applied!");
    } catch {
      toast("Error saving config");
    }
  }

  return (
    <SettingsContext.Provider
      value={{
        settings,
        config,
        stats,
        transitions,
        updateSettings,
        updateConfig,
        saveDisplaySettings,
        saveInfraConfig: handleSaveInfraConfig,
        reload: load,
        apiAvailable,
      }}
    >
      {children}
    </SettingsContext.Provider>
  );
}

export function useSettings() {
  return useContext(SettingsContext);
}
