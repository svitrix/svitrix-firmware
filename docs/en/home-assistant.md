# Home Assistant Integration

SVITRIX supports Home Assistant auto-discovery via MQTT. When enabled, the device automatically registers in HA with 66 controllable entities.

## Requirements

- Configured MQTT broker (Mosquitto, EMQX, etc.)
- Home Assistant with MQTT integration enabled
- SVITRIX connected to the same MQTT broker

## Configuration

### 1. In SVITRIX

Go to **Settings > MQTT** in the web interface:

| Field | Description |
|-------|-------------|
| Host | IP or hostname of the MQTT broker |
| Port | Port (default: 1883) |
| User / Password | Broker credentials |
| Prefix | MQTT topic prefix (default: `svitrix`) |
| **HA Discovery** | Enable for auto-discovery |

### 2. In Home Assistant

The MQTT integration must be configured. If using HA's Mosquitto add-on, it's ready. For external brokers:

```yaml
# configuration.yaml
mqtt:
  broker: 192.168.1.100
  port: 1883
  username: your_user
  password: your_password
```

Once HA Discovery is enabled in SVITRIX and restarted, entities will appear automatically in **Settings > Devices & Services > MQTT**.

---

## Available Entities (66 total)

### Lights (5)

| Entity | Description | Controls |
|--------|-------------|----------|
| **Matrix** | Main LED display | On/Off, Brightness (0-255), RGB Color |
| **Indicator 1** | Top-right indicator LED | On/Off, RGB Color |
| **Indicator 2** | Middle-right indicator LED | On/Off, RGB Color |
| **Indicator 3** | Bottom-right indicator LED | On/Off, RGB Color |
| **Night color** | Text color in night mode | RGB Color |

### Switches (14)

| Entity | Description |
|--------|-------------|
| **Transition** | Enable/disable automatic app transition |
| **Night mode** | Activate/deactivate night mode |
| **Night block transition** | Block transitions during night mode |
| **Sound enabled** | Enable/disable buzzer sounds |
| **Show time app** | Show/hide time app |
| **Show date app** | Show/hide date app |
| **Show temperature app** | Show/hide temperature app |
| **Show humidity app** | Show/hide humidity app |
| **Show battery app** | Show/hide battery app |
| **Show outdoor temp** | Show/hide outdoor temperature (weather) |
| **Show outdoor humidity** | Show/hide outdoor humidity (weather) |
| **Show pressure** | Show/hide atmospheric pressure (weather) |
| **Show air quality** | Show/hide air quality index (weather) |
| **Show UV index** | Show/hide UV index (weather) |

### Numbers (14)

| Entity | Range | Description |
|--------|-------|-------------|
| **Night brightness** | 1-50 | Brightness during night mode |
| **Sound volume** | 0-30 | Buzzer volume |
| **Time per app** | 1-60s | Duration of each app in rotation |
| **Scroll speed** | 20-200ms | Text scroll speed |
| **Clock duration** | 1-300s | Time app duration |
| **Date duration** | 1-60s | Date app duration |
| **Temperature duration** | 1-60s | Temperature app duration |
| **Humidity duration** | 1-60s | Humidity app duration |
| **Battery duration** | 1-60s | Battery app duration |
| **Outdoor temp duration** | 1-60s | Outdoor temperature duration |
| **Outdoor humidity duration** | 1-60s | Outdoor humidity duration |
| **Pressure duration** | 1-60s | Atmospheric pressure duration |
| **Air quality duration** | 1-60s | Air quality duration |
| **UV index duration** | 1-60s | UV index duration |

### Selects (3)

| Entity | Options | Description |
|--------|---------|-------------|
| **Brightness mode** | Manual, Auto | Brightness mode (manual or light sensor based) |
| **Transition effect** | 14 effects | Transition effect between apps |
| **Background effect** | 21 options | Background effect (None, Fade, Matrix, Plasma, Fire, etc.) |

### Buttons (8)

| Entity | Action |
|--------|--------|
| **Dismiss notification** | Close current notification |
| **Start Update** | Start OTA firmware update |
| **Next app** | Switch to next app |
| **Previous app** | Switch to previous app |
| **Reboot** | Restart device |
| **Play test sound** | Play a test beep |
| **Snooze alarm** | Snooze the active alarm |
| **Dismiss alarm** | Cancel the active alarm |

### Sensors (17-18)

| Entity | Unit | Description |
|--------|------|-------------|
| **Current app** | — | Active app name |
| **Device topic** | — | Device MQTT prefix |
| **Temperature** | °C | Ambient temperature (internal sensor) |
| **Humidity** | % | Relative humidity (internal sensor) |
| **Illuminance** | lx | Ambient light (LDR sensor) |
| **WiFi signal** | dB | WiFi signal strength |
| **Firmware version** | — | Firmware version |
| **Free RAM** | bytes | Available heap memory |
| **Uptime** | s | Time since last restart |
| **IP address** | — | Device IP address |
| **Battery** | % | Battery level (Ulanzi TC001 only) |
| **Outdoor temperature** | °C | Outdoor temperature (WeatherAPI) |
| **Outdoor humidity** | % | Outdoor humidity (WeatherAPI) |
| **Pressure** | hPa | Atmospheric pressure (WeatherAPI) |
| **Air quality** | AQI | Air quality index (WeatherAPI) |
| **Weather condition** | — | Current weather condition (WeatherAPI) |
| **UV index** | — | UV index (WeatherAPI) |
| **Next alarm** | — | Next scheduled alarm (HH:MM or "None") |

