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
import { Toggle, TextField, ColorField, Slider, Select, Card, FormRow, Button } from "../components/ui";

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

  const [apiAvailable, setApiAvailable] = useState(true);

  useEffect(() => {
    getSettings()
      .then(setS)
      .catch(() => setApiAvailable(false));
    getStats().then(setStats).catch(() => {});
    getTransitions().then(setTransitions).catch(() => {});
    getConfig().then((c) => setCfg(c as InfraConfig)).catch(() => {});
  }, []);

  function WifiSetup({ description }: { description?: string }) {
    return (
      <Card title="WiFi" subtitle={description}>
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <Button onClick={doScan} disabled={scanning}>
            {scanning ? "Scanning..." : "Scan Networks"}
          </Button>
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
          <FormRow>
            <TextField label="SSID" value={wifiSsid} onChange={setWifiSsid} />
            <TextField label="Password" value={wifiPass} onChange={setWifiPass} type="password" />
          </FormRow>
          <Button variant="primary" onClick={doConnect}>Connect</Button>
        </div>
      </Card>
    );
  }

  // AP mode: show only WiFi setup when API is not available
  if (!s && !apiAvailable) {
    return (
      <div style={{ display: "flex", flexDirection: "column", gap: 16 }}>
        <WifiSetup description="Connect to your home WiFi network. After connecting, the device will reboot with full settings available." />
      </div>
    );
  }

  if (!s) return <p>Loading...</p>;

  function upd(patch: Partial<Settings>) {
    setS((prev) => (prev ? { ...prev, ...patch } : prev));
  }

  function updCfg(key: string, val: unknown) {
    setCfg((prev) => (prev ? { ...prev, [key]: val } : prev));
  }

  // Firmware expects color fields as hex strings "#RRGGBB", not numbers
  function prepareSettingsForSave(settings: Settings): Record<string, unknown> {
    const colorKeys = [
      "TCOL", "CHCOL", "CTCOL", "CBCOL", "WDCA", "WDCI",
      "TIME_COL", "DATE_COL", "TEMP_COL", "HUM_COL", "BAT_COL",
    ];
    const out: Record<string, unknown> = { ...settings } as unknown as Record<string, unknown>;
    for (const key of colorKeys) {
      const v = (settings as unknown as Record<string, unknown>)[key];
      if (typeof v === "number") {
        out[key] = "#" + (v & 0xffffff).toString(16).padStart(6, "0");
      }
    }
    return out;
  }

  async function handleSaveDisplay() {
    setSaving(true);
    try {
      await saveSettings(prepareSettingsForSave(s!) as Partial<Settings>);
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
      toast("Connecting to WiFi...");
      // Device will reboot after connecting — wait and redirect
      setTimeout(() => {
        toast("Device rebooting... check matrix for new IP");
        fetch("/restart").catch(() => {});
      }, 3000);
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
      <WifiSetup />

      {/* Network */}
      {cfg && (
        <Card title="Network">
          <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
            <Toggle
              label="Static IP"
              checked={!!cfg["Static IP"]}
              onChange={(v) => updCfg("Static IP", v)}
            />
            {cfg["Static IP"] && (
              <FormRow>
                <TextField label="Local IP" value={cfg["Local IP"] || ""} onChange={(v) => updCfg("Local IP", v)} />
                <TextField label="Gateway" value={cfg["Gateway"] || ""} onChange={(v) => updCfg("Gateway", v)} />
                <TextField label="Subnet" value={cfg["Subnet"] || ""} onChange={(v) => updCfg("Subnet", v)} />
                <TextField label="Primary DNS" value={cfg["Primary DNS"] || ""} onChange={(v) => updCfg("Primary DNS", v)} />
              </FormRow>
            )}
          </div>
        </Card>
      )}

      {/* MQTT */}
      {cfg && (
        <Card title="MQTT">
          <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
            <FormRow>
              <TextField label="Broker" value={cfg["Broker"] || ""} onChange={(v) => updCfg("Broker", v)} />
              <TextField label="Port" value={cfg["Port"] || 1883} onChange={(v) => updCfg("Port", parseInt(v) || 1883)} type="number" />
            </FormRow>
            <FormRow>
              <TextField label="Username" value={cfg["Username"] || ""} onChange={(v) => updCfg("Username", v)} />
              <TextField label="Password" value={cfg["Password"] || ""} onChange={(v) => updCfg("Password", v)} type="password" />
            </FormRow>
            <TextField label="Prefix" value={cfg["Prefix"] || ""} onChange={(v) => updCfg("Prefix", v)} placeholder="svitrix" />
            <Toggle
              label="Home Assistant Discovery"
              checked={!!cfg["Homeassistant Discovery"]}
              onChange={(v) => updCfg("Homeassistant Discovery", v)}
            />
          </div>
        </Card>
      )}

      {/* Time */}
      {cfg && (
        <Card title="NTP & Timezone">
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
        </Card>
      )}

      {/* Auth */}
      {cfg && (
        <Card title="Authentication">
          <FormRow>
            <TextField label="Username" value={cfg["Auth Username"] || ""} onChange={(v) => updCfg("Auth Username", v)} />
            <TextField label="Password" value={cfg["Auth Password"] || ""} onChange={(v) => updCfg("Auth Password", v)} type="password" />
          </FormRow>
          <p style={{ fontSize: 12, color: "var(--text-dim)", marginTop: 8 }}>
            Leave empty to disable HTTP authentication.
          </p>
        </Card>
      )}

      {/* Save infra config */}
      {cfg && (
        <Button variant="primary" onClick={handleSaveInfra} loading={saving}>
          Save Network/MQTT/Time/Auth
        </Button>
      )}

      {/* Display */}
      <Card title="Display">
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <Toggle label="Matrix Power" checked={s.MATP} onChange={(v) => upd({ MATP: v })} />
          <Toggle label="Auto Brightness" checked={s.ABRI} onChange={(v) => upd({ ABRI: v })} />
          <Slider label="Brightness" min={0} max={255} value={s.BRI} onChange={(v) => upd({ BRI: v })} />
          <Slider label="Gamma" min={0.5} max={3} step={0.1} value={s.GAMMA} onChange={(v) => upd({ GAMMA: v })} />
          <Toggle label="Uppercase" checked={s.UPPERCASE} onChange={(v) => upd({ UPPERCASE: v })} />
          <ColorField label="Text Color" value={s.TCOL} onChange={(v) => upd({ TCOL: v })} />
          <FormRow>
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
          </FormRow>
        </div>
      </Card>

      {/* Apps */}
      <Card title="Apps">
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <Toggle label="Time" checked={s.TIM} onChange={(v) => upd({ TIM: v })} />
          <Toggle label="Date" checked={s.DAT} onChange={(v) => upd({ DAT: v })} />
          <FormRow>
            <Toggle label="Temperature" checked={s.TEMP} onChange={(v) => upd({ TEMP: v })} />
            <ColorField label="Temp Color" value={s.TEMP_COL} onChange={(v) => upd({ TEMP_COL: v })} />
          </FormRow>
          <FormRow>
            <Toggle label="Humidity" checked={s.HUM} onChange={(v) => upd({ HUM: v })} />
            <ColorField label="Hum Color" value={s.HUM_COL} onChange={(v) => upd({ HUM_COL: v })} />
          </FormRow>
          <FormRow>
            <Toggle label="Battery" checked={s.BAT} onChange={(v) => upd({ BAT: v })} />
            <ColorField label="Battery Color" value={s.BAT_COL} onChange={(v) => upd({ BAT_COL: v })} />
          </FormRow>
          <Slider label="App Duration" min={1} max={60} value={s.ATIME} onChange={(v) => upd({ ATIME: v })} unit="s" />
          <Toggle label="Auto Transition" checked={s.ATRANS} onChange={(v) => upd({ ATRANS: v })} />
          {transitions.length > 0 && (
            <Select
              label="Transition Effect"
              value={s.TEFF}
              options={transitions.map((t, i) => ({ value: i, label: t.name }))}
              onChange={(v) => upd({ TEFF: +v })}
            />
          )}
          <Slider label="Transition Speed" min={100} max={2000} step={100} value={s.TSPEED} onChange={(v) => upd({ TSPEED: v })} unit="ms" />
          <Slider label="Scroll Speed" min={10} max={100} value={s.SSPEED} onChange={(v) => upd({ SSPEED: v })} />
          <Toggle label="Block Navigation" checked={s.BLOCKN} onChange={(v) => upd({ BLOCKN: v })} />
        </div>
      </Card>

      {/* Time & Date */}
      <Card title="Time & Date Format">
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <FormRow>
            <TextField label="Time Format" value={s.TFORMAT} onChange={(v) => upd({ TFORMAT: v })} />
            <TextField label="Date Format" value={s.DFORMAT} onChange={(v) => upd({ DFORMAT: v })} />
          </FormRow>
          <Select
            label="Time Mode"
            value={s.TMODE}
            options={[
              { value: 0, label: "24h" },
              { value: 1, label: "12h" },
            ]}
            onChange={(v) => upd({ TMODE: +v })}
          />
          <Toggle label="Start on Monday" checked={s.SOM} onChange={(v) => upd({ SOM: v })} />
          <Toggle label="Celsius" checked={s.CEL} onChange={(v) => upd({ CEL: v })} />
          <FormRow>
            <ColorField label="Time Color" value={s.TIME_COL} onChange={(v) => upd({ TIME_COL: v })} />
            <ColorField label="Date Color" value={s.DATE_COL} onChange={(v) => upd({ DATE_COL: v })} />
          </FormRow>
          <FormRow>
            <Toggle label="Show Weekday" checked={s.WD} onChange={(v) => upd({ WD: v })} />
          </FormRow>
          <FormRow>
            <ColorField label="Weekday Active" value={s.WDCA} onChange={(v) => upd({ WDCA: v })} />
            <ColorField label="Weekday Inactive" value={s.WDCI} onChange={(v) => upd({ WDCI: v })} />
          </FormRow>
          <FormRow>
            <ColorField label="Cal Header" value={s.CHCOL} onChange={(v) => upd({ CHCOL: v })} />
            <ColorField label="Cal Text" value={s.CTCOL} onChange={(v) => upd({ CTCOL: v })} />
          </FormRow>
          <ColorField label="Cal Body" value={s.CBCOL} onChange={(v) => upd({ CBCOL: v })} />
        </div>
      </Card>

      {/* Sound */}
      <Card title="Sound">
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <Toggle label="Sound Enabled" checked={s.SOUND} onChange={(v) => upd({ SOUND: v })} />
          <Slider label="Volume" min={0} max={30} value={s.VOL} onChange={(v) => upd({ VOL: v })} />
        </div>
      </Card>

      {/* Icon Picker */}
      <Card title="Icon Picker">
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <FormRow>
            <TextField label="LaMetric Icon ID" value={iconId} onChange={setIconId} placeholder="13" />
            <div class="form-group" style={{ justifyContent: "flex-end" }}>
              <div style={{ display: "flex", gap: 6 }}>
                <Button onClick={() => {
                  if (iconId) setIconPreview(`https://developer.lametric.com/content/apps/icon_thumbs/${iconId}`);
                }}>Preview</Button>
                <Button variant="primary" onClick={downloadIcon}>Download</Button>
              </div>
            </div>
          </FormRow>
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
      </Card>

      {/* Save display + actions */}
      <div style={{ display: "flex", gap: 8, flexWrap: "wrap" }}>
        <Button variant="primary" onClick={handleSaveDisplay} loading={saving}>
          Save Display Settings
        </Button>
        <Button onClick={() => {
          if (confirm("Reset all settings to defaults?")) {
            resetSettings().then(() => { toast("Settings reset"); getSettings().then(setS); });
          }
        }}>Reset Defaults</Button>
        <Button variant="danger" onClick={() => {
          if (confirm("Reboot device?")) reboot().then(() => toast("Rebooting..."));
        }}>Reboot</Button>
      </div>
    </div>
  );
}
