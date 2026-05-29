import { useState, useEffect } from "preact/hooks";
import { scanWifi, getWifiNetworks, saveWifiNetworks, reboot } from "../../../api/client";
import type { WifiNetwork } from "../../../api/client";
import { toast } from "../../../components/Toast";
import { TextField, Card, FormRow, Button, Select } from "../../../components/ui";
import { useT } from "../../../i18n";
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
  const t = useT();

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
      toast(t.settings.wifiScanFailed);
    }
    setScanning(false);
  }

  function updateNetwork(index: number, field: "ssid" | "password", value: string) {
    const updated = [...networks];
    updated[index] = { ...updated[index], [field]: value };
    setNetworks(updated);
  }

  async function doSave(andReboot = false) {
    setSaving(true);
    try {
      await saveWifiNetworks(networks);
      if (andReboot) {
        toast(t.settings.wifiSavedReboot);
        try {
          await reboot();
        } catch {
          toast(t.system.rebooting);
        }
      } else {
        toast(t.settings.wifiSavedNoReboot);
      }
    } catch {
      toast(t.settings.wifiSaveFailed);
    }
    setSaving(false);
  }

  const ssidOptions = scannedNetworks.map((n) => ({
    value: n.ssid,
    label: `${n.ssid} (${n.strength} dBm)${n.security ? " 🔒" : ""}`,
  }));

  return (
    <Card title={t.settings.wifiTitle} subtitle={t.settings.wifiSubtitle}>
      <div class={styles.stack}>
        <Button onClick={doScan} disabled={scanning}>
          {scanning ? t.settings.wifiScanning : t.settings.wifiScan}
        </Button>

        {loading ? (
          <p class={styles.hint}>{t.loading}</p>
        ) : (
          networks.map((net, i) => (
            <div key={i} class={styles.wifiSlot}>
              <div class={styles.wifiSlotHeader}>
                <span>WiFi {i + 1}</span>
                {net.configured && !net.password && (
                  <span class={styles.configuredBadge}>{t.settings.wifiConfigured}</span>
                )}
              </div>
              <FormRow>
                {ssidOptions.length > 0 ? (
                  <Select
                    label="SSID"
                    value={net.ssid}
                    options={[
                      { value: "", label: t.settings.wifiSelectNetwork },
                      ...(net.ssid && !ssidOptions.some((o) => o.value === net.ssid)
                        ? [{ value: net.ssid, label: `${net.ssid} ${t.settings.wifiSaved}` }]
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
                    placeholder={t.settings.wifiScanOrType}
                  />
                )}
                <TextField
                  label={t.settings.wifiPassword}
                  value={net.password || ""}
                  onChange={(v) => updateNetwork(i, "password", v)}
                  type="password"
                  placeholder={net.configured ? t.settings.wifiUnchanged : undefined}
                />
              </FormRow>
            </div>
          ))
        )}

        <div class={styles.buttonRow}>
          <Button onClick={() => doSave(false)} disabled={saving}>
            {saving ? t.settings.wifiSaving : t.settings.wifiSave}
          </Button>
          <Button variant="primary" onClick={() => doSave(true)} disabled={saving}>
            {saving ? t.settings.wifiSaving : t.settings.wifiSaveReboot}
          </Button>
          <Button onClick={async () => {
            toast(t.system.rebooting);
            try {
              await reboot();
            } catch {
              // ignore
            }
          }}>
            {t.system.reboot}
          </Button>
        </div>
      </div>
    </Card>
  );
}
