import { useRef, useEffect } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { getScreen, nextApp, previousApp } from "../../api/client";
import styles from "./Screen.module.css";

const COLS = 32;
const ROWS = 8;
const CELL = 33;
const PIX = 29;

export function ScreenPage(_props: { path?: string; default?: boolean }) {
  const { t } = useTranslation();
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const running = useRef(true);

  useEffect(() => {
    running.current = true;
    const canvas = canvasRef.current!;
    const ctx = canvas.getContext("2d")!;
    ctx.fillStyle = "#000";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    let timer: ReturnType<typeof setTimeout>;
    async function poll() {
      if (!running.current) return;
      if (document.hidden) {
        timer = setTimeout(poll, 500);
        return;
      }
      try {
        const data = await getScreen();
        for (let i = 0; i < COLS * ROWS; i++) {
          const c = data[i];
          const r = (c & 0xff0000) >> 16;
          const g = (c & 0x00ff00) >> 8;
          const b = c & 0x0000ff;
          const col = i % COLS;
          const row = Math.floor(i / COLS);
          ctx.fillStyle = `rgb(${r},${g},${b})`;
          ctx.fillRect(col * CELL, row * CELL, PIX, PIX);
        }
        timer = setTimeout(poll, 100);
      } catch {
        timer = setTimeout(poll, 1000);
      }
    }
    poll();
    return () => {
      running.current = false;
      clearTimeout(timer);
    };
  }, []);

  function downloadPng() {
    const a = document.createElement("a");
    a.download = "svitrix-screen.png";
    a.href = canvasRef.current!.toDataURL("image/png");
    a.click();
  }

  return (
    <div>
      <div class={styles.controls}>
        <button onClick={() => previousApp()}>&#9664; {t("screen.prevApp")}</button>
        <button onClick={() => nextApp()}>{t("screen.nextApp")} &#9654;</button>
        <button onClick={downloadPng}>{t("screen.downloadPng")}</button>
      </div>
      <canvas
        ref={canvasRef}
        width={COLS * CELL}
        height={ROWS * CELL}
        class={styles.canvas}
        role="img"
        aria-label={t("screen.livePreview")}
      />
    </div>
  );
}
