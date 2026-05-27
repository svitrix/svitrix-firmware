import { useState, useEffect, useRef } from "preact/hooks";
import {
  getTimer,
  timerStart,
  timerPause,
  timerReset,
  timerSetTime,
  getStopwatch,
  stopwatchStart,
  stopwatchPause,
  stopwatchReset,
  getAlarms,
  addAlarm,
  updateAlarm,
  deleteAlarm,
  snoozeAlarm,
  dismissAlarm,
} from "../../api/client";
import type { TimerState, StopwatchState, Alarm, AlarmsState } from "../../api/types";
import styles from "./Autonomous.module.css";

const DAYS = ["S", "M", "T", "W", "T", "F", "S"];

function formatTime(seconds: number): string {
  const h = Math.floor(seconds / 3600);
  const m = Math.floor((seconds % 3600) / 60);
  const s = seconds % 60;
  if (h > 0) {
    return `${h}:${m.toString().padStart(2, "0")}:${s.toString().padStart(2, "0")}`;
  }
  return `${m.toString().padStart(2, "0")}:${s.toString().padStart(2, "0")}`;
}

function formatMs(ms: number): string {
  const totalSecs = Math.floor(ms / 1000);
  const centis = Math.floor((ms % 1000) / 10);
  const m = Math.floor(totalSecs / 60);
  const s = totalSecs % 60;
  return `${m.toString().padStart(2, "0")}:${s.toString().padStart(2, "0")}.${centis.toString().padStart(2, "0")}`;
}

function TimerSection() {
  const [timer, setTimer] = useState<TimerState | null>(null);
  const [inputMins, setInputMins] = useState(5);
  const [inputSecs, setInputSecs] = useState(0);
  const pollRef = useRef<ReturnType<typeof setInterval>>();

  const poll = async () => {
    try {
      setTimer(await getTimer());
    } catch {
      // ignore
    }
  };

  useEffect(() => {
    poll();
    pollRef.current = setInterval(poll, 1000);
    return () => clearInterval(pollRef.current);
  }, []);

  const handleSet = async () => {
    const totalSecs = inputMins * 60 + inputSecs;
    await timerSetTime(totalSecs);
    poll();
  };

  const timerClass = [
    styles.timerDisplay,
    timer?.running ? styles.running : "",
    timer?.finished ? styles.finished : "",
  ].join(" ");

  return (
    <div class={styles.section}>
      <div class={styles.sectionTitle}>Timer</div>
      <div class={timerClass}>
        {timer ? formatTime(timer.remaining) : "--:--"}
      </div>
      <div class={styles.timeInput}>
        <input
          type="number"
          min="0"
          max="99"
          value={inputMins}
          onChange={(e) => setInputMins(parseInt((e.target as HTMLInputElement).value) || 0)}
        />
        <span>:</span>
        <input
          type="number"
          min="0"
          max="59"
          value={inputSecs}
          onChange={(e) => setInputSecs(parseInt((e.target as HTMLInputElement).value) || 0)}
        />
        <button class={styles.btnReset} onClick={handleSet}>Set</button>
      </div>
      <div class={styles.controls}>
        <button
          class={styles.btnStart}
          onClick={() => { timerStart(); poll(); }}
          disabled={timer?.running || timer?.remaining === 0}
        >
          Start
        </button>
        <button
          class={styles.btnPause}
          onClick={() => { timerPause(); poll(); }}
          disabled={!timer?.running}
        >
          Pause
        </button>
        <button
          class={styles.btnReset}
          onClick={() => { timerReset(); poll(); }}
        >
          Reset
        </button>
      </div>
    </div>
  );
}

function StopwatchSection() {
  const [sw, setSw] = useState<StopwatchState | null>(null);
  const pollRef = useRef<ReturnType<typeof setInterval>>();

  const poll = async () => {
    try {
      setSw(await getStopwatch());
    } catch {
      // ignore
    }
  };

  useEffect(() => {
    poll();
    pollRef.current = setInterval(poll, 100);
    return () => clearInterval(pollRef.current);
  }, []);

  const displayClass = [
    styles.timerDisplay,
    sw?.running ? styles.running : "",
  ].join(" ");

  return (
    <div class={styles.section}>
      <div class={styles.sectionTitle}>Stopwatch</div>
      <div class={displayClass}>
        {sw ? formatMs(sw.elapsed) : "--:--.--"}
      </div>
      <div class={styles.controls}>
        <button
          class={styles.btnStart}
          onClick={() => { stopwatchStart(); poll(); }}
          disabled={sw?.running}
        >
          Start
        </button>
        <button
          class={styles.btnPause}
          onClick={() => { stopwatchPause(); poll(); }}
          disabled={!sw?.running}
        >
          Pause
        </button>
        <button
          class={styles.btnReset}
          onClick={() => { stopwatchReset(); poll(); }}
        >
          Reset
        </button>
      </div>
    </div>
  );
}

