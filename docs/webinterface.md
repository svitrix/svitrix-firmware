# Web Interface

SVITRIX uses a modern Single Page Application (SPA) built with Preact as its web interface. The SPA is served from the device's LittleFS filesystem and communicates with the firmware via REST API.

Once SVITRIX is connected to your Wi-Fi network, access the web interface by entering the device's IP address in your browser (port 80). The IP address is displayed on the matrix at each boot.

## Pages

| Page | Route | Description |
|------|-------|-------------|
| **Screen** | `/` | Live view of the 32x8 LED matrix with app navigation (previous/next), PNG download, and GIF recording. |
| **Settings** | `/settings` | Device configuration split into independent sections, each with its own Save button: WiFi, Network, MQTT, NTP/Timezone, Authentication, Display, Apps, Time & Date, Sound, Send Notification, and Icon Picker. Includes dark/light theme toggle. |
| **Data Fetcher** | `/datafetcher` | Configure external HTTP data sources that automatically fetch and display data on the matrix. See [Data Fetcher](./datafetcher) for details. |
| **Files** | `/files` | Built-in file manager to browse, upload, download, edit, and delete files on the device (icons, melodies, custom apps, palettes). |
| **Backup** | `/backup` | Download all device files as a JSON backup, or restore from a previously downloaded backup. |
| **Update** | `/update` | Upload firmware (.bin) for OTA update. The device reboots automatically after a successful upload. |

## First-Time Setup (AP Mode)

When SVITRIX cannot connect to a saved WiFi network, it creates its own access point:

| Parameter | Value |
|-----------|-------|
| Network name | `svitrix_XXXXX` |
| Password | `12345678` |

Connect to this network and open **http://192.168.4.1** in your browser. A minimal WiFi setup page will appear with network scanning and connection. After connecting to your home WiFi, the device reboots and the full SPA becomes available.

## SPA Deployment

The web interface is stored separately from the firmware in the LittleFS filesystem partition. After flashing the firmware, upload the SPA files to the device:

```bash
cd web && npm run upload
```

This builds the SPA and uploads it to the device's LittleFS root directory. The SPA bundle is approximately 18 KB (gzip compressed) and includes all 6 pages.

::: tip
Once uploaded, the SPA persists across firmware updates. You only need to re-upload the SPA when the web interface itself is updated.
:::

## Authentication

You can set a username and password in Settings → Authentication. When configured, every page, API call, and the SVITRIX app will require these credentials. Leave both fields empty to disable authentication.

::: warning
Do not lose your auth credentials — otherwise you will need to factory reset the device.
:::
