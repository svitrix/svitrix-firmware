import { signal } from "@preact/signals";

const toastMsg = signal("");
let timer: ReturnType<typeof setTimeout>;

export function toast(msg: string, ms = 2000) {
  toastMsg.value = msg;
  clearTimeout(timer);
  timer = setTimeout(() => (toastMsg.value = ""), ms);
}

export function ToastContainer() {
  if (!toastMsg.value) return null;
  return <div class="toast">{toastMsg.value}</div>;
}
