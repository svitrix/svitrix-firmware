import { useState } from "preact/hooks";
import { scanWifi, connectWifi } from "../../api/client";
import { toast } from "../../components/Toast";
import { TextField, Card, FormRow, Button } from "../../components/ui";
import styles from "./sections.module.css";

export function WifiSection({ apMode }: { apMode?: boolean }) {
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
      toast("Scan failed");
    }
    setScanning(false);
  }

  async function doConnect() {
    if (!wifiSsid) { toast("Enter SSID"); return; }
    try {
      await connectWifi(wifiSsid, wifiPass);
      toast("Connecting to WiFi...");
      setTimeout(() => {
        toast("Device rebooting... check matrix for new IP");
        fetch("/restart").catch(() => {});
      }, 3000);
    } catch {
      toast("Connection failed");
    }
  }

  const subtitle = apMode
    ? "Connect to your home WiFi network. After connecting, the device will reboot with full settings available."
    : undefined;

  return (
    <Card title="WiFi" subtitle={subtitle}>
      <div class={styles.stack}>
        <Button onClick={doScan} disabled={scanning}>
          {scanning ? "Scanning..." : "Scan Networks"}
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
          <TextField label="SSID" value={wifiSsid} onChange={setWifiSsid} />
          <TextField label="Password" value={wifiPass} onChange={setWifiPass} type="password" />
        </FormRow>
        <Button variant="primary" onClick={doConnect}>Connect</Button>
      </div>
    </Card>
  );
}
