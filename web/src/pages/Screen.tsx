import { useRef, useEffect, useState } from "preact/hooks";
import { getScreen, nextApp, previousApp } from "../api/client";

const COLS = 32;
const ROWS = 8;
const CELL = 33;
const PIX = 29;

export function ScreenPage(_props: { path?: string; default?: boolean }) {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const [recording, setRecording] = useState(false);
  const frames = useRef<ImageData[]>([]);
  const running = useRef(true);

  useEffect(() => {
    running.current = true;
    const canvas = canvasRef.current!;
    const ctx = canvas.getContext("2d")!;
    ctx.fillStyle = "#000";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    async function poll() {
      while (running.current) {
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
          if (recording) {
            frames.current.push(ctx.getImageData(0, 0, canvas.width, canvas.height));
          }
        } catch {
          await new Promise((r) => setTimeout(r, 1000));
        }
        await new Promise((r) => setTimeout(r, 100));
      }
    }
    poll();
    return () => {
      running.current = false;
    };
  }, [recording]);

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
        <button
          class={recording ? "btn-danger" : ""}
          onClick={() => {
            if (recording) {
              setRecording(false);
              frames.current = [];
            } else {
              frames.current = [];
              setRecording(true);
            }
          }}
        >
          {recording ? "Stop REC" : "REC GIF"}
        </button>
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