function AlarmsSection() {
  const [state, setState] = useState<AlarmsState | null>(null);
  const [newTime, setNewTime] = useState("07:00");
  const [newLabel, setNewLabel] = useState("");
  const pollRef = useRef<ReturnType<typeof setInterval>>();

  const poll = async () => {
    try {
      setState(await getAlarms());
    } catch {
      // ignore
    }
  };

  useEffect(() => {
    poll();
    pollRef.current = setInterval(poll, 2000);
    return () => clearInterval(pollRef.current);
  }, []);

  const handleAdd = async () => {
    const [h, m] = newTime.split(":").map(Number);
    await addAlarm({
      hour: h,
      minute: m,
      days: 0x7f,
      enabled: true,
      label: newLabel,
      melody: "",
    });
    setNewLabel("");
    poll();
  };

  const toggleDay = async (alarm: Alarm, day: number) => {
    const mask = 1 << day;
    await updateAlarm({ ...alarm, days: alarm.days ^ mask });
    poll();
  };

  const toggleEnabled = async (alarm: Alarm) => {
    await updateAlarm({ ...alarm, enabled: !alarm.enabled });
    poll();
  };

  const handleDelete = async (id: number) => {
    await deleteAlarm(id);
    poll();
  };

  return (
    <div class={styles.section}>
      <div class={styles.sectionTitle}>Alarms</div>

      {state?.ringing && (
        <div class={styles.ringingAlert}>
          <div>ALARM RINGING!</div>
          <button class={styles.btnPause} onClick={() => { snoozeAlarm(5); poll(); }}>
            Snooze 5min
          </button>
          <button class={styles.btnDanger} onClick={() => { dismissAlarm(); poll(); }}>
            Dismiss
          </button>
        </div>
      )}

      <div class={styles.alarmList}>
        {state?.alarms.map((alarm) => (
          <div key={alarm.id} class={styles.alarmItem}>
            <div
              class={`${styles.toggle} ${alarm.enabled ? styles.on : ""}`}
              onClick={() => toggleEnabled(alarm)}
            />
            <div class={styles.alarmTime}>
              {alarm.hour.toString().padStart(2, "0")}:
              {alarm.minute.toString().padStart(2, "0")}
            </div>
            <div class={styles.alarmDays}>
              {DAYS.map((d, i) => (
                <div
                  key={i}
                  class={`${styles.dayBadge} ${alarm.days & (1 << i) ? styles.active : ""}`}
                  onClick={() => toggleDay(alarm, i)}
                >
                  {d}
                </div>
              ))}
            </div>
            <div class={styles.alarmLabel}>{alarm.label || "Alarm"}</div>
            <div class={styles.alarmActions}>
              <button class={styles.btnDanger} onClick={() => handleDelete(alarm.id)}>
                X
              </button>
            </div>
          </div>
        ))}

        {(!state?.alarms || state.alarms.length === 0) && (
          <div class={styles.emptyState}>No alarms configured</div>
        )}
      </div>

      <div class={styles.addAlarm}>
        <input
          type="time"
          value={newTime}
          onChange={(e) => setNewTime((e.target as HTMLInputElement).value)}
        />
        <input
          type="text"
          placeholder="Label (optional)"
          value={newLabel}
          onChange={(e) => setNewLabel((e.target as HTMLInputElement).value)}
        />
        <button class={styles.btnStart} onClick={handleAdd}>
          Add
        </button>
      </div>
    </div>
  );
}

export function AutonomousPage(_props: { path?: string }) {
  return (
    <div class={styles.container}>
      <TimerSection />
      <StopwatchSection />
      <AlarmsSection />
    </div>
  );
}