### Binary Sensors (4)

| Entity | Description |
|--------|-------------|
| **Button left** | Left button state |
| **Button middle** | Middle button state |
| **Button right** | Right button state |
| **Alarm ringing** | Indicates if an alarm is ringing |

---

## Automation Examples

### Mute when TV is on

```yaml
alias: "SVITRIX - Mute with TV"
trigger:
  - platform: state
    entity_id: media_player.living_room_tv
    to: "on"
action:
  - service: switch.turn_off
    target:
      entity_id: switch.svitrix_sound_enabled
```

### Activate night mode with scene

```yaml
alias: "SVITRIX - Movie mode"
trigger:
  - platform: state
    entity_id: input_boolean.movie_mode
    to: "on"
action:
  - service: switch.turn_on
    target:
      entity_id: switch.svitrix_night_mode
  - service: number.set_value
    target:
      entity_id: number.svitrix_night_brightness
    data:
      value: 5
  - service: light.turn_on
    target:
      entity_id: light.svitrix_night_color
    data:
      rgb_color: [255, 50, 0]
```

### Notification on button press

```yaml
alias: "SVITRIX - Middle button pressed"
trigger:
  - platform: state
    entity_id: binary_sensor.svitrix_button_middle
    to: "on"
action:
  - service: notify.mobile_app
    data:
      message: "Middle button pressed on SVITRIX"
```

### Automatic brightness by time

```yaml
alias: "SVITRIX - Brightness by schedule"
trigger:
  - platform: time
    at: "22:00:00"
action:
  - service: select.select_option
    target:
      entity_id: select.svitrix_brightness_mode
    data:
      option: "Manual"
  - service: light.turn_on
    target:
      entity_id: light.svitrix_matrix
    data:
      brightness: 30
```

### Alarm status indicator

```yaml
alias: "SVITRIX - Alarm indicator"
trigger:
  - platform: state
    entity_id: alarm_control_panel.home
action:
  - choose:
      - conditions:
          - condition: state
            entity_id: alarm_control_panel.home
            state: "armed_away"
        sequence:
          - service: light.turn_on
            target:
              entity_id: light.svitrix_indicator_1
            data:
              rgb_color: [255, 0, 0]
      - conditions:
          - condition: state
            entity_id: alarm_control_panel.home
            state: "disarmed"
        sequence:
          - service: light.turn_on
            target:
              entity_id: light.svitrix_indicator_1
            data:
              rgb_color: [0, 255, 0]
```

### Send notification via MQTT

For more advanced notifications, use the MQTT topic directly:

```yaml
alias: "SVITRIX - Doorbell notification"
trigger:
  - platform: state
    entity_id: binary_sensor.doorbell
    to: "on"
action:
  - service: mqtt.publish
    data:
      topic: "svitrix/notify"
      payload: >
        {
          "text": "Someone at the door",
          "icon": "door",
          "color": "#FFAA00",
          "duration": 10,
          "sound": "doorbell"
        }
```

---

## Google Calendar Integration

Display events, anniversaries, and tasks from Google Calendar on SVITRIX using Home Assistant as a bridge.

### Requirements

- Home Assistant with working MQTT integration
- Google account with access to Google Calendar

### 1. Configure Google Calendar in Home Assistant

1. Go to **Settings > Devices & Services > Add Integration**
2. Search for **Google Calendar**
3. Follow the OAuth authentication flow with your Google account
4. Select the calendars you want to import

Once configured, you'll have entities like:
- `calendar.my_personal_calendar`
- `calendar.birthdays`
- `calendar.work`

### 2. Test manually

Before automating, test by sending an event manually from **Developer Tools > Services**:

```yaml
service: mqtt.publish
data:
  topic: "svitrix/notify"
  payload: |
    {
      "text": "Calendar test",
      "icon": "6741",
      "duration": 10,
      "color": "#00BFFF"
    }
```

If it appears on SVITRIX, the MQTT connection works correctly.

### 3. Automations

#### Show next event as permanent app

```yaml
automation:
  - alias: "SVITRIX - Next calendar event"
    trigger:
      - platform: time_pattern
        minutes: "/15"  # update every 15 min
      - platform: state
        entity_id: calendar.my_calendar
    condition:
      - condition: state
        entity_id: calendar.my_calendar
        state: "on"  # there's an active or upcoming event
    action:
      - service: mqtt.publish
        data:
          topic: "svitrix/custom/calendar"
          payload: >
            {
              "text": "{{ state_attr('calendar.my_calendar', 'message') }}",
              "icon": "6741",
              "duration": 10,
              "color": "#00BFFF"
            }
```

#### Birthday/anniversary notification

