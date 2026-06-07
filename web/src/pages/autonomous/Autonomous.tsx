import { useState, useEffect, useRef } from "preact/hooks";
import {
  getAlarms,
  addAlarm,
  updateAlarm,
  deleteAlarm,
  snoozeAlarm,
  dismissAlarm,
  playRtttl,
} from "../../api/client";
import type { Alarm, AlarmsState } from "../../api/types";
import { useT } from "../../i18n";
import { useSettings } from "../../context/SettingsContext";
import styles from "./Autonomous.module.css";

// Preset RTTTL melodies offered in the alarm melody dropdown.
const MELODIES = [
  { name: "Beep", rtttl: "beep:d=4,o=5,b=160:16e6,16e6" },
  { name: "Alarm", rtttl: "alarm:d=8,o=6,b=300:c,p,c,p,c,p,c,p,c,p,c,p" },
  { name: "Nokia", rtttl: "Nokia:d=4,o=5,b=225:8e6,8d6,4f#,4g#,8c#6,8b,4d,4e,8b,8a,4c#,4e,2a" },
  { name: "Mario", rtttl: "Mario:d=4,o=5,b=200:8e6,8e6,16p,8e6,16p,8c6,8e6,16p,8g6,8p,8g,8p" },
  { name: "Star Wars", rtttl: "StarWars:d=4,o=5,b=45:32p,8d.,8d.,8d.,8a#4,16f,8d.,8a#4,16f,8d.,2a" },
  { name: "Reveille", rtttl: "Reveille:d=4,o=5,b=140:8g,8c6,8e6,8g6,8e6,8c6,8g,8c6,8e6,8g6,8e6,8c6" },
];

// Minutes from `now` until the alarm's next fire, or -1 if it never will.
// Mirrors firmware AlarmLogic for an at-a-glance "next alarm" display.
function minutesUntil(a: Alarm, now: Date): number {
  if (!a.enabled) return -1;
  const nowMins = now.getHours() * 60 + now.getMinutes();
  const alarmMins = a.hour * 60 + a.minute;
  if (a.oneTime) {
    let d = alarmMins - nowMins;
    if (d < 0) d += 1440;
    return d;
  }
  if (a.days === 0) return -1;
  const wday = now.getDay(); // 0=Sun
  let best = -1;
  for (let off = 0; off < 7; off++) {
    const day = (wday + off) % 7;
    if (!(a.days & (1 << day))) continue;
    let d = off * 1440 + (alarmMins - nowMins);
    if (d < 0) d += 7 * 1440;
    if (best < 0 || d < best) best = d;
  }
  return best;
}

function fmtUntil(mins: number): string {
  const h = Math.floor(mins / 60);
  const m = mins % 60;
  return h > 0 ? `${h}h ${m}m` : `${m}m`;
}

