
# Quick Start

Setting up SVITRIX-XE1E takes just a few minutes. Follow the steps below to go from unboxing to a working smart clock.

## Step 1. Flash the firmware

Connect your Ulanzi TC001 to a computer via USB cable and open the [online flasher](./flasher).

::: tip
If flashing doesn't start — try a different USB cable or port. Some cables only support charging, not data transfer.
:::

## Step 2. Connect to WiFi

After flashing, the device will create a WiFi access point:

| Parameter | Value |
|-----------|-------|
| Network name | `svitrix_XXXXX` |
| Password | `12345678` |

Connect to this network from your phone or computer.

## Step 3. Configure your network

Open a browser and navigate to:

**http://192.168.4.1**

A WiFi setup page will appear. Scan for available networks, select yours, enter the password, and click Connect. The device will reboot and connect to your home WiFi.

## Step 4. Find the device on your network

Once connected to WiFi, the device will show:
1. The firmware version (e.g., `0.4.0`) for a few seconds
2. A scrolling banner with "SVITRIX XE1E", the IP address, and the mDNS hostname

Enter the IP address in your browser to open the [web interface](./webinterface).

::: tip
You can also access the device using `http://svitrix_XXXXX.local` (where XXXXX is your device ID).
If you missed the IP address on screen — look for the device in your router's connected devices list.
:::

## Step 5. Upload the Web UI (developers only)

If you're building from source, upload the SPA web interface to the device:

```bash
cd web && npm run upload
```

::: tip
Pre-built firmware releases include this step in the online flasher. You only need this if you're building from source.
:::

## Step 6. Set up Weather (optional)

SVITRIX-XE1E includes native weather apps that show outdoor temperature, humidity, pressure, and air quality. To enable them:

1. Sign up for a free account at [WeatherAPI.com](https://www.weatherapi.com)
2. Copy your API key from the dashboard
3. In the web interface, go to **Settings → Weather**
4. Paste your API key
5. Choose your location method:
   - **City Name** — e.g., "Mexico City"
   - **Coordinates** — latitude and longitude
   - **Auto (IP)** — automatic detection based on your IP
   - **Station ID** — for personal weather stations
6. Enable the weather apps you want (Outdoor Temp, Outdoor Humidity, Pressure, Air Quality)
7. Click **Save Weather**

::: tip
The free WeatherAPI.com plan allows 1 million calls per month — more than enough for a 10-minute update interval.
:::

## Step 7. Set up MQTT (optional)

In the web interface, configure the connection to your MQTT broker. This enables control from HomeAssistant, IOBroker, NodeRed, and other smart home systems.

For details on available commands, see the [API](./api) section.

## Done!

SVITRIX-XE1E is already running with pre-installed apps: time, date, temperature, humidity, battery, and weather. To create your own custom apps, check the [Apps](./apps) section.
