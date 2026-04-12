import { useState } from "preact/hooks";
import { sendNotify, dismissNotify } from "../../api/client";
import type { Notification } from "../../api/types";
import { Card, TextField, ColorField, Toggle, Slider, Select, Button, FormRow } from "../../components/ui";
import { toast } from "../../components/Toast";
import styles from "./sections.module.css";

export function NotifySection() {
  const [notif, setNotif] = useState<Notification>({
    text: "",
    icon: "",
    duration: 5,
    color: "#ffffff",
    rainbow: false,
  });
  const [sending, setSending] = useState(false);

  function upd(patch: Partial<Notification>) {
    setNotif((prev) => ({ ...prev, ...patch }));
  }

  async function handleSend() {
    if (!notif.text) {
      toast("Enter message text");
      return;
    }
    setSending(true);
    try {
      const payload: Notification = { text: notif.text };
      if (notif.icon) payload.icon = notif.icon;
      if (notif.layout && notif.layout !== "left") payload.layout = notif.layout;
      if (notif.duration && notif.duration !== 5) payload.duration = notif.duration;
      if (notif.rainbow) payload.rainbow = true;
      if (notif.color && !notif.rainbow) payload.color = notif.color;
      if (notif.rtttl) payload.rtttl = notif.rtttl;
      if (notif.sound) payload.sound = notif.sound;

      await sendNotify(payload);
      toast("Notification sent!");
    } catch {
      toast("Failed to send");
    }
    setSending(false);
  }

  async function handleDismiss() {
    try {
      await dismissNotify();
      toast("Dismissed");
    } catch {
      toast("Failed to dismiss");
    }
  }

  return (
    <Card title="Send Notification">
      <div class={styles.stack}>
        <TextField
          label="Text"
          value={notif.text}
          onChange={(v) => upd({ text: v })}
          placeholder="Hello world!"
        />
        <FormRow>
          <TextField
            label="Icon"
            value={notif.icon || ""}
            onChange={(v) => upd({ icon: v })}
            placeholder="Icon ID or name"
          />
          <Select
            label="Icon Layout"
            value={notif.layout || "left"}
            options={[
              { value: "left", label: "Left" },
              { value: "right", label: "Right" },
              { value: "none", label: "None" },
            ]}
            onChange={(v) => upd({ layout: v as "left" | "right" | "none" })}
          />
        </FormRow>
        <Slider
          label="Duration"
          min={1}
          max={60}
          value={notif.duration || 5}
          onChange={(v) => upd({ duration: v })}
          unit="s"
        />
        <FormRow>
          <Toggle
            label="Rainbow"
            checked={notif.rainbow || false}
            onChange={(v) => upd({ rainbow: v })}
          />
          {!notif.rainbow && (
            <ColorField
              label="Color"
              value={typeof notif.color === "string" ? parseInt(notif.color.replace("#", ""), 16) || 0xffffff : 0xffffff}
              onChange={(v) => upd({ color: "#" + (v & 0xffffff).toString(16).padStart(6, "0") })}
            />
          )}
        </FormRow>
        <FormRow>
          <TextField
            label="Sound"
            value={notif.sound || ""}
            onChange={(v) => upd({ sound: v })}
            placeholder="Filename (optional)"
          />
          <TextField
            label="RTTTL"
            value={notif.rtttl || ""}
            onChange={(v) => upd({ rtttl: v })}
            placeholder="RTTTL string (optional)"
          />
        </FormRow>
        <div class={styles.actions}>
          <Button variant="primary" onClick={handleSend} loading={sending}>
            Send
          </Button>
          <Button onClick={handleDismiss}>
            Dismiss
          </Button>
        </div>
      </div>
    </Card>
  );
}
