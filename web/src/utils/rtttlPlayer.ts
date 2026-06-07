/**
 * RTTTL Parser and Player using Web Audio API
 * Plays Ring Tone Text Transfer Language melodies in the browser
 */

interface RtttlNote {
  frequency: number;
  duration: number; // in seconds
}

interface RtttlParsed {
  name: string;
  notes: RtttlNote[];
}

// Note frequencies (Hz) for octave 4
const NOTE_FREQS: Record<string, number> = {
  c: 261.63,
  "c#": 277.18,
  d: 293.66,
  "d#": 311.13,
  e: 329.63,
  f: 349.23,
  "f#": 369.99,
  g: 392.00,
  "g#": 415.30,
  a: 440.00,
  "a#": 466.16,
  b: 493.88,
  p: 0, // pause
};

/**
 * Parse an RTTTL string into notes with frequencies and durations
 */
export function parseRtttl(rtttl: string): RtttlParsed | null {
  try {
    const parts = rtttl.split(":");
    if (parts.length !== 3) return null;

    const name = parts[0].trim();
    const defaults = parts[1].trim();
    const notesStr = parts[2].trim();

    // Parse defaults (d=duration, o=octave, b=bpm)
    let defaultDuration = 4;
    let defaultOctave = 6;
    let bpm = 63;

    defaults.split(",").forEach((param) => {
      const [key, value] = param.trim().split("=");
      if (key === "d") defaultDuration = parseInt(value, 10);
      if (key === "o") defaultOctave = parseInt(value, 10);
      if (key === "b") bpm = parseInt(value, 10);
    });

    const wholeNoteDuration = (60 / bpm) * 4; // duration of a whole note in seconds
    const notes: RtttlNote[] = [];

    notesStr.split(",").forEach((noteStr) => {
      const note = noteStr.trim().toLowerCase();
      if (!note) return;

      let i = 0;

      // Parse duration (optional, at start)
      let duration = defaultDuration;
      let durationStr = "";
      while (i < note.length && /\d/.test(note[i])) {
        durationStr += note[i];
        i++;
      }
      if (durationStr) duration = parseInt(durationStr, 10);

      // Parse note name
      let noteName = "";
      if (i < note.length && /[a-gp]/.test(note[i])) {
        noteName = note[i];
        i++;
      }

      // Check for sharp
      if (i < note.length && note[i] === "#") {
        noteName += "#";
        i++;
      }

      // Check for dotted note
      let dotted = false;
      if (i < note.length && note[i] === ".") {
        dotted = true;
        i++;
      }

      // Parse octave (optional, at end)
      let octave = defaultOctave;
      let octaveStr = "";
      while (i < note.length && /\d/.test(note[i])) {
        octaveStr += note[i];
        i++;
      }
      if (octaveStr) octave = parseInt(octaveStr, 10);

      // Check for dotted note after octave
      if (i < note.length && note[i] === ".") {
        dotted = true;
      }

      // Calculate frequency
      let frequency = 0;
      if (noteName && noteName !== "p") {
        const baseFreq = NOTE_FREQS[noteName] || 0;
        // Adjust for octave (octave 4 is base)
        frequency = baseFreq * Math.pow(2, octave - 4);
      }

      // Calculate duration in seconds
      let noteDuration = wholeNoteDuration / duration;
      if (dotted) noteDuration *= 1.5;

      notes.push({ frequency, duration: noteDuration });
    });

    return { name, notes };
  } catch {
    return null;
  }
}

let audioContext: AudioContext | null = null;
let currentOscillator: OscillatorNode | null = null;
let isPlaying = false;
let stopRequested = false;

/**
 * Stop any currently playing melody
 */
export function stopRtttl(): void {
  stopRequested = true;
  if (currentOscillator) {
    try {
      currentOscillator.stop();
      currentOscillator.disconnect();
    } catch {
      // Ignore errors when stopping
    }
    currentOscillator = null;
  }
  isPlaying = false;
}

/**
 * Play an RTTTL melody using Web Audio API
 */
export async function playRtttlLocal(rtttl: string): Promise<void> {
  const parsed = parseRtttl(rtttl);
  if (!parsed || parsed.notes.length === 0) {
    throw new Error("Invalid RTTTL format");
  }

  // Stop any currently playing melody
  stopRtttl();
  stopRequested = false;

  // Create or reuse audio context
  if (!audioContext) {
    audioContext = new (window.AudioContext || (window as unknown as { webkitAudioContext: typeof AudioContext }).webkitAudioContext)();
  }

  // Resume context if suspended (browser autoplay policy)
  if (audioContext.state === "suspended") {
    await audioContext.resume();
  }

  isPlaying = true;

  for (const note of parsed.notes) {
    if (stopRequested || !isPlaying) break;

    if (note.frequency > 0) {
      // Create oscillator for this note
      const oscillator = audioContext.createOscillator();
      const gainNode = audioContext.createGain();

      oscillator.type = "square"; // Square wave sounds more like a buzzer
      oscillator.frequency.value = note.frequency;

      // Envelope to avoid clicks
      gainNode.gain.setValueAtTime(0, audioContext.currentTime);
      gainNode.gain.linearRampToValueAtTime(0.3, audioContext.currentTime + 0.01);
      gainNode.gain.setValueAtTime(0.3, audioContext.currentTime + note.duration - 0.02);
      gainNode.gain.linearRampToValueAtTime(0, audioContext.currentTime + note.duration);

      oscillator.connect(gainNode);
      gainNode.connect(audioContext.destination);

      currentOscillator = oscillator;
      oscillator.start();
      oscillator.stop(audioContext.currentTime + note.duration);

      // Wait for note duration
      await new Promise((resolve) => setTimeout(resolve, note.duration * 1000));
    } else {
      // Pause/rest
      await new Promise((resolve) => setTimeout(resolve, note.duration * 1000));
    }
  }

  isPlaying = false;
  currentOscillator = null;
}

/**
 * Check if a melody is currently playing
 */
export function isRtttlPlaying(): boolean {
  return isPlaying;
}
