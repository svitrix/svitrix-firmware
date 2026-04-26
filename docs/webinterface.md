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

## Settings Guide

The Settings page is organized into independent sections. Each section has its own **Save** button — you only save what you changed.

A **dark/light theme toggle** (☀/☽) is available in the top-right corner of the navigation bar. Your preference is saved in the browser.

### Stats Bar

A read-only bar at the top showing real-time device info: firmware version, free RAM, WiFi signal strength, ambient light (lux), uptime, temperature, humidity, and current brightness.

### WiFi

Scan for available networks, select one, and enter the password to connect. The device reboots after connecting.

### Network

Enable **Static IP** to configure a fixed IP address, gateway, subnet, and DNS server instead of DHCP.

### MQTT

Connect to an MQTT broker for Home Assistant integration and remote control:
- **Broker** — hostname or IP of your MQTT broker
- **Port** — default 1883
- **Username / Password** — broker credentials
- **Prefix** — MQTT topic prefix (default: device ID)
- **Home Assistant Discovery** — enable auto-discovery of device entities in HA

### NTP & Timezone

- **NTP Server** — time server (default: `pool.ntp.org`)
- **Timezone** — POSIX timezone string (find yours at [posix_tz_db](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv))

### Authentication

See the [Authentication](#authentication) section below.

### Night Mode

Schedule a low-brightness, single-color mode for nighttime use (e.g. bedroom clock):

- **Enable Night Mode** — toggle the feature on/off
- **Start / End** — time range (e.g. 21:00 to 06:00, supports crossing midnight)
- **Night Brightness** — display brightness during night hours (1–50)
- **Night Color** — text color during night mode (default: red — easier on the eyes)
- **Block Auto-Transition** — when checked, apps won't cycle automatically; use buttons to navigate

During the scheduled time, the display dims to the configured brightness and all text renders in the chosen night color. When the time window ends, normal settings are restored automatically.

### Display

- **Matrix Power** — turn the LED matrix on/off
- **Auto Brightness** — automatically adjust brightness based on ambient light
- **Brightness** — manual brightness level (0–255)
- **Gamma** — gamma correction curve (0.5–3.0)
- **Uppercase** — force all text to uppercase
- **Text Color** — default text color for all apps
- **Color Correction / Color Temperature** — advanced LED color tuning

### Apps

Toggle built-in apps on/off, each with its own color picker:
- **Time**, **Date**, **Temperature** (with color), **Humidity** (with color), **Battery** (with color)

App behavior:
- **App Duration** — how long each app shows before switching (1–60s)
- **Auto Transition** — automatically cycle through apps
- **Transition Effect** — visual effect when switching apps (None, Slide, Dim, Zoom, etc.)
- **Transition Speed** — how fast the transition animation plays (100–2000ms)
- **Scroll Speed** — text scroll speed for long text
- **Block Navigation** — disable button navigation between apps

### Time & Date

- **Time Format / Date Format** — strftime format strings (e.g., `%H:%M`, `%d.%m.%y`)
- **Time Mode** — display style: Plain Text, Calendar, Calendar Top, Calendar Alt, Big Digits, or Binary
- **Start on Monday** — week starts on Monday instead of Sunday
- **Celsius** — show temperature in °C (off = °F)
- **Time / Date Color** — individual colors for time and date apps
- **Show Weekday** — show weekday indicator bar
- **Weekday Active / Inactive Color** — colors for weekday dots
- **Calendar Header / Text / Body Color** — colors for the calendar box

### Sound

- **Sound Enabled** — enable/disable the buzzer
- **Volume** — buzzer volume level (0–30)

### Send Notification

Send a one-time message to the display:
- **Text** — message to show (required)
- **Icon** — icon ID or filename from `/ICONS/`
- **Icon Layout** — position the icon on the left, right, or hide it
- **Duration** — how long the notification shows (1–60s)
- **Rainbow** — cycle text through rainbow colors
- **Color** — text color (when rainbow is off)
- **Sound** — play a sound file from `/MELODIES/`
- **RTTTL** — play a melody in [RTTTL format](https://en.wikipedia.org/wiki/Ring_Tone_Text_Transfer_Language)

### Icon Picker

Download icons from the [LaMetric icon library](https://developer.lametric.com/icons):
1. Enter the icon ID number
2. Click **Preview** to see it
3. Click **Download** to save it to the device's `/ICONS/` folder

### Actions

- **Save Display Settings** — saves all display-related settings at once (fallback if individual Save buttons were skipped)
- **Reset Defaults** — restore all settings to factory defaults (requires confirmation)
- **Reboot** — restart the device (requires confirmation)

## Authentication

You can set a username and password in Settings → Authentication. When configured, every page, API call, and the SVITRIX app will require these credentials. Leave both fields empty to disable authentication.

::: warning
Do not lose your auth credentials — otherwise you will need to factory reset the device.
:::
