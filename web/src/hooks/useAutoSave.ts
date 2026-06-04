import { useRef, useCallback, useEffect } from "preact/hooks";
import type { Settings } from "../api/types";
import { saveSettings } from "../api/client";
import { toast } from "../components/Toast";
import { getT } from "../i18n";

const COLOR_KEYS = [
  "TCOL", "CHCOL", "CTCOL", "CBCOL", "WDCA", "WDCI",
  "TIME_COL", "DATE_COL", "TEMP_COL", "HUM_COL", "BAT_COL", "NCOL",
];

export function prepareForSave(fields: Partial<Settings>): Record<string, unknown> {
  const out: Record<string, unknown> = { ...fields } as unknown as Record<string, unknown>;
  for (const key of COLOR_KEYS) {
    if (key in out) {
      const v = out[key];
      if (typeof v === "number") {
        out[key] = "#" + (v & 0xffffff).toString(16).padStart(6, "0");
      }
    }
  }
  return out;
}

interface AutoSaveOptions {
  delay?: number;
  showToast?: boolean;
}

export function useDebounce<T extends (...args: Parameters<T>) => void>(
  fn: T,
  delay: number
): T {
  const timeoutRef = useRef<ReturnType<typeof setTimeout> | null>(null);

  const debouncedFn = useCallback(
    (...args: Parameters<T>) => {
      if (timeoutRef.current) {
        clearTimeout(timeoutRef.current);
      }
      timeoutRef.current = setTimeout(() => {
        fn(...args);
      }, delay);
    },
    [fn, delay]
  ) as T;

  useEffect(() => {
    return () => {
      if (timeoutRef.current) {
        clearTimeout(timeoutRef.current);
      }
    };
  }, []);

  return debouncedFn;
}

export function useAutoSave(options: AutoSaveOptions = {}) {
  const { delay = 500, showToast = false } = options;
  const pendingRef = useRef<Partial<Settings>>({});
  const savingRef = useRef(false);

  const doSave = useCallback(async () => {
    if (savingRef.current || Object.keys(pendingRef.current).length === 0) return;

    savingRef.current = true;
    const toSave = { ...pendingRef.current };
    pendingRef.current = {};

    try {
      await saveSettings(prepareForSave(toSave) as Partial<Settings>);
      if (showToast) {
        const t = getT();
        toast(t.ok);
      }
    } catch (e) {
      console.error("Auto-save failed:", e);
      const t = getT();
      toast(t.errorSaving);
      // Re-queue failed fields
      pendingRef.current = { ...toSave, ...pendingRef.current };
    } finally {
      savingRef.current = false;
    }
  }, [showToast]);

  const debouncedSave = useDebounce(doSave, delay);

  const queueSave = useCallback((fields: Partial<Settings>) => {
    pendingRef.current = { ...pendingRef.current, ...fields };
    debouncedSave();
  }, [debouncedSave]);

  const saveNow = useCallback(async (fields: Partial<Settings>) => {
    try {
      await saveSettings(prepareForSave(fields) as Partial<Settings>);
    } catch (e) {
      console.error("Instant save failed:", e);
      const t = getT();
      toast(t.errorSaving);
    }
  }, []);

  return { queueSave, saveNow };
}
