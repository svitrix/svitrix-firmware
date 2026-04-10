import { useState, useEffect } from "preact/hooks";
import {
  getSettings,
  saveSettings,
  getStats,
  getTransitions,
  reboot,
  resetSettings,
} from "../api/client";
import type { Settings, Stats, TransitionInfo } from "../api/types";
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

export function SettingsPage(_props: { path?: string }) {
  const [s, setS] = useState<Settings | null>(null);
  const [stats, setStats] = useState<Stats | null>(null);
  const [transitions, setTransitions] = useState<TransitionInfo[]>([]);
  const [saving, setSaving] = useState(false);

  useEffect(() => {
    getSettings().then(setS);
    getStats().then(setStats);
    getTransitions().then(setTransitions);
  }, []);

  if (!s) return <p>Loading...</p>;

  function upd(patch: Partial<Settings>) {
    setS((prev) => (prev ? { ...prev, ...patch } : prev));
  }

  async function handleSave() {
    setSaving(true);
    try {
      await saveSettings(s!);
      toast("Saved!");
    } catch {
      toast("Error saving");
    }
    setSaving(false);
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

      {/* Display */}
      <div class="card">
        <h3 style={{ marginBottom: 12 }}>Display</h3>
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <Toggle label="Matrix Power" checked={s.MATP} onChange={(v) => upd({ MATP: v })} />
          <Toggle
            label="Auto Brightness"
            checked={s.ABRI}
            onChange={(v) => upd({ ABRI: v })}
          />
          <div class="form-group">
            <label>
              Brightness: {s.BRI}
              <input
                type="range"
                min={0}
                max={255}
                value={s.BRI}
                onInput={(e) => upd({ BRI: +(e.target as HTMLInputElement).value })}
              />
            </label>
          </div>
          <div class="form-group">
            <label>
              Gamma: {s.GAMMA}
              <input
                type="range"
                min={0.5}
                max={3}
                step={0.1}
                value={s.GAMMA}
                onInput={(e) => upd({ GAMMA: +(e.target as HTMLInputElement).value })}
              />
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
              <input
                type="range"
                min={1}
                max={60}
                value={s.ATIME}
                onInput={(e) => upd({ ATIME: +(e.target as HTMLInputElement).value })}
              />
            </label>
          </div>
          <Toggle
            label="Auto Transition"
            checked={s.ATRANS}
            onChange={(v) => upd({ ATRANS: v })}
          />
          {transitions.length > 0 && (
            <div class="form-group">
              <label>Transition Effect</label>
              <select
                value={s.TEFF}
                onChange={(e) => upd({ TEFF: +(e.target as HTMLSelectElement).value })}
              >
                {transitions.map((t, i) => (
                  <option key={i} value={i}>
                    {t.name}
                  </option>
                ))}
              </select>
            </div>
          )}
          <div class="form-group">
            <label>
              Transition Speed: {s.TSPEED}ms
              <input
                type="range"
                min={100}
                max={2000}
                step={100}
                value={s.TSPEED}
                onInput={(e) => upd({ TSPEED: +(e.target as HTMLInputElement).value })}
              />
            </label>
          </div>
          <div class="form-group">
            <label>
              Scroll Speed: {s.SSPEED}
              <input
                type="range"
                min={10}
                max={100}
                value={s.SSPEED}
                onInput={(e) => upd({ SSPEED: +(e.target as HTMLInputElement).value })}
              />
            </label>
          </div>
          <Toggle
            label="Block Navigation"
            checked={s.BLOCKN}
            onChange={(v) => upd({ BLOCKN: v })}
          />
        </div>
      </div>

      {/* Time & Date */}
      <div class="card">
        <h3 style={{ marginBottom: 12 }}>Time & Date</h3>
        <div style={{ display: "flex", flexDirection: "column", gap: 10 }}>
          <div class="form-row">
            <div class="form-group">
              <label>Time Format</label>
              <input
                type="text"
                value={s.TFORMAT}
                onInput={(e) => upd({ TFORMAT: (e.target as HTMLInputElement).value })}
              />
            </div>
            <div class="form-group">
              <label>Date Format</label>
              <input
                type="text"
                value={s.DFORMAT}
                onInput={(e) => upd({ DFORMAT: (e.target as HTMLInputElement).value })}
              />
            </div>
          </div>
          <div class="form-group">
            <label>Time Mode</label>
            <select
              value={s.TMODE}
              onChange={(e) => upd({ TMODE: +(e.target as HTMLSelectElement).value })}
            >
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
          <ColorField
            label="Weekday Active"
            value={s.WDCA}
            onChange={(v) => upd({ WDCA: v })}
          />
          <ColorField
            label="Weekday Inactive"
            value={s.WDCI}
            onChange={(v) => upd({ WDCI: v })}
          />
          <ColorField label="Cal Header" value={s.CHCOL} onChange={(v) => upd({ CHCOL: v })} />
          <ColorField label="Cal Text" value={s.CTCOL} onChange={(v) => upd({ CTCOL: v })} />
          <ColorField label="Cal Body" value={s.CBCOL} onChange={(v) => upd({ CBCOL: v })} />
        </div>
        <div class="form-row" style={{ marginTop: 12 }}>
          <div class="form-group">
            <label>Color Correction</label>
            <input
              type="color"
              value={s.CCORRECTION}
              onInput={(e) => upd({ CCORRECTION: (e.target as HTMLInputElement).value })}
            />
          </div>
          <div class="form-group">
            <label>Color Temperature</label>
            <input
              type="color"
              value={s.CTEMP}
              onInput={(e) => upd({ CTEMP: (e.target as HTMLInputElement).value })}
            />
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
            <input
              type="range"
              min={0}
              max={30}
              value={s.VOL}
              onInput={(e) => upd({ VOL: +(e.target as HTMLInputElement).value })}
            />
          </label>
        </div>
      </div>

      {/* Actions */}
      <div style={{ display: "flex", gap: 8, flexWrap: "wrap" }}>
        <button class="btn-primary" onClick={handleSave} disabled={saving}>
          {saving ? "Saving..." : "Save Settings"}
        </button>
        <button
          onClick={() => {
            if (confirm("Reset all settings to defaults?")) {
              resetSettings().then(() => {
                toast("Settings reset");
                getSettings().then(setS);
              });
            }
          }}
        >
          Reset Defaults
        </button>
        <button
          class="btn-danger"
          onClick={() => {
            if (confirm("Reboot device?")) reboot().then(() => toast("Rebooting..."));
          }}
        >
          Reboot
        </button>
      </div>
    </div>
  );
}
