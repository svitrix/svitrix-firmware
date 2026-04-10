import { useState, useEffect } from "preact/hooks";
import {
  getSettings,
  saveSettings,
  getStats,
  getTransitions,
  getConfig,
  saveConfig,
  scanWifi,
  connectWifi,
  uploadFile,
  reboot,
  resetSettings,
} from "../api/client";
import type { Settings, Stats, TransitionInfo, InfraConfig } from "../api/types";
import { toast } from "../components/Toast";

function intToHex(n: number): string {
  return "#" + (n & 0xffffff).toString(16).padStart(6, "0");
}

function hexToInt(hex: string): number {
  return parseInt(hex.replace("#", ""), 16);
}

function ColorField({
  label,
  value,
  onChange,
}: {
  label: string;
  value: number;
  onChange: (v: number) => void;
}) {
  return (
    <div class="form-group">
      <label>
        <input
          type="color"
          value={intToHex(value)}
          onInput={(e) => onChange(hexToInt((e.target as HTMLInputElement).value))}
        />
        {label}
      </label>
    </div>
  );
}

function Toggle({
  label,
  checked,
  onChange,
}: {
  label: string;
  checked: boolean;
  onChange: (v: boolean) => void;
}) {
  return (
    <label>
      <input
        type="checkbox"
        checked={checked}
        onChange={(e) => onChange((e.target as HTMLInputElement).checked)}
      />
      {label}
    </label>
  );
}

function TextField({
  label,
  value,
  onChange,
  type = "text",
  placeholder,
}: {
  label: string;
  value: string | number;
  onChange: (v: string) => void;
  type?: string;
  placeholder?: string;
}) {
  return (
    <div class="form-group">
      <label>{label}</label>
      <input
        type={type}
        value={value}
        placeholder={placeholder}
        onInput={(e) => onChange((e.target as HTMLInputElement).value)}
      />
    </div>
  );
}

