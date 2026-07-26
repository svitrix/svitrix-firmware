import { useRef } from "preact/hooks";
import { useTranslation } from "react-i18next";
import styles from "./SettingsNav.module.css";

export type CategoryId = "appearance" | "screens" | "network" | "system" | "tools";

export const CATEGORY_IDS: CategoryId[] = [
  "appearance",
  "screens",
  "network",
  "system",
  "tools",
];

interface SettingsNavProps {
  active: CategoryId;
  onChange: (id: CategoryId) => void;
}

export function SettingsNav({ active, onChange }: SettingsNavProps) {
  const { t } = useTranslation();
  const btnRefs = useRef<(HTMLButtonElement | null)[]>([]);

  const focusTab = (index: number) => {
    const clamped = (index + CATEGORY_IDS.length) % CATEGORY_IDS.length;
    const id = CATEGORY_IDS[clamped];
    onChange(id);
    btnRefs.current[clamped]?.focus();
  };

  const onKeyDown = (e: KeyboardEvent, index: number) => {
    switch (e.key) {
      case "ArrowRight":
      case "ArrowDown":
        e.preventDefault();
        focusTab(index + 1);
        break;
      case "ArrowLeft":
      case "ArrowUp":
        e.preventDefault();
        focusTab(index - 1);
        break;
      case "Home":
        e.preventDefault();
        focusTab(0);
        break;
      case "End":
        e.preventDefault();
        focusTab(CATEGORY_IDS.length - 1);
        break;
    }
  };

  return (
    <div
      class={styles.rail}
      role="tablist"
      aria-orientation="vertical"
      aria-label={t("settingsNav.panel")}
    >
      {CATEGORY_IDS.map((id, index) => {
        const selected = id === active;
        return (
          <button
            key={id}
            ref={(el) => {
              btnRefs.current[index] = el;
            }}
            type="button"
            role="tab"
            id={`settings-tab-${id}`}
            aria-selected={selected}
            aria-controls={`settings-panel-${id}`}
            tabIndex={selected ? 0 : -1}
            class={`${styles.tab} ${selected ? styles.active : ""}`}
            onClick={() => onChange(id)}
            onKeyDown={(e) => onKeyDown(e, index)}
          >
            <span class={styles.label}>{t(`settingsNav.${id}.label`)}</span>
            <span class={styles.desc}>{t(`settingsNav.${id}.description`)}</span>
          </button>
        );
      })}
    </div>
  );
}
