import { createContext } from "preact";
import { useContext, useState, useEffect, useCallback, useRef } from "preact/hooks";
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
import { prepareForSave, useDebounce } from "../hooks/useAutoSave";
import type { ComponentChildren } from "preact";

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
  autoSave: (fields: Partial<Settings>) => void;
  instantSave: (fields: Partial<Settings>) => Promise<void>;
  reload: () => void;
  apiAvailable: boolean;
  appsVersion: number;
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
  const [appsVersion, setAppsVersion] = useState(0);

  const pendingAutoSave = useRef<Partial<Settings>>({});

  const load = useCallback(() => {
    setLoading(true);
    Promise.allSettled([
      getSettings().then(setSettings).catch((e) => { console.error("Failed to load settings:", e); setApiAvailable(false); }),
      getStats().then(setStats).catch((e) => console.error("Failed to load stats:", e)),
      getTransitions().then(setTransitions).catch((e) => console.error("Failed to load transitions:", e)),
      getEffects().then(setEffects).catch((e) => console.error("Failed to load effects:", e)),
      getConfig().then((c) => setConfig(c as unknown as InfraConfig)).catch((e) => console.error("Failed to load config:", e)),
      getWeatherConfig().then(setWeatherConfig).catch((e) => console.error("Failed to load weather config:", e)),
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

  const flushAutoSave = useCallback(async () => {
    if (Object.keys(pendingAutoSave.current).length === 0) return;
    const toSave = { ...pendingAutoSave.current };
    pendingAutoSave.current = {};
    try {
      await saveSettings(prepareForSave(toSave) as Partial<Settings>);
    } catch (e) {
      console.error("Auto-save failed:", e);
      const t = getT();
      toast(t.errorSaving);
    }
  }, []);

  const debouncedFlush = useDebounce(flushAutoSave, 500);

  const autoSave = useCallback((fields: Partial<Settings>) => {
    setSettings((prev) => (prev ? { ...prev, ...fields } : prev));
    pendingAutoSave.current = { ...pendingAutoSave.current, ...fields };
    debouncedFlush();
  }, [debouncedFlush]);

  const instantSave = useCallback(async (fields: Partial<Settings>) => {
    setSettings((prev) => (prev ? { ...prev, ...fields } : prev));
    try {
      await saveSettings(prepareForSave(fields) as Partial<Settings>);
    } catch (e) {
      console.error("Instant save failed:", e);
      const t = getT();
      toast(t.errorSaving);
    }
  }, []);

  async function saveDisplaySettings(fields: Partial<Settings>, successMsg?: string) {
    const t = getT();
    try {
      await saveSettings(prepareForSave(fields) as Partial<Settings>);
      setAppsVersion((v) => v + 1);
      if (successMsg !== "") toast(successMsg || t.ok);
    } catch (e) {
      console.error("Save display settings failed:", e);
      toast(t.errorSaving);
    }
  }

  async function handleSaveInfraConfig(successMsg?: string) {
    if (!config) return;
    const t = getT();
    try {
      await saveConfig(config as unknown as Record<string, unknown>);
      if (successMsg !== "") toast(successMsg || t.ok);
    } catch (e) {
      console.error("Save infra config failed:", e);
      toast(t.errorSaving);
    }
  }

  async function handleSaveWeatherConfig(successMsg?: string) {
    if (!weatherConfig) return;
    const t = getT();
    try {
      await apiSaveWeatherConfig(weatherConfig);
      setAppsVersion((v) => v + 1);
      if (successMsg !== "") toast(successMsg || t.ok);
    } catch (e) {
      console.error("Save weather config failed:", e);
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
        autoSave,
        instantSave,
        reload: load,
        apiAvailable,
        appsVersion,
      }}
    >
      {children}
    </SettingsContext.Provider>
  );
}

export function useSettings() {
  return useContext(SettingsContext);
}