export function SettingsPage(_props: { path?: string }) {
  const [s, setS] = useState<Settings | null>(null);
  const [stats, setStats] = useState<Stats | null>(null);
  const [transitions, setTransitions] = useState<TransitionInfo[]>([]);
  const [cfg, setCfg] = useState<InfraConfig | null>(null);
  const [saving, setSaving] = useState(false);

  // WiFi
  const [networks, setNetworks] = useState<Array<{ ssid: string; rssi: number; secure: number }>>([]);
  const [scanning, setScanning] = useState(false);
  const [wifiSsid, setWifiSsid] = useState("");
  const [wifiPass, setWifiPass] = useState("");

  // Icon picker
  const [iconId, setIconId] = useState("");
  const [iconPreview, setIconPreview] = useState("");

  useEffect(() => {
    getSettings().then(setS);
    getStats().then(setStats);
    getTransitions().then(setTransitions);
    getConfig().then((c) => setCfg(c as InfraConfig)).catch(() => {});
  }, []);

  if (!s) return <p>Loading...</p>;

  function upd(patch: Partial<Settings>) {
    setS((prev) => (prev ? { ...prev, ...patch } : prev));
  }

  function updCfg(key: string, val: unknown) {
    setCfg((prev) => (prev ? { ...prev, [key]: val } : prev));
  }

  async function handleSaveDisplay() {
    setSaving(true);
    try {
      await saveSettings(s!);
      toast("Display settings saved!");
    } catch {
      toast("Error saving");
    }
    setSaving(false);
  }

  async function handleSaveInfra() {
    if (!cfg) return;
    setSaving(true);
    try {
      await saveConfig(cfg);
      toast("Config saved & applied!");
    } catch {
      toast("Error saving config");
    }
    setSaving(false);
  }

  async function doScan() {
    setScanning(true);
    try {
      const nets = await scanWifi();
      setNetworks(nets.sort((a, b) => b.rssi - a.rssi));
    } catch {
      toast("Scan failed");
    }
    setScanning(false);
  }

  async function doConnect() {
    if (!wifiSsid) { toast("Enter SSID"); return; }
    try {
      await connectWifi(wifiSsid, wifiPass);
      toast("Connecting... device will get new IP");
    } catch {
      toast("Connection failed");
    }
  }

  async function downloadIcon() {
    if (!iconId) { toast("Enter icon ID"); return; }
    try {
      const res = await fetch(
        `https://developer.lametric.com/content/apps/icon_thumbs/${iconId}`
      );
      const blob = await res.blob();
      const ct = res.headers.get("content-type") || "";
      let ext = ".jpg";
      if (ct.includes("gif")) ext = ".gif";

      if (ext === ".jpg" && (ct.includes("jpeg") || ct.includes("png"))) {
        const img = new Image();
        const url = URL.createObjectURL(blob);
        img.onload = async () => {
          const canvas = document.createElement("canvas");
          canvas.width = img.width;
          canvas.height = img.height;
          canvas.getContext("2d")!.drawImage(img, 0, 0);
          canvas.toBlob(async (jpgBlob) => {
            if (jpgBlob) {
              await uploadFile(`/ICONS/${iconId}.jpg`, jpgBlob);
              toast("Icon saved!");
            }
          }, "image/jpeg", 1);
          URL.revokeObjectURL(url);
        };
        img.src = url;
      } else {
        await uploadFile(`/ICONS/${iconId}.gif`, blob);
        toast("Icon saved!");
      }
    } catch {
      toast("Icon download failed");
    }
  }

  return (
    <div style={{ display: "flex", flexDirection: "column", gap: 16 }}>
      {/* Stats bar */}
      {stats && (
        <div class="card" style={{ display: "flex", gap: 16, flexWrap: "wrap", fontSize: 13 }}>
          <span>FW: {stats.version}</span>
          <span>RAM: {(stats.ram / 1024).toFixed(0)} KB</span>
          <span>WiFi: {stats.wifi_signal} dBm</span>
          <span>Lux: {stats.lux}</span>
          <span>Uptime: {stats.uptime}</span>
          {stats.temp > 0 && <span>Temp: {stats.temp}°</span>}
          {stats.hum > 0 && <span>Hum: {stats.hum}%</span>}
          <span>Bri: {stats.bri}</span>
        </div>
      )}

      {/* WiFi */}
      <div class="card">
        <h3 style={{ marginBottom: 12 }}>WiFi</h3>
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <div style={{ display: "flex", gap: 8 }}>
            <button onClick={doScan} disabled={scanning}>
              {scanning ? "Scanning..." : "Scan Networks"}
            </button>
          </div>
          {networks.length > 0 && (
            <div style={{ maxHeight: 150, overflow: "auto", fontSize: 13 }}>
              {networks.map((n) => (
                <div
                  key={n.ssid}
                  style={{
                    padding: "4px 8px",
                    cursor: "pointer",
                    borderBottom: "1px solid var(--border)",
                  }}
                  onClick={() => setWifiSsid(n.ssid)}
                >
                  {n.ssid} ({n.rssi} dBm) {n.secure ? "🔒" : ""}
                </div>
              ))}
            </div>
          )}
          <div class="form-row">
            <TextField label="SSID" value={wifiSsid} onChange={setWifiSsid} />
            <TextField label="Password" value={wifiPass} onChange={setWifiPass} type="password" />
          </div>
          <button class="btn-primary" onClick={doConnect}>Connect</button>
        </div>
      </div>

      {/* Network */}
      {cfg && (
        <div class="card">
          <h3 style={{ marginBottom: 12 }}>Network</h3>
          <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
            <Toggle
              label="Static IP"
              checked={!!cfg["Static IP"]}
              onChange={(v) => updCfg("Static IP", v)}
            />
            {cfg["Static IP"] && (
              <div class="form-row">
                <TextField label="Local IP" value={cfg["Local IP"] || ""} onChange={(v) => updCfg("Local IP", v)} />
                <TextField label="Gateway" value={cfg["Gateway"] || ""} onChange={(v) => updCfg("Gateway", v)} />
                <TextField label="Subnet" value={cfg["Subnet"] || ""} onChange={(v) => updCfg("Subnet", v)} />
                <TextField label="Primary DNS" value={cfg["Primary DNS"] || ""} onChange={(v) => updCfg("Primary DNS", v)} />
              </div>
            )}
          </div>
        </div>
      )}

      {/* MQTT */}
      {cfg && (
        <div class="card">
          <h3 style={{ marginBottom: 12 }}>MQTT</h3>
          <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
            <div class="form-row">
              <TextField label="Broker" value={cfg["Broker"] || ""} onChange={(v) => updCfg("Broker", v)} />
              <TextField label="Port" value={cfg["Port"] || 1883} onChange={(v) => updCfg("Port", parseInt(v) || 1883)} type="number" />
            </div>
            <div class="form-row">
              <TextField label="Username" value={cfg["Username"] || ""} onChange={(v) => updCfg("Username", v)} />
              <TextField label="Password" value={cfg["Password"] || ""} onChange={(v) => updCfg("Password", v)} type="password" />
            </div>
            <TextField label="Prefix" value={cfg["Prefix"] || ""} onChange={(v) => updCfg("Prefix", v)} placeholder="svitrix" />
            <Toggle
              label="Home Assistant Discovery"
              checked={!!cfg["Homeassistant Discovery"]}
              onChange={(v) => updCfg("Homeassistant Discovery", v)}
            />
          </div>
        </div>
      )}

      {/* Time */}
      {cfg && (
        <div class="card">
          <h3 style={{ marginBottom: 12 }}>NTP & Timezone</h3>
          <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
            <TextField label="NTP Server" value={cfg["NTP Server"] || ""} onChange={(v) => updCfg("NTP Server", v)} placeholder="pool.ntp.org" />
            <TextField label="Timezone" value={cfg["Timezone"] || ""} onChange={(v) => updCfg("Timezone", v)} placeholder="CET-1CEST,M3.5.0,M10.5.0/3" />
            <p style={{ fontSize: 12, color: "var(--text-dim)" }}>
              Find your timezone at{" "}
              <a href="https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv" target="_blank" rel="noopener">
                posix_tz_db
              </a>
            </p>
          </div>
        </div>
      )}

      {/* Auth */}
      {cfg && (
        <div class="card">
          <h3 style={{ marginBottom: 12 }}>Authentication</h3>
          <div class="form-row">
            <TextField label="Username" value={cfg["Auth Username"] || ""} onChange={(v) => updCfg("Auth Username", v)} />
            <TextField label="Password" value={cfg["Auth Password"] || ""} onChange={(v) => updCfg("Auth Password", v)} type="password" />
          </div>
          <p style={{ fontSize: 12, color: "var(--text-dim)", marginTop: 8 }}>
            Leave empty to disable HTTP authentication.
          </p>
        </div>
      )}

      {/* Save infra config */}
      {cfg && (
        <button class="btn-primary" onClick={handleSaveInfra} disabled={saving}>
          {saving ? "Saving..." : "Save Network/MQTT/Time/Auth"}
        </button>
      )}

      {/* Display */}
      <div class="card">
        <h3 style={{ marginBottom: 12 }}>Display</h3>
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <Toggle label="Matrix Power" checked={s.MATP} onChange={(v) => upd({ MATP: v })} />
          <Toggle label="Auto Brightness" checked={s.ABRI} onChange={(v) => upd({ ABRI: v })} />
          <div class="form-group">
            <label>
              Brightness: {s.BRI}
              <input type="range" min={0} max={255} value={s.BRI}
                onInput={(e) => upd({ BRI: +(e.target as HTMLInputElement).value })} />
            </label>
          </div>
          <div class="form-group">
            <label>
              Gamma: {s.GAMMA}
              <input type="range" min={0.5} max={3} step={0.1} value={s.GAMMA}
                onInput={(e) => upd({ GAMMA: +(e.target as HTMLInputElement).value })} />
            </label>
          </div>
          <Toggle label="Uppercase" checked={s.UPPERCASE} onChange={(v) => upd({ UPPERCASE: v })} />
        </div>
      </div>

      {/* Apps */}
      <div class="card">
        <h3 style={{ marginBottom: 12 }}>Apps</h3>
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <Toggle label="Time" checked={s.TIM} onChange={(v) => upd({ TIM: v })} />
          <Toggle label="Date" checked={s.DAT} onChange={(v) => upd({ DAT: v })} />
          <Toggle label="Temperature" checked={s.TEMP} onChange={(v) => upd({ TEMP: v })} />
          <Toggle label="Humidity" checked={s.HUM} onChange={(v) => upd({ HUM: v })} />
          <Toggle label="Battery" checked={s.BAT} onChange={(v) => upd({ BAT: v })} />
          <div class="form-group">
            <label>
              App Duration: {s.ATIME}s
              <input type="range" min={1} max={60} value={s.ATIME}
                onInput={(e) => upd({ ATIME: +(e.target as HTMLInputElement).value })} />
            </label>
          </div>
          <Toggle label="Auto Transition" checked={s.ATRANS} onChange={(v) => upd({ ATRANS: v })} />
          {transitions.length > 0 && (
            <div class="form-group">
              <label>Transition Effect</label>
              <select value={s.TEFF} onChange={(e) => upd({ TEFF: +(e.target as HTMLSelectElement).value })}>
                {transitions.map((t, i) => <option key={i} value={i}>{t.name}</option>)}
              </select>
            </div>
          )}
          <div class="form-group">
            <label>
              Transition Speed: {s.TSPEED}ms
              <input type="range" min={100} max={2000} step={100} value={s.TSPEED}
                onInput={(e) => upd({ TSPEED: +(e.target as HTMLInputElement).value })} />
            </label>
          </div>
          <div class="form-group">
            <label>
              Scroll Speed: {s.SSPEED}
              <input type="range" min={10} max={100} value={s.SSPEED}
                onInput={(e) => upd({ SSPEED: +(e.target as HTMLInputElement).value })} />
            </label>
          </div>
          <Toggle label="Block Navigation" checked={s.BLOCKN} onChange={(v) => upd({ BLOCKN: v })} />
        </div>
      </div>

      {/* Time & Date */}
      <div class="card">
        <h3 style={{ marginBottom: 12 }}>Time & Date Format</h3>
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <div class="form-row">
            <TextField label="Time Format" value={s.TFORMAT} onChange={(v) => upd({ TFORMAT: v })} />
            <TextField label="Date Format" value={s.DFORMAT} onChange={(v) => upd({ DFORMAT: v })} />
          </div>
          <div class="form-group">
            <label>Time Mode</label>
            <select value={s.TMODE} onChange={(e) => upd({ TMODE: +(e.target as HTMLSelectElement).value })}>
              <option value={0}>24h</option>
              <option value={1}>12h</option>
            </select>
          </div>
          <Toggle label="Start on Monday" checked={s.SOM} onChange={(v) => upd({ SOM: v })} />
          <Toggle label="Celsius" checked={s.CEL} onChange={(v) => upd({ CEL: v })} />
          <Toggle label="Show Weekday" checked={s.WD} onChange={(v) => upd({ WD: v })} />
        </div>
      </div>

      {/* Colors */}
      <div class="card">
        <h3 style={{ marginBottom: 12 }}>Colors</h3>
        <div style={{ display: "flex", flexWrap: "wrap", gap: 12 }}>
          <ColorField label="Text" value={s.TCOL} onChange={(v) => upd({ TCOL: v })} />
          <ColorField label="Time" value={s.TIME_COL} onChange={(v) => upd({ TIME_COL: v })} />
          <ColorField label="Date" value={s.DATE_COL} onChange={(v) => upd({ DATE_COL: v })} />
          <ColorField label="Temp" value={s.TEMP_COL} onChange={(v) => upd({ TEMP_COL: v })} />
          <ColorField label="Hum" value={s.HUM_COL} onChange={(v) => upd({ HUM_COL: v })} />
          <ColorField label="Battery" value={s.BAT_COL} onChange={(v) => upd({ BAT_COL: v })} />
          <ColorField label="Weekday Active" value={s.WDCA} onChange={(v) => upd({ WDCA: v })} />
          <ColorField label="Weekday Inactive" value={s.WDCI} onChange={(v) => upd({ WDCI: v })} />
          <ColorField label="Cal Header" value={s.CHCOL} onChange={(v) => upd({ CHCOL: v })} />
          <ColorField label="Cal Text" value={s.CTCOL} onChange={(v) => upd({ CTCOL: v })} />
          <ColorField label="Cal Body" value={s.CBCOL} onChange={(v) => upd({ CBCOL: v })} />
        </div>
        <div class="form-row" style={{ marginTop: 12 }}>
          <div class="form-group">
            <label>Color Correction</label>
            <input type="color" value={s.CCORRECTION}
              onInput={(e) => upd({ CCORRECTION: (e.target as HTMLInputElement).value })} />
          </div>
          <div class="form-group">
            <label>Color Temperature</label>
            <input type="color" value={s.CTEMP}
              onInput={(e) => upd({ CTEMP: (e.target as HTMLInputElement).value })} />
          </div>
        </div>
      </div>

      {/* Sound */}
      <div class="card">
        <h3 style={{ marginBottom: 12 }}>Sound</h3>
        <Toggle label="Sound Enabled" checked={s.SOUND} onChange={(v) => upd({ SOUND: v })} />
        <div class="form-group" style={{ marginTop: 10 }}>
          <label>
            Volume: {s.VOL}
            <input type="range" min={0} max={30} value={s.VOL}
              onInput={(e) => upd({ VOL: +(e.target as HTMLInputElement).value })} />
          </label>
        </div>
      </div>

      {/* Icon Picker */}
      <div class="card">
        <h3 style={{ marginBottom: 12 }}>Icon Picker</h3>
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <div class="form-row">
            <TextField label="LaMetric Icon ID" value={iconId} onChange={setIconId} placeholder="13" />
            <div class="form-group" style={{ justifyContent: "flex-end" }}>
              <div style={{ display: "flex", gap: 6 }}>
                <button onClick={() => {
                  if (iconId) setIconPreview(`https://developer.lametric.com/content/apps/icon_thumbs/${iconId}`);
                }}>Preview</button>
                <button class="btn-primary" onClick={downloadIcon}>Download</button>
              </div>
            </div>
          </div>
          {iconPreview && (
            <div style={{
              width: 150, height: 150, background: "#000",
              borderRadius: "var(--radius)", border: "1px solid var(--border)",
              display: "flex", alignItems: "center", justifyContent: "center",
            }}>
              <img
                src={iconPreview}
                style={{ imageRendering: "pixelated", maxWidth: 150, maxHeight: 150 }}
                onError={() => { setIconPreview(""); toast("Icon not found"); }}
              />
            </div>
          )}
        </div>
      </div>

      {/* Save display + actions */}
      <div style={{ display: "flex", gap: 8, flexWrap: "wrap" }}>
        <button class="btn-primary" onClick={handleSaveDisplay} disabled={saving}>
          {saving ? "Saving..." : "Save Display Settings"}
        </button>
        <button onClick={() => {
          if (confirm("Reset all settings to defaults?")) {
            resetSettings().then(() => { toast("Settings reset"); getSettings().then(setS); });
          }
        }}>Reset Defaults</button>
        <button class="btn-danger" onClick={() => {
          if (confirm("Reboot device?")) reboot().then(() => toast("Rebooting..."));
        }}>Reboot</button>
      </div>
    </div>
  );
}
