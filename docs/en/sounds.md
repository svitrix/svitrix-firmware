# Sounds

SVITRIX uses the integrated passive buzzer in the Ulanzi TC001 to play monophonic RTTTL melodies.

## Playing Melodies

You can play RTTTL melodies in two ways:

**Via API:**
Send the RTTTL string directly with your API request.
See documentation for commands:
- [Single sounds](./api#sound-playback)
- [With your notification](./api#json-properties)

**Via file:**
This method avoids long JSON payloads since the receive buffer is limited.
Go to the file manager in the [web interface](./webinterface) and create a new text file in the `MELODIES` folder.
Name it whatever you like but use the `.txt` extension, e.g. `alarm.txt`. Inside the file, place a melody in RTTTL format.
When referencing the sound file in API calls, omit the file extension.

You can find many melodies on the internet:
* [Laub-Home Wiki: RTTTL Songs](https://www.laub-home.de/wiki/RTTTL_Songs)
* [Online RTTTL player](https://adamonsoon.github.io/rtttl-play/)
* [RTTTL editor](https://corax89.github.io/esp8266Game/soundEditor.html)

## Volume Control

Volume controls the PWM duty cycle of the buzzer. Range: **0–30** (default: **25**).

**On-screen menu:**
Navigate to the `VOLUME` menu item on the clock. Use left/right buttons to adjust. Long-press select to save.

**Via API:**
```bash
# Set volume to 15
curl -X POST http://<ip>/api/settings -d '{"VOL": 15}'

# Get current volume
curl http://<ip>/api/settings
```

The volume setting is saved to flash and persists across reboots.

## Sound Toggle

Sound can be enabled or disabled globally:

**On-screen menu:**
Navigate to the `SOUND` menu item. Toggle with left/right buttons. Long-press select to save.

**Via API:**
```json
{"SOUND": true}
```

When sound is disabled, all melody playback (notifications, boot sound, API calls) is muted. The volume level is preserved and restored when sound is re-enabled.
