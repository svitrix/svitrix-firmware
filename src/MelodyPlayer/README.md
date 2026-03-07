# MelodyPlayer Block — AI Reference

> RTTTL melody parser + async/sync playback via ESP32 PWM (LEDC).

## Files

| File | Purpose |
|------|---------|
| `melody_player.h` / `.cpp` | Async playback engine using Ticker interrupts |
| `melody.h` | Melody data structure (title, notes, timeUnit) |
| `melody_factory.h` / `.cpp` | Melody creation from strings, files, arrays |
| `melody_factory_rtttl.cpp` | RTTTL format parser |
| `notes.h` | Note frequency constants (NOTE_C4=262 to NOTE_DS8=4978) |
| `notes_array.h` | String-to-frequency mapping table (92 entries) |

## MelodyPlayer

**Constructor**: `MelodyPlayer(pin, pwmChannel=0, offLevel=HIGH)`
**State**: `enum State { STOP, PLAY, PAUSE }`

### Playback API

| Method | Blocking | Notes |
|--------|----------|-------|
| `play()` | Yes | Plays loaded melody synchronously |
| `play(Melody&)` | Yes | Loads + plays synchronously |
| `playAsync()` | No | Plays loaded melody with Ticker interrupt |
| `playAsync(Melody&, loop, stopCallback)` | No | Full async with loop/callback |
| `stop()` | — | Halt + reset to beginning |
| `pause()` | — | Halt but preserve position |
| `mute()` / `unmute()` | — | Silence without stopping |
| `setVolume(byte 0-255)` | — | PWM duty cycle |
| `changeTempo(int bpm)` | — | Adjust BPM at runtime |

### Async Engine

```
playAsync() → schedule changeTone() via Ticker
  changeTone():
    advance to next note
    if silence: emit 0Hz for 0.3× duration
    else: emit frequency via ledcWriteTone()
    schedule next changeTone() after duration ms
    at end: stop(), loop if enabled, or fire callback
```

### PWM Control
- `turnOn()` → `ledcSetup(channel, 2000Hz, 8-bit)` + attach pin
- `turnOff()` → detach pin, set LOW (energy-safe for passive buzzer)
- `setVolume()` → `ledcWrite(channel, duty)` (0-255 → 0-127)

## Melody Data Structure

```cpp
struct NoteDuration {
  unsigned short frequency;   // Hz (0 = silence)
  unsigned short duration;    // Fixed-point: relative_units × 2
};

class Melody {
  String title;
  unsigned short timeUnit;              // ms per time unit
  shared_ptr<vector<NoteDuration>> notes;
  bool automaticSilence;                // Insert silence between notes
};
```

**Duration encoding**: stored as `relativeDuration * 2`
**Actual ms**: `timeUnit * (storedDuration / 2)`

## RTTTL Format

```
title:d=4,o=6,b=120:c,d,e,f,g
^^^^^  ^^^^^^^^^^^^^^^^^ ^^^^^^
title  defaults           notes
```

- `d` = default duration (1/N, N ∈ {1,2,4,8,16,32})
- `o` = default octave (4-7)
- `b` = BPM (10-300)

**Note syntax**: `[duration]note[#][octave][.]`
- `4c6` = quarter C6
- `8.g` = dotted eighth G (default octave)
- `p` = pause/silence
- `.` = +50% duration

**Duration mapping** (RTTTL denominator → relative units):
```
32→1, 16→2, 8→4, 4→8, 2→16, 1→32 (then ×2 for storage)
```

**BPM to timeUnit**: `60000 * 4 / BPM / 32`

## MelodyFactory Load Methods

| Method | Source |
|--------|--------|
| `loadRtttlString(const char[])` | RTTTL string in memory |
| `loadRtttlFile(path, fs)` | RTTTL file from LittleFS |
| `loadRtttlDB(path, title, fs)` | Find title in multi-melody file |
| `load(path, fs)` | Native MelodyPlayer format file |
| `load(title, timeUnit, notes[], n)` | From string note array |
| `load(title, timeUnit, freqs[], n)` | From frequency array |

## Integration

Games call `PeripheryManager.playRTTTLString("melody_name:d=8,o=5,b=120:notes")`.
Flow: `playRTTTLString()` → `MelodyFactory.loadRtttlString()` → `MelodyPlayer.playAsync(melody)`.
