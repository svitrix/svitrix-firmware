import type {
  Settings,
  Stats,
  AppInfo,
  DataSource,
  FileEntry,
  Notification,
  Indicator,
  EffectInfo,
  TransitionInfo,
} from "./types";

async function get<T>(url: string): Promise<T> {
  const res = await fetch(url);
  if (!res.ok) throw new Error(`${res.status} ${res.statusText}`);
  return res.json();
}

async function post(url: string, body?: unknown): Promise<Response> {
  return fetch(url, {
    method: "POST",
    headers: body ? { "Content-Type": "application/json" } : undefined,
    body: body ? JSON.stringify(body) : undefined,
  });
}

async function del(url: string): Promise<Response> {
  return fetch(url, { method: "DELETE" });
}

// Settings
export const getSettings = () => get<Settings>("/api/settings");
export const saveSettings = (s: Partial<Settings>) => post("/api/settings", s);

// Stats
export const getStats = () => get<Stats>("/api/stats");

// Screen buffer (256 ints, 0xRRGGBB)
export const getScreen = () => get<number[]>("/api/screen");

// Apps
export const getApps = () => get<AppInfo[]>("/api/apps");
export const getLoop = () => get<AppInfo[]>("/api/loop");
export const updateApps = (data: unknown) => post("/api/apps", data);
export const switchApp = (name: string) => post("/api/switch", { name });
export const nextApp = () => fetch("/api/nextapp");
export const previousApp = () => post("/api/previousapp");
export const reorderApps = (order: string[]) => post("/api/reorder", order);

// Custom apps
export const setCustomApp = (name: string, data: unknown) =>
  post(`/api/custom?name=${encodeURIComponent(name)}`, data);
export const deleteCustomApp = (name: string) =>
  post(`/api/custom?name=${encodeURIComponent(name)}`);

// Notifications
export const sendNotify = (n: Notification) => post("/api/notify", n);
export const dismissNotify = () => fetch("/api/notify/dismiss");
export const setIndicator = (id: 1 | 2 | 3, data: Indicator) =>
  post(`/api/indicator${id}`, data);

// Audio
export const playRtttl = (melody: string) => post("/api/rtttl", { rtttl: melody });
export const playSound = (sound: string) => post("/api/sound", { sound });
export const playR2D2 = () => post("/api/r2d2");

// DataFetcher
export const getDataSources = () => get<DataSource[]>("/api/datafetcher");
export const addDataSource = (src: DataSource) => post("/api/datafetcher", src);
export const deleteDataSource = (name: string) =>
  del(`/api/datafetcher?name=${encodeURIComponent(name)}`);
export const fetchDataSource = (name: string) =>
  post(`/api/datafetcher/fetch?name=${encodeURIComponent(name)}`);

// Effects & Transitions
export const getEffects = () => get<EffectInfo[]>("/api/effects");
export const getTransitions = () => get<TransitionInfo[]>("/api/transitions");

// Power & System
export const setPower = (on: boolean) => post("/api/power", { power: on });
export const setSleep = (seconds: number) => post("/api/sleep", { sleep: seconds });
export const reboot = () => fetch("/api/reboot");
export const factoryReset = () => fetch("/api/erase");
export const resetSettings = () => fetch("/api/resetSettings");
export const doUpdate = () => post("/api/doupdate");
export const getVersion = () => get<string>("/version");

// File Manager (LittleFS)
export const listDir = (dir: string) =>
  get<FileEntry[]>(`/list?dir=${encodeURIComponent(dir)}`);

export async function readFile(path: string): Promise<string> {
  const res = await fetch(`/edit?file=${encodeURIComponent(path)}`);
  return res.text();
}

export async function uploadFile(path: string, content: Blob | string): Promise<Response> {
  const form = new FormData();
  const blob = typeof content === "string" ? new Blob([content]) : content;
  form.append("data", blob, path);
  return fetch(`/edit?filename=${encodeURIComponent(path)}`, {
    method: "POST",
    body: form,
  });
}

export async function createDir(path: string): Promise<Response> {
  return fetch("/edit", {
    method: "PUT",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: `path=${encodeURIComponent(path)}`,
  });
}

export async function deleteFile(path: string): Promise<Response> {
  return del(`/edit?filename=${encodeURIComponent(path)}`);
}

// WiFi
export const scanWifi = () => get<Array<{ ssid: string; rssi: number; secure: number }>>("/scan");
export const connectWifi = (ssid: string, password: string) => {
  const form = new FormData();
  form.append("ssid", ssid);
  form.append("pass", password);
  return fetch("/connect", { method: "POST", body: form });
};

// Config file
export const getConfig = () => get<Record<string, unknown>>("/DoNotTouch.json");
