import { createContext } from "preact";
import { useContext, useState, useEffect, useCallback } from "preact/hooks";
import type { Settings, Stats, TransitionInfo, EffectInfo, InfraConfig, WeatherConfig } from "../api/types";
import {
  getSettings,
  saveSettings,
  getStats,
  getTransitions,
  getEffects,
  getConfig,
  saveConfig,
  getWeatherConfig,
  saveWeatherConfig as apiSaveWeatherConfig,
} from "../api/client";
import { toast } from "../components/Toast";
import { getT } from "../i18n";
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

interface SettingsContextValue {
  settings: Settings | null;
  config: InfraConfig | null;
  weatherConfig: WeatherConfig | null;
  stats: Stats | null;
  transitions: TransitionInfo[];
  effects: EffectInfo[];
  loading: boolean;
  updateSettings: (patch: Partial<Settings>) => void;
  updateConfig: <K extends keyof InfraConfig>(key: K, val: InfraConfig[K]) => void;
  updateWeatherConfig: (patch: Partial<WeatherConfig>) => void;
  saveDisplaySettings: (fields: Partial<Settings>, successMsg?: string) => Promise<void>;
  saveInfraConfig: (successMsg?: string) => Promise<void>;
  saveWeatherConfig: (successMsg?: string) => Promise<void>;
  reload: () => void;
  apiAvailable: boolean;
}

const SettingsContext = createContext<SettingsContextValue>(null!);

export function SettingsProvider({ children }: { children: ComponentChildren }) {
  const [settings, setSettings] = useState<Settings | null>(null);
  const [config, setConfig] = useState<InfraConfig | null>(null);
  const [weatherConfig, setWeatherConfig] = useState<WeatherConfig | null>(null);
  const [stats, setStats] = useState<Stats | null>(null);
  const [transitions, setTransitions] = useState<TransitionInfo[]>([]);
  const [effects, setEffects] = useState<EffectInfo[]>([]);
  const [apiAvailable, setApiAvailable] = useState(true);
  const [loading, setLoading] = useState(true);

  const load = useCallback(() => {
    setLoading(true);
    Promise.allSettled([
      getSettings().then(setSettings).catch(() => setApiAvailable(false)),
      getStats().then(setStats).catch(() => {}),
      getTransitions().then(setTransitions).catch(() => {}),
      getEffects().then(setEffects).catch(() => {}),
      getConfig().then((c) => setConfig(c as unknown as InfraConfig)).catch(() => {}),
      getWeatherConfig().then(setWeatherConfig).catch(() => {}),
    ]).finally(() => setLoading(false));
  }, []);

  useEffect(() => { load(); }, [load]);

  function updateSettings(patch: Partial<Settings>) {
    setSettings((prev) => (prev ? { ...prev, ...patch } : prev));
  }

  function updateConfig<K extends keyof InfraConfig>(key: K, val: InfraConfig[K]) {
    setConfig((prev) => (prev ? { ...prev, [key]: val } : prev));
  }

  function updateWeatherConfig(patch: Partial<WeatherConfig>) {
    setWeatherConfig((prev) => (prev ? { ...prev, ...patch } : prev));
  }

  async function saveDisplaySettings(fields: Partial<Settings>, successMsg?: string) {
    const t = getT();
    try {
      await saveSettings(prepareSettingsForSave(fields) as Partial<Settings>);
      if (successMsg !== "") toast(successMsg || t.ok);
    } catch {
      toast(t.errorSaving);
    }
  }

  async function handleSaveInfraConfig(successMsg?: string) {
    if (!config) return;
    const t = getT();
    try {
      await saveConfig(config as unknown as Record<string, unknown>);
      if (successMsg !== "") toast(successMsg || t.ok);
    } catch {
      toast(t.errorSaving);
    }
  }

  async function handleSaveWeatherConfig(successMsg?: string) {
    if (!weatherConfig) return;
    const t = getT();
    try {
      await apiSaveWeatherConfig(weatherConfig);
      if (successMsg !== "") toast(successMsg || t.ok);
    } catch {
      toast(t.errorSaving);
    }
  }

  return (
    <SettingsContext.Provider
      value={{
        settings,
        config,
        weatherConfig,
        stats,
        transitions,
        effects,
        loading,
        updateSettings,
        updateConfig,
        updateWeatherConfig,
        saveDisplaySettings,
        saveInfraConfig: handleSaveInfraConfig,
        saveWeatherConfig: handleSaveWeatherConfig,
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
