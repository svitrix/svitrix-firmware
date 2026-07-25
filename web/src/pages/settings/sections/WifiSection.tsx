import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { scanWifi, connectWifi } from "../../../api/client";
import { toast } from "../../../components/Toast";
import { TextField, Card, FormRow, Button, ConfirmDialog } from "../../../components/ui";
import styles from "./sections.module.css";

export function WifiSection({ apMode }: { apMode?: boolean }) {
  const { t } = useTranslation();
  const [networks, setNetworks] = useState<Array<{ ssid: string; rssi: number; secure: number }>>([]);
  const [scanning, setScanning] = useState(false);
  const [wifiSsid, setWifiSsid] = useState("");
  const [wifiPass, setWifiPass] = useState("");
  const [ssidError, setSsidError] = useState("");
  const [confirmConnect, setConfirmConnect] = useState(false);

  async function doScan() {
    setScanning(true);
    try {
      const nets = await scanWifi();
      setNetworks(nets.sort((a, b) => b.rssi - a.rssi));
    } catch {
      toast(t("settings.wifi.scanFailed"), { error: true });
    }
    setScanning(false);
  }

  function requestConnect() {
    if (!wifiSsid) {
      setSsidError(t("settings.wifi.enterSsid"));
      return;
    }
    setSsidError("");
    setConfirmConnect(true);
  }

  async function doConnect() {
    setConfirmConnect(false);
    try {
      await connectWifi(wifiSsid, wifiPass);
      toast(t("settings.wifi.connecting"));
      setTimeout(() => {
        toast(t("settings.wifi.rebooting"));
        fetch("/restart").catch(() => {});
      }, 3000);
    } catch {
      toast(t("settings.wifi.connectionFailed"), { error: true });
    }
  }

  const subtitle = apMode ? t("settings.wifi.apSubtitle") : undefined;

  return (
    <Card title={t("settings.wifi.title")} subtitle={subtitle}>
      <div class={styles.stack}>
        <Button onClick={doScan} disabled={scanning}>
          {scanning ? t("settings.wifi.scanning") : t("settings.wifi.scan")}
        </Button>
        {networks.length > 0 && (
          <div class={styles.networkList}>
            {networks.map((n) => (
              <div
                key={n.ssid}
                class={styles.networkItem}
                onClick={() => { setWifiSsid(n.ssid); setSsidError(""); }}
              >
                {n.ssid} ({n.rssi} dBm) {n.secure ? "\u{1f512}" : ""}
              </div>
            ))}
          </div>
        )}
        <FormRow>
          <TextField label={t("settings.wifi.ssid")} value={wifiSsid} onChange={(v) => { setWifiSsid(v); if (v) setSsidError(""); }} placeholder={t("settings.wifi.ssidPlaceholder")} error={ssidError || undefined} autocomplete="off" />
          <TextField label={t("settings.wifi.password")} value={wifiPass} onChange={setWifiPass} type="password" placeholder="••••••••" autocomplete="off" />
        </FormRow>
        <Button variant="primary" onClick={requestConnect}>{t("settings.wifi.connect")}</Button>
      </div>

      <ConfirmDialog
        open={confirmConnect}
        title={t("settings.wifi.confirmConnectTitle")}
        body={t("settings.wifi.confirmConnectBody", { ssid: wifiSsid })}
        confirmLabel={t("settings.wifi.confirmConnect")}
        onConfirm={doConnect}
        onCancel={() => setConfirmConnect(false)}
      />
    </Card>
  );
}
