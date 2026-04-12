# Dev Features

This section contains small setting options that the majority of users do not change or change very rarely and therefore saved the effort of creating an elaborate settings interface.

All features are only applied at boot. So you have to restart svitrix after modifying.

Create a `dev.json` in your filemanager.

## JSON Properties

The JSON object has the following properties:

| Key | Type | Description | Default |
| --- | ---- | ----------- | ------- |
| `hostname` | string | Changes the hostname of your svitrix. This is used for App discovery, mDNS etc. | uniqeID |
| `ap_timeout` | integer | The timeout in seconds before SVITRIX switches to AP mode if the saved WLAN was not found. | 15 |
| `bootsound` | string | Uses a custom melodie while booting |  |
| `matrix` | integer | Changes the matrix layout (0,1 or 2) | `0` |
| `color_correction` | array of int | Sets the colorcorrection of the matrix | `[255,255,255]` |
| `color_temperature` | array of int | Sets the colortemperature of the matrix | `[255,255,255]` |
| `rotate_screen` | boolean | Rotates the screen upside down | `false` |
| `mirror_screen` | boolean | Mirrors the screen | `false` |
| `temp_dec_places` | integer | Number of decimal places for temperature measurements | `0` |
| `sensor_reading` | boolean | Enables or disables the reading of the Temp&Hum sensor | `true` |
| `temp_offset` | float | Sets the offset for the internal temperature measurement | `-9` |
| `hum_offset` | float | Sets the offset for the internal humidity measurement | `0` |
| `min_brightness` | integer | Sets minimum brightness level for the Autobrightness control | `2` |
| `max_brightness` | integer | Sets maximum brightness level for the Autobrightness control. On high levels, this could result in overheating! | `180` |
| `ldr_gamma` | float | Allows to set the gammacorrection of the brightness control | 3.0 |
| `ldr_factor` | float | This factor is calculated into the raw ldr value wich is 0-1023 | 1.0 |
| `min_battery` | integer | Calibrates the minimum battery measurement by the given raw value. You will get that from the stats api | `475` |
| `max_battery` | integer | Calibrates the maximum battery measurement by the given raw value. You will get that from the stats api | `665` |
| `ha_prefix` | string | Sets the prefix for Homassistant discovery | `homeassistant` |
| `background_effect` | string | Sets an [effect](./effects) as global background layer | - |
| `stats_interval` | integer | Sets the interval in milliseconds when svitrix should send its stats to HA and MQTT | 10000 |
| `debug_mode` | boolean | Enables serial debug outputs. | false |
| `button_callback` | string | http callback url for button presses. | - |
| `new_year` | boolean | Displays fireworks and plays a jingle at newyear. | false |
| `swap_buttons` | boolean | Swaps the left and right hardware button. | false |
| `ldr_on_ground` | boolean | Sets the LDR configuration to LDR-on-ground. | false |
| `update_check` | boolean | Enables automatic firmware update checks. | true |
| `web_port` | integer | Changes the HTTP web server port. | `80` |
| `update_version_url` | string | Custom URL to check for firmware version. | built-in |
| `update_firmware_url` | string | Custom URL to download firmware binary. | built-in |


#### Example:
```json
{
  "temp_dec_places":1,
  "bootsound":true,
  "hum_offset":-2
}
```
