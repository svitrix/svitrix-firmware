
# Quick Start

Setting up SVITRIX takes just a few minutes. Follow the steps below to go from unboxing to a working smart clock.

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

Once connected to WiFi, the device's IP address will appear on the matrix display. Enter this address in your browser to open the [web interface](./webinterface).

::: tip
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

## Step 6. Set up MQTT

In the web interface, configure the connection to your MQTT broker. This enables control from HomeAssistant, IOBroker, NodeRed, and other smart home systems.

For details on available commands, see the [API](./api) section.

## Done!

SVITRIX is already running with pre-installed apps: time, date, temperature, and humidity. To create your own custom apps, check the [Apps](./apps) section.
