# Web Interface

SVITRIX uses a modern Single Page Application (SPA) built with Preact as its web interface. The SPA is served from the device's LittleFS filesystem and communicates with the firmware via REST API.

Once SVITRIX is connected to your Wi-Fi network, access the web interface by entering the device's IP address in your browser (port 80). The IP address is displayed on the matrix at each boot.

## Pages

| Page | Route | Description |
|------|-------|-------------|
| **Screen** | `/` | Live view of the 32x8 LED matrix with app navigation (previous/next), PNG download, and GIF recording. |
| **Settings** | `/settings` | Device configuration as a master–detail screen with five category tabs — Appearance, Screens, Network, System, and Tools. Appearance and Screens auto-save; Network and Authentication save explicitly with a "Save & restart" button. Includes a dark/light theme toggle. |
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

Connect to this network and open **http://192.168.4.1** in your browser. The setup page opens with a short welcome, then a minimal WiFi form with network scanning and connection. After connecting to your home WiFi, the device reboots and the full SPA becomes available.

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

The Settings page is a **master–detail** screen. A category rail holds five tabs — **Appearance · Screens · Network · System · Tools** — and the panel beside it shows one category at a time, instead of one long scroll of cards. Most controls also carry a short one-line hint explaining what they do.

A **dark/light theme toggle** (System/Light/Dark) is available in the top-right corner of the navigation bar. Your preference is saved in the browser.

### Two ways settings save

- **Appearance & Screens settings auto-save.** They apply and save the moment you change a control — there is no per-section Save button. A small status indicator at the top shows **Saving…** and then **Saved ✓**.
- **Network & Authentication save explicitly.** These edits are held until you press **Save & restart** (marked **Requires restart**), because applying them reboots the clock. After you save — or after a Reboot or Reset — a **reconnect overlay** appears, waits for the clock to come back online, and then reloads the page.

### Offline handling

If the clock becomes unreachable, a banner appears at the top of the page and every control is disabled until it returns (with a **Retry** button), so you can't edit settings that couldn't be delivered.

### Appearance

How the clock looks — Display, Clock face, and Night Mode.

#### Display

- **Display** — turn the whole LED display on or off
- **Auto Brightness** — automatically adjust brightness based on ambient light
- **Brightness** — manual brightness level (5–255)
- **Color depth** — mid-tone richness of the gamma curve (0.5–3.0)
- **Uppercase** — force all text to uppercase
- **Text Color** — default text color for all apps
- **Background Effect** — an animated effect drawn behind the apps (None, or one of the built-in effects)
- **White balance** — advanced LED color tuning:
  - **Tint** — fine-tunes the display's overall color cast
  - **Warmth** — shifts the whole display warmer or cooler

#### Clock face

- **Time Format / Date Format** — strftime format strings (e.g. `%H:%M`, `%d.%m.%y`). One-tap **preset chips** below each field render the actual result — e.g. `13:45`, `1:45 PM`, `13:45:30`, `Mon 05`, `05 Jul`, `2026-07-05` — and a live **"Right now it shows: …"** line previews your current format. You can still type a custom format.
- **Time Mode** — display style: Plain Text, Calendar, Calendar Top, Calendar Alt, Calendar Alt Top, Big Digits, or Binary
- **Start on Monday** — week starts on Monday instead of Sunday
- **Celsius** — show temperature in °C (off = °F)
- **Time / Date Color** — individual colors for the time and date
- **Calendar colors** (Header / Text / Body) — appear only in the calendar time modes
- **Show Weekday** — show the weekday indicator row
- **Weekday colors** (Active / Inactive) — appear only when the weekday row is enabled

#### Night Mode

Schedule a low-brightness, single-color mode for nighttime use (e.g. a bedroom clock):

- **Enable Night Mode** — toggle the feature on/off
- **Start / End** — time range (e.g. 21:00 to 06:00, supports crossing midnight)
- **Night Brightness** — display brightness during night hours (1–50)
- **Night Color** — text color during night mode (default: red — easier on the eyes)
- **Freeze on one app at night** — stop auto-cycling screens during night mode; use the buttons to navigate

During the scheduled window the display dims to the configured brightness and all text renders in the chosen night color. When the window ends, normal settings are restored automatically.

### Screens

Which built-in screens rotate on the display, and how they cycle. Toggle each screen on or off; temperature, humidity, and battery each have their own color picker.

- **Time**, **Date**, **Temperature** (with color), **Humidity** (with color), **Battery** (with color)
- **App Duration** — how long each screen shows before switching (1–60s)
- **Auto Transition** — automatically cycle through screens
- **Transition Effect** — visual effect when switching screens (None, Slide, Dim, Zoom, etc.)
- **Transition Speed** — how fast the transition animation plays (100–2000ms)
- **Scroll Speed** — how fast long text scrolls
- **Lock button navigation** — stop the side buttons switching screens

### Network

Connectivity and time sync. Everything in this category is applied with the explicit **Save & restart** button at the bottom.

#### WiFi

Scan for available networks, select one, and enter the password to connect. Connecting warns you that the page will lose contact — the clock shows its new IP on the display, so reconnect there.

#### MQTT

Connect to an MQTT broker for Home Assistant integration and remote control:
- **Broker** — hostname or IP of your MQTT broker
- **Port** — default 1883
- **Username / Password** — broker credentials
- **Prefix** — MQTT topic prefix (default: device ID)
- **Home Assistant Discovery** — enable auto-discovery of device entities in HA

#### NTP & Timezone

- **NTP Server** — time server (default: `pool.ntp.org`)
- **Timezone** — POSIX timezone string (find yours at [posix_tz_db](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv))

#### Network (Static IP)

Enable **Static IP** to configure a fixed IP address, gateway, subnet, and DNS server instead of DHCP.

### System

Access, sound, and recovery — Authentication, Sound, and device Actions.

#### Authentication

Set a username and password for the web interface — see the [Authentication](#authentication) section below. Because credentials are part of the device configuration, they take effect through the same **Save & restart** flow as the Network settings.

#### Sound

- **Sound Enabled** — enable/disable the buzzer
- **Volume** — buzzer volume level (0–30)

#### Actions

- **Reset Defaults** — restore all settings to factory defaults (requires confirmation)
- **Reboot** — restart the device (requires confirmation)

Both actions trigger the **reconnect overlay**, which waits for the clock to come back online and then reloads the page.

### Tools

One-off actions you send to the clock — nothing here is a saved setting.

#### Send Notification

Send a one-time message to the display:
- **Text** — message to show (required)
- **Icon** — icon ID or filename from `/ICONS/`
- **Icon Layout** — position the icon on the left, right, or hide it
- **Duration** — how long the notification shows (1–60s)
- **Rainbow** — cycle text through rainbow colors
- **Color** — text color (when rainbow is off)
- **Sound** — play a sound file from `/MELODIES/`
- **RTTTL** — play a melody in [RTTTL format](https://en.wikipedia.org/wiki/Ring_Tone_Text_Transfer_Language)

#### Icon Picker

Download icons from the [LaMetric icon library](https://developer.lametric.com/icons):
1. Enter the icon ID number
2. Click **Preview** to see it
3. Click **Download** to save it to the device's `/ICONS/` folder

## Authentication

You can set a username and password in Settings → Authentication. When configured, every page, API call, and the SVITRIX app will require these credentials. Leave both fields empty to disable authentication.

::: warning
Do not lose your auth credentials — otherwise you will need to factory reset the device.
:::
