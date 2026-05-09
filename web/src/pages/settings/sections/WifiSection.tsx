import { useState, useEffect } from "preact/hooks";
import { scanWifi, getWifiNetworks, saveWifiNetworks } from "../../../api/client";
import type { WifiNetwork } from "../../../api/client";
import { toast } from "../../../components/Toast";
import { TextField, Card, FormRow, Button, Select } from "../../../components/ui";
import styles from "./sections.module.css";

export function WifiSection() {
  const [scannedNetworks, setScannedNetworks] = useState<Array<{ ssid: string; strength: number; security: boolean }>>([]);
  const [scanning, setScanning] = useState(false);
  const [loading, setLoading] = useState(true);
  const [networks, setNetworks] = useState<WifiNetwork[]>([
    { ssid: "", password: "" },
    { ssid: "", password: "" },
    { ssid: "", password: "" },
  ]);
  const [saving, setSaving] = useState(false);

  useEffect(() => {
    getWifiNetworks()
      .then((data) => {
        if (data.networks) {
          setNetworks(
            data.networks.map((n) => ({
              ssid: n.ssid || "",
              password: "",
              configured: n.configured,
            }))
          );
        }
      })
      .catch(() => {})
      .finally(() => setLoading(false));
  }, []);

  async function doScan() {
    setScanning(true);
    try {
      const nets = await scanWifi();
      setScannedNetworks(nets.sort((a, b) => b.strength - a.strength));
    } catch {
      toast("Scan failed");
    }
    setScanning(false);
  }

  function updateNetwork(index: number, field: "ssid" | "password", value: string) {
    const updated = [...networks];
    updated[index] = { ...updated[index], [field]: value };
    setNetworks(updated);
  }

  async function doSave() {
    setSaving(true);
    try {
      await saveWifiNetworks(networks);
      toast("WiFi networks saved. Reboot to apply.");
    } catch {
      toast("Failed to save");
    }
    setSaving(false);
  }

  const ssidOptions = scannedNetworks.map((n) => ({
    value: n.ssid,
    label: `${n.ssid} (${n.strength} dBm)${n.security ? " 🔒" : ""}`,
  }));

  return (
    <Card title="WiFi Networks" subtitle="Configure up to 3 WiFi networks. The device will try each in order.">
      <div class={styles.stack}>
        <Button onClick={doScan} disabled={scanning}>
          {scanning ? "Scanning..." : "Scan Networks"}
        </Button>

        {loading ? (
          <p class={styles.hint}>Loading...</p>
        ) : (
          networks.map((net, i) => (
            <div key={i} class={styles.wifiSlot}>
              <div class={styles.wifiSlotHeader}>
                <span>WiFi {i + 1}</span>
                {net.configured && !net.password && (
                  <span class={styles.configuredBadge}>configured</span>
                )}
              </div>
              <FormRow>
                {ssidOptions.length > 0 ? (
                  <Select
                    label="SSID"
                    value={net.ssid}
                    options={[
                      { value: "", label: "(select network)" },
                      ...(net.ssid && !ssidOptions.some((o) => o.value === net.ssid)
                        ? [{ value: net.ssid, label: `${net.ssid} (saved)` }]
                        : []),
                      ...ssidOptions,
                    ]}
                    onChange={(v) => updateNetwork(i, "ssid", v as string)}
                  />
                ) : (
                  <TextField
                    label="SSID"
                    value={net.ssid}
                    onChange={(v) => updateNetwork(i, "ssid", v)}
                    placeholder="Click 'Scan Networks' or type manually"
                  />
                )}
                <TextField
                  label="Password"
                  value={net.password || ""}
                  onChange={(v) => updateNetwork(i, "password", v)}
                  type="password"
                  placeholder={net.configured ? "(unchanged)" : undefined}
                />
              </FormRow>
            </div>
          ))
        )}

        <Button variant="primary" onClick={doSave} disabled={saving}>
          {saving ? "Saving..." : "Save WiFi Networks"}
        </Button>
      </div>
    </Card>
  );
}
