import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { scanWifi, connectWifi } from "../../../api/client";
import { toast } from "../../../components/Toast";
import { TextField, Card, FormRow, Button } from "../../../components/ui";
import styles from "./sections.module.css";

export function WifiSection({ apMode }: { apMode?: boolean }) {
  const { t } = useTranslation();
  const [networks, setNetworks] = useState<Array<{ ssid: string; rssi: number; secure: number }>>([]);
  const [scanning, setScanning] = useState(false);
  const [wifiSsid, setWifiSsid] = useState("");
  const [wifiPass, setWifiPass] = useState("");

  async function doScan() {
    setScanning(true);
    try {
      const nets = await scanWifi();
      setNetworks(nets.sort((a, b) => b.rssi - a.rssi));
    } catch {
      toast(t("settings.wifi.scanFailed"));
    }
    setScanning(false);
  }

  async function doConnect() {
    if (!wifiSsid) { toast(t("settings.wifi.enterSsid")); return; }
    try {
      await connectWifi(wifiSsid, wifiPass);
      toast(t("settings.wifi.connecting"));
      setTimeout(() => {
        toast(t("settings.wifi.rebooting"));
        fetch("/restart").catch(() => {});
      }, 3000);
    } catch {
      toast(t("settings.wifi.connectionFailed"));
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
                onClick={() => setWifiSsid(n.ssid)}
              >
                {n.ssid} ({n.rssi} dBm) {n.secure ? "\u{1f512}" : ""}
              </div>
            ))}
          </div>
        )}
        <FormRow>
          <TextField label={t("settings.wifi.ssid")} value={wifiSsid} onChange={setWifiSsid} placeholder={t("settings.wifi.ssidPlaceholder")} />
          <TextField label={t("settings.wifi.password")} value={wifiPass} onChange={setWifiPass} type="password" placeholder="••••••••" />
        </FormRow>
        <Button variant="primary" onClick={doConnect}>{t("settings.wifi.connect")}</Button>
      </div>
    </Card>
  );
}
