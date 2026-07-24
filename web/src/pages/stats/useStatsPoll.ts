import { useEffect, useRef, useState } from "preact/hooks";
import type { Stats } from "../../api/types";
import { getStats } from "../../api/client";

export interface StatsHistory {
  ram: number[]; // KB
  wifi: number[]; // dBm
  temp: number[]; // °C
}

function push(arr: number[], v: number, max: number): number[] {
  const next = arr.concat(v);
  return next.length > max ? next.slice(next.length - max) : next;
}

/** Poll /api/stats on an interval, keeping the latest value + a small ring
 *  buffer of history for sparklines. Pauses on hidden tab; single-flight so a
 *  slow device is never hammered with overlapping requests. */
export function useStatsPoll(initial: Stats | null, intervalMs = 5000, bufSize = 30) {
  const [stats, setStats] = useState<Stats | null>(initial);
  const [history, setHistory] = useState<StatsHistory>({ ram: [], wifi: [], temp: [] });
  const inFlight = useRef(false);

  useEffect(() => {
    let alive = true;

    async function tick() {
      if (document.hidden || inFlight.current) return;
      inFlight.current = true;
      try {
        const s = await getStats();
        if (!alive) return;
        setStats(s);
        setHistory((h) => ({
          ram: push(h.ram, Math.round(s.ram / 1024), bufSize),
          wifi: push(h.wifi, s.wifi_signal, bufSize),
          temp: push(h.temp, s.temp, bufSize),
        }));
      } catch {
        /* keep last good value */
      } finally {
        inFlight.current = false;
      }
    }

    tick();
    const timer = setInterval(tick, intervalMs);
    const onVis = () => { if (!document.hidden) tick(); };
    document.addEventListener("visibilitychange", onVis);
    return () => {
      alive = false;
      clearInterval(timer);
      document.removeEventListener("visibilitychange", onVis);
    };
  }, [intervalMs, bufSize]);

  return { stats, history };
}
