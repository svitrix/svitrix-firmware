import type { ComponentChildren } from "preact";
import { useEffect, useRef, useState } from "preact/hooks";
import styles from "./Files.module.css";

/** Wraps the listing as a drop target. Shows a dashed-accent overlay on drag,
 *  guards the window from navigating on stray drops, announces via aria-live. */
export function DropZone({
  onFiles,
  label,
  children,
}: {
  onFiles: (files: FileList) => void;
  label: string;
  children: ComponentChildren;
}) {
  const [over, setOver] = useState(false);
  const depth = useRef(0);

  // Stop the browser from opening a file dropped outside the zone.
  useEffect(() => {
    const prevent = (e: DragEvent) => e.preventDefault();
    window.addEventListener("dragover", prevent);
    window.addEventListener("drop", prevent);
    return () => {
      window.removeEventListener("dragover", prevent);
      window.removeEventListener("drop", prevent);
    };
  }, []);

  return (
    <div
      class={styles.dropWrap}
      onDragEnter={(e) => {
        e.preventDefault();
        depth.current++;
        setOver(true);
      }}
      onDragOver={(e) => e.preventDefault()}
      onDragLeave={() => {
        depth.current--;
        if (depth.current <= 0) setOver(false);
      }}
      onDrop={(e) => {
        e.preventDefault();
        depth.current = 0;
        setOver(false);
        const files = e.dataTransfer?.files;
        if (files && files.length) onFiles(files);
      }}
    >
      {children}
      {over && (
        <div class={styles.dropOverlay} aria-hidden="true">
          <span>{label}</span>
        </div>
      )}
      <span class="sr-only" aria-live="polite">
        {over ? label : ""}
      </span>
    </div>
  );
}
