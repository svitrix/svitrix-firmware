# Web Interface

SVITRIX uses a modern Single Page Application (SPA) built with Preact as its web interface. The SPA is served from the device's LittleFS filesystem and communicates with the firmware via REST API.

Once SVITRIX is connected to your Wi-Fi network, access the web interface by entering the device's IP address in your browser (port 80). The IP address is displayed on the matrix at each boot.

## Language

The interface supports **Spanish** (default) and **English**. Use the **ES/EN** toggle in the navigation bar to switch languages. Your preference is saved in the browser.

## Tabs

The interface is organized into focused tabs, each with its own **Save** button that only saves settings for that section.

| Tab | Description |
|-----|-------------|
| **Ajustes / Settings** | WiFi networks, static IP, NTP server, timezone, and Weather API configuration. |
| **MQTT** | MQTT broker connection and HTTP authentication settings. |
| **Pantalla / Display** | Matrix power, brightness, gamma, text color, background effects, night mode, and send notifications. |
| **Apps** | Native apps (time, date, temperature, humidity, battery), weather apps, transitions, and navigation settings. |
| **Fecha/Hora / Time/Date** | Time and date formats, display modes (calendar, big digits, binary), colors. |
| **Sonido / Sound** | Buzzer enable/disable and volume. |
| **Vista / Screen** | Live view of the 32x8 LED matrix with app navigation and PNG download. |
| **Datos / Data** | Configure external HTTP data sources. See [Data Fetcher](./datafetcher). |
| **Alarmas / Alarms** | Configure wake-up alarms with day selection and snooze. |
| **Archivos / Files** | File manager to browse, upload, edit, and delete files on the device. |
| **Iconos / Icons** | Download icons from the LaMetric icon library. |
| **Respaldo / Backup** | Download/restore device backup as JSON. |
| **Sistema / System** | Save all settings, reset to defaults, erase WiFi, or reboot. |
| **Actualizar / Update** | Upload firmware (.bin) for OTA update. |

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

This builds the SPA and uploads it to the device's LittleFS root directory. The SPA bundle is approximately 30 KB (gzip compressed).

::: tip
Once uploaded, the SPA persists across firmware updates. You only need to re-upload the SPA when the web interface itself is updated.
:::

## Tab Details

### Ajustes / Settings

**Stats Bar** — Read-only bar at the top showing: firmware version, free RAM, WiFi signal, ambient light (lux), uptime, temperature, humidity, and brightness.

**WiFi** — Configure up to 3 WiFi networks. The device tries each in order. Scan for networks or enter manually.

**Network** — Enable **Static IP** to configure fixed IP, gateway, subnet, and DNS.

**NTP & Timezone** — Select NTP server and configure timezone using POSIX format. Find yours at [posix_tz_db](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv).

**Weather API** — Configure weatherapi.com API key, location method (city, coordinates, auto-IP, or station ID), and update interval. Click **Fetch Data** to test the configuration.

### MQTT

**MQTT Settings:**
- **Broker** — hostname or IP of your MQTT broker
- **Port** — default 1883
- **Username / Password** — broker credentials
- **Prefix** — MQTT topic prefix (default: device ID)
- **HA Discovery** — enable Home Assistant auto-discovery

**Authentication** — Set web username/password. Leave empty to disable. All pages and API calls require credentials when set.

::: warning
Do not lose your auth credentials — otherwise you will need to factory reset the device.
:::

### Pantalla / Display

**Display Settings:**
- **Matrix Power** — turn the LED matrix on/off
- **Auto Brightness** — adjust brightness based on ambient light (with min/max range)
- **Brightness** — manual brightness level (0–255)
- **Gamma** — gamma correction (0.5–3.0)
- **Uppercase** — force all text to uppercase
- **Text Color** — default text color
- **Background Effect** — visual effect behind apps
- **Color Correction / Temperature** — advanced LED tuning

**Night Mode** — Schedule low-brightness mode for nighttime:
- **Enable** — toggle on/off
- **Start / End** — time range (supports crossing midnight)
- **Night Brightness** — brightness during night (1–50)
- **Night Color** — text color (default: red)
- **Block Auto-Transition** — stop app cycling during night

**Send Notification** — Send a one-time message:
- **Text** — message (required)
- **Icon** — icon ID or filename
- **Layout** — icon position (left/right)
- **Hold** — keep until dismissed
- **Duration** — display time (1–60s)
- **Rainbow** — rainbow text effect
- **Color** — text color
- **Sound / RTTTL** — play melody

### Apps

**App Order** — A drag-and-drop list at the top of the Apps page controls the order in which **all** apps in the loop are shown: the **native** ones (the originals — Time, Date, Temperature, Humidity, Battery — plus the **weather** apps) and your **custom** apps. Drag any row to reorder; the change is saved instantly and **persists across reboots**.

**Native Apps** — Toggle and configure:
- **Time** — digital clock
- **Date** — date display
- **Temperature** — internal sensor (Celsius/Fahrenheit, offset, color)
- **Humidity** — internal sensor (color)
- **Battery** — battery level (color)

**Weather Apps** — Toggle outdoor data from Weather API:
- Outdoor Temperature, Humidity, Pressure, Air Quality, UV Index
- **Auto Color** — color changes based on level (green → yellow → orange → red)

**Transitions & Navigation:**
- **Alarms Indicator** — LED in corner when alarms active
- **Auto Transition** — cycle through apps automatically
- **Transition Effect** — animation style (slide, dim, zoom, etc.)
- **Transition Speed** — animation speed (100–2000ms)
- **Scroll Speed** — text scroll speed
- **Block Navigation** — disable button navigation

### Fecha/Hora / Time/Date

- **Time Format / Date Format** — strftime format strings
- **Time Mode** — Plain Text, Calendar, Calendar Alt, Big Digits, Binary
- **Start on Monday** — week starts Monday
- **Show Weekday** — weekday indicator bar
- **Time / Date Color** — individual colors
- **Weekday Active / Inactive** — weekday dot colors
- **Calendar Header / Text / Body** — calendar box colors

### Sonido / Sound

- **Sound Enabled** — enable/disable buzzer
- **Volume** — buzzer volume (0–30)

### Vista / Screen

Live preview of the 32x8 LED matrix. Use **Prev/Next** buttons to navigate apps. **Download PNG** saves the current frame.

### Datos / Data

Configure external HTTP data sources. See [Data Fetcher](./datafetcher) for details.

### Alarmas / Alarms

Configure wake-up alarms:
- Set time and select days
- Add optional label
- Snooze (5 min) or dismiss when ringing

### Archivos / Files

Built-in file manager:
- Browse directories
- Upload files
- Edit text files inline
- Delete files/folders
- Create new directories

### Iconos / Icons

Download icons from [LaMetric icon library](https://developer.lametric.com/icons):
1. Enter the icon ID number
2. Click **Preview** to see it
3. Click **Download** to save to `/ICONS/`

### Respaldo / Backup

- **Download Backup** — saves all files and settings as JSON
- **Restore** — upload a backup file to restore (device reboots)

### Sistema / System

- **Save All Settings** — saves display, MQTT, and weather config at once
- **Reset Defaults** — restore factory settings (confirmation required)
- **Erase WiFi** — clear all WiFi credentials (device enters AP mode)
- **Reboot** — restart device (confirmation required)

### Actualizar / Update

Upload firmware (.bin or .bin.gz) for OTA update. The device reboots automatically after successful upload.
