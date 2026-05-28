import { useState, useEffect, useRef } from "preact/hooks";
import {
  getAlarms,
  addAlarm,
  updateAlarm,
  deleteAlarm,
  snoozeAlarm,
  dismissAlarm,
} from "../../api/client";
import type { Alarm, AlarmsState } from "../../api/types";
import styles from "./Autonomous.module.css";

const DAYS = ["S", "M", "T", "W", "T", "F", "S"];

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
      <AlarmsSection />
    </div>
  );
}