function AlarmsSection() {
  const [state, setState] = useState<AlarmsState | null>(null);
  const [newTime, setNewTime] = useState("07:00");
  const [newLabel, setNewLabel] = useState("");
  const [newOnce, setNewOnce] = useState(false);
  const pollRef = useRef<ReturnType<typeof setInterval>>();
  const t = useT();
  const { settings, instantSave } = useSettings();

  // Full reload — replaces the whole list (mount, add, delete).
  const load = async () => {
    try {
      setState(await getAlarms());
    } catch {
      // ignore
    }
  };

  useEffect(() => {
    load();
    // Poll ONLY the ringing flag so it never clobbers a field being edited.
    pollRef.current = setInterval(async () => {
      try {
        const s = await getAlarms();
        setState((prev) => (prev ? { ...prev, ringing: s.ringing } : s));
      } catch {
        // ignore
      }
    }, 2000);
    return () => clearInterval(pollRef.current);
  }, []);

  // Update one alarm in local state only — instant, no network, no clobber.
  const setLocal = (id: number, fields: Partial<Alarm>) =>
    setState((prev) =>
      prev
        ? { ...prev, alarms: prev.alarms.map((a) => (a.id === id ? { ...a, ...fields } : a)) }
        : prev
    );

  // Apply locally and persist to the device.
  const patch = (alarm: Alarm, fields: Partial<Alarm>) => {
    setLocal(alarm.id, fields);
    updateAlarm({ ...alarm, ...fields });
  };

  const handleAdd = async () => {
    const [h, m] = newTime.split(":").map(Number);
    await addAlarm({
      hour: h,
      minute: m,
      days: newOnce ? 0 : 0x7f,
      enabled: true,
      oneTime: newOnce,
      snoozeMinutes: 5,
      label: newLabel,
      melody: "",
    });
    setNewLabel("");
    setNewOnce(false);
    load();
  };

  const setTime = (alarm: Alarm, value: string) => {
    const [h, m] = value.split(":").map(Number);
    patch(alarm, { hour: h, minute: m });
  };

  const handleDelete = async (id: number) => {
    await deleteAlarm(id);
    load();
  };

  // Soonest upcoming alarm (computed from browser time, for display only).
  let soonest = -1;
  if (state?.alarms) {
    const now = new Date();
    for (const a of state.alarms) {
      const mu = minutesUntil(a, now);
      if (mu >= 0 && (soonest < 0 || mu < soonest)) soonest = mu;
    }
  }

  const fmtTime = (a: Alarm) =>
    `${a.hour.toString().padStart(2, "0")}:${a.minute.toString().padStart(2, "0")}`;

  return (
    <div class={styles.section}>
      <div class={styles.sectionTitle}>
        {t.alarms.title}
        {soonest >= 0 && (
          <span class={styles.nextAlarm}>
            {t.alarms.nextAlarm}: {fmtUntil(soonest)}
          </span>
        )}
      </div>

      {settings && (
        <label class={styles.indicatorToggle}>
          <input
            type="checkbox"
            checked={settings.SALARMS}
            onChange={(e) => instantSave({ SALARMS: (e.target as HTMLInputElement).checked })}
          />
          {t.apps.alarmsIndicator}
        </label>
      )}

      {state?.ringing && (
        <div class={styles.ringingAlert}>
          <div>{t.alarms.ringing}</div>
          <button class={styles.btnPause} onClick={() => { snoozeAlarm(5); load(); }}>
            {t.alarms.snooze5min}
          </button>
          <button class={styles.btnDanger} onClick={() => { dismissAlarm(); load(); }}>
            {t.alarms.dismiss}
          </button>
        </div>
      )}

      <div class={styles.alarmList}>
        {state?.alarms.map((alarm) => (
          <div key={alarm.id} class={styles.alarmItem}>
            <div
              class={`${styles.toggle} ${alarm.enabled ? styles.on : ""}`}
              onClick={() => patch(alarm, { enabled: !alarm.enabled })}
            />
            <input
              type="time"
              class={styles.alarmTimeInput}
              value={fmtTime(alarm)}
              onChange={(e) => setTime(alarm, (e.target as HTMLInputElement).value)}
            />

            {alarm.oneTime ? (
              <div class={`${styles.dayBadge} ${styles.active}`}>1×</div>
            ) : (
              <div class={styles.alarmDays}>
                {t.alarms.dayLetters.map((d, i) => (
                  <div
                    key={i}
                    class={`${styles.dayBadge} ${alarm.days & (1 << i) ? styles.active : ""}`}
                    onClick={() => patch(alarm, { days: alarm.days ^ (1 << i) })}
                  >
                    {d}
                  </div>
                ))}
              </div>
            )}

            <label class={styles.onceToggle}>
              <input
                type="checkbox"
                checked={alarm.oneTime}
                onChange={(e) =>
                  patch(alarm, {
                    oneTime: (e.target as HTMLInputElement).checked,
                    days: (e.target as HTMLInputElement).checked ? 0 : 0x7f,
                  })
                }
              />
              {t.alarms.once}
            </label>

            <input
              type="text"
              class={styles.alarmLabelInput}
              placeholder={t.alarms.labelPlaceholder}
              value={alarm.label}
              onInput={(e) => setLocal(alarm.id, { label: (e.target as HTMLInputElement).value })}
              onBlur={(e) => updateAlarm({ ...alarm, label: (e.target as HTMLInputElement).value })}
            />
            <select
              class={styles.alarmMelodyInput}
              value={MELODIES.some((m) => m.rtttl === alarm.melody) ? alarm.melody : ""}
              onChange={(e) => patch(alarm, { melody: (e.target as HTMLSelectElement).value })}
            >
              <option value="">{t.alarms.melodyNone}</option>
              {MELODIES.map((m) => (
                <option key={m.name} value={m.rtttl}>
                  {m.name}
                </option>
              ))}
            </select>
            <button
              class={styles.previewBtn}
              title={t.alarms.preview}
              disabled={!alarm.melody}
              onClick={() => playRtttl(alarm.melody)}
            >
              ▶
            </button>
            <label class={styles.snoozeField}>
              {t.alarms.snoozeMin}
              <input
                type="number"
                min={1}
                max={60}
                value={alarm.snoozeMinutes}
                onInput={(e) =>
                  setLocal(alarm.id, { snoozeMinutes: Number((e.target as HTMLInputElement).value) || 0 })
                }
                onBlur={(e) => {
                  const v = Math.min(60, Math.max(1, Number((e.target as HTMLInputElement).value) || 5));
                  patch(alarm, { snoozeMinutes: v });
                }}
              />
            </label>

            <div class={styles.alarmActions}>
              <button class={styles.btnDanger} onClick={() => handleDelete(alarm.id)}>
                X
              </button>
            </div>
          </div>
        ))}

        {(!state?.alarms || state.alarms.length === 0) && (
          <div class={styles.emptyState}>{t.alarms.noAlarms}</div>
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
          placeholder={t.alarms.labelPlaceholder}
          value={newLabel}
          onChange={(e) => setNewLabel((e.target as HTMLInputElement).value)}
        />
        <label class={styles.onceToggle}>
          <input
            type="checkbox"
            checked={newOnce}
            onChange={(e) => setNewOnce((e.target as HTMLInputElement).checked)}
          />
          {t.alarms.once}
        </label>
        <button class={styles.btnStart} onClick={handleAdd}>
          {t.alarms.add}
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