```yaml
automation:
  - alias: "SVITRIX - Daily anniversaries"
    trigger:
      - platform: time
        at: "08:00:00"
    condition:
      - condition: state
        entity_id: calendar.birthdays
        state: "on"
    action:
      - service: mqtt.publish
        data:
          topic: "svitrix/notify"
          payload: >
            {
              "text": "{{ state_attr('calendar.birthdays', 'message') }}",
              "icon": "955",
              "duration": 30,
              "color": "#FF69B4",
              "sound": "birthday"
            }
```

#### Meeting reminder (15 min before)

```yaml
automation:
  - alias: "SVITRIX - Meeting reminder"
    trigger:
      - platform: calendar
        event: start
        entity_id: calendar.work
        offset: "-0:15:00"
    action:
      - service: mqtt.publish
        data:
          topic: "svitrix/notify"
          payload: >
            {
              "text": "In 15 min: {{ trigger.calendar_event.summary }}",
              "icon": "7956",
              "duration": 60,
              "color": "#FFA500",
              "sound": "chime"
            }
```

### 4. Recommended icons

| Icon | ID | Use |
|------|-----|-----|
| Calendar | 6741 | General events |
| Cake | 955 | Birthdays |
| Gift | 52 | Anniversaries |
| Meeting | 7956 | Work/meetings |
| Check | 51167 | Tasks |
| Alarm | 5765 | Reminders |

You can find more icons at [LaMetric Icon Gallery](https://developer.lametric.com/icons).

### 5. Remove calendar app

To remove the calendar app from rotation:

```yaml
service: mqtt.publish
data:
  topic: "svitrix/custom/calendar"
  payload: "{}"
```

---

## Alexa and Google Home Integration

Use SVITRIX with voice commands through Home Assistant.

### Requirements

- Home Assistant with working MQTT integration
- **For Alexa:** [Alexa Media Player](https://github.com/custom-components/alexa_media_player) (HACS) or Home Assistant Cloud
- **For Google:** Native Google Assistant integration or Home Assistant Cloud

### Initial Configuration

#### 1. Expose entities to assistants

To control SVITRIX by voice, expose these entities:

```yaml
# configuration.yaml
cloud:
  alexa:
    filter:
      include_entities:
        - light.svitrix_matrix
        - switch.svitrix_night_mode
        - switch.svitrix_sound_enabled
  google_assistant:
    filter:
      include_entities:
        - light.svitrix_matrix
        - switch.svitrix_night_mode
        - switch.svitrix_sound_enabled
```

#### 2. Create input_boolean for routines

To activate automations by voice:

```yaml
# configuration.yaml
input_boolean:
  good_morning:
    name: Good morning
    icon: mdi:weather-sunny
  show_message:
    name: Show message
    icon: mdi:message
```

Expose these `input_boolean` to Alexa/Google. Then create routines:
- **Alexa:** "When I say 'Good morning'" → Activate `input_boolean.good_morning`
- **Google:** "When I say 'Good morning'" → Activate `input_boolean.good_morning`

### Available voice commands

Once configured, you can use:

| Command | Action |
|---------|--------|
| "Alexa, turn on the clock" | Turn on the matrix |
| "Hey Google, turn off the clock" | Turn off the matrix |
| "Alexa, set clock brightness to 50%" | Adjust brightness |
| "Hey Google, activate night mode" | Activate night mode |
| "Alexa, Good morning" | Execute morning routine |

### Blueprints for voice assistants

| Blueprint | Description | Import |
|-----------|-------------|--------|
| **Button to announcement** | Physical button → Alexa/Google announces | [![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https://xe1e.github.io/svitrix-firmware-XE1E/blueprints/svitrix_button_announcement.yaml) |
| **Voice notification** | Script to send text to SVITRIX | [![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https://xe1e.github.io/svitrix-firmware-XE1E/blueprints/svitrix_voice_notification.yaml) |
| **Morning routine** | "Good morning" → weather + calendar | [![Import](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https://xe1e.github.io/svitrix-firmware-XE1E/blueprints/svitrix_morning_routine.yaml) |

---

## Troubleshooting

### Entities don't appear in HA

1. Verify MQTT is connected (WiFi+MQTT icon on SVITRIX)
2. Confirm "HA Discovery" is enabled in Settings > MQTT
3. Restart SVITRIX after enabling discovery
4. Check HA logs in **Settings > System > Logs**

### Entities appear as "unavailable"

- The device may be disconnected or restarting
- Verify MQTT connection on both sides

### Changes aren't reflected

- Some changes require SVITRIX to publish its state (every 5 seconds by default)
- To force immediate update, restart the device

### Orphan entities (from previous versions)

If you see entities like "Clock Color", "Time Color" that don't respond:
1. Go to **HA > Settings > Devices & Services > MQTT**
2. Find your SVITRIX device
3. Delete the non-responsive entities manually

Or force full re-discovery:
1. In SVITRIX web: disable "HA Discovery", save
2. In HA: delete the entire SVITRIX device
3. In SVITRIX web: re-enable "HA Discovery", save

---

## See also

- [MQTT/HTTP API](api.md) — Advanced commands via MQTT
- [MQTT Setup with Docker](mqtt-docker-setup.md) — Mosquitto installation
