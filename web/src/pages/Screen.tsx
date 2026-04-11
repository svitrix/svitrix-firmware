import { useRef, useEffect } from "preact/hooks";
import { getScreen, nextApp, previousApp } from "../api/client";

const COLS = 32;
const ROWS = 8;
const CELL = 33;
const PIX = 29;

export function ScreenPage(_props: { path?: string; default?: boolean }) {
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
      <div style={{ display: "flex", gap: 8, marginBottom: 12, flexWrap: "wrap" }}>
        <button onClick={() => previousApp()}>&#9664; Prev</button>
        <button onClick={() => nextApp()}>Next &#9654;</button>
        <button onClick={downloadPng}>Download PNG</button>
      </div>
      <canvas
        ref={canvasRef}
        width={COLS * CELL}
        height={ROWS * CELL}
        style={{
          display: "block",
          background: "#000",
          borderRadius: "var(--radius)",
          border: "1px solid var(--border)",
          maxWidth: "100%",
          height: "auto",
        }}
      />
    </div>
  );
}
