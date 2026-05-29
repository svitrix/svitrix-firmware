import { useState } from "preact/hooks";
import { sendNotify, dismissNotify } from "../../../api/client";
import type { Notification } from "../../../api/types";
import { Card, TextField, ColorField, Toggle, Slider, Select, Button, FormRow } from "../../../components/ui";
import { toast } from "../../../components/Toast";
import { useT } from "../../../i18n";
import styles from "./sections.module.css";

export function NotifySection() {
  const [notif, setNotif] = useState<Notification>({
    text: "",
    icon: "",
    duration: 5,
    color: "#ffffff",
    rainbow: false,
  });
  const [hold, setHold] = useState(false);
  const [sending, setSending] = useState(false);
  const t = useT();

  function upd(patch: Partial<Notification>) {
    setNotif((prev) => ({ ...prev, ...patch }));
  }

  async function handleSend() {
    if (!notif.text) {
      toast(t.display.notifyEnterText);
      return;
    }
    setSending(true);
    try {
      const payload: Notification = { text: notif.text };
      if (notif.icon) {
        payload.icon = notif.icon;
        if (notif.layout && notif.layout !== "left") payload.layout = notif.layout;
      } else {
        payload.layout = "none";
      }
      if (hold) {
        payload.hold = true;
      } else if (notif.duration && notif.duration !== 5) {
        payload.duration = notif.duration;
      }
      if (notif.rainbow) payload.rainbow = true;
      if (notif.color && !notif.rainbow) payload.color = notif.color;
      if (notif.rtttl) payload.rtttl = notif.rtttl;
      if (notif.sound) payload.sound = notif.sound;

      await sendNotify(payload);
      toast(t.display.notifySent);
    } catch {
      toast(t.display.notifyFailed);
    }
    setSending(false);
  }

  async function handleDismiss() {
    try {
      await dismissNotify();
      toast(t.display.notifyDismissed);
    } catch {
      toast(t.display.notifyDismissFailed);
    }
  }

  return (
    <Card title={t.display.sendNotification}>
      <div class={styles.stack}>
        <TextField
          label={t.display.notifyText}
          value={notif.text}
          onChange={(v) => upd({ text: v })}
          placeholder="Hello world!"
        />
        <FormRow>
          <TextField
            label={t.display.notifyIcon}
            value={notif.icon || ""}
            onChange={(v) => upd({ icon: v })}
            placeholder={t.display.notifyIconPlaceholder}
          />
          {notif.icon && (
            <Select
              label={t.display.notifyLayout}
              value={notif.layout || "left"}
              options={[
                { value: "left", label: t.display.notifyLayoutLeft },
                { value: "right", label: t.display.notifyLayoutRight },
              ]}
              onChange={(v) => upd({ layout: v as "left" | "right" | "none" })}
            />
          )}
        </FormRow>
        <FormRow>
          <Toggle
            label={t.display.notifyHold}
            checked={hold}
            onChange={(v) => setHold(v)}
          />
          {!hold && (
            <Slider
              label={t.display.notifyDuration}
              min={1}
              max={60}
              value={notif.duration || 5}
              onChange={(v) => upd({ duration: v })}
              unit="s"
            />
          )}
        </FormRow>
        <FormRow>
          <Toggle
            label={t.display.notifyRainbow}
            checked={notif.rainbow || false}
            onChange={(v) => upd({ rainbow: v })}
          />
          {!notif.rainbow && (
            <ColorField
              label={t.display.notifyColor}
              value={typeof notif.color === "string" ? parseInt(notif.color.replace("#", ""), 16) || 0xffffff : 0xffffff}
              onChange={(v) => upd({ color: "#" + (v & 0xffffff).toString(16).padStart(6, "0") })}
            />
          )}
        </FormRow>
        <FormRow>
          <TextField
            label={t.display.notifySound}
            value={notif.sound || ""}
            onChange={(v) => upd({ sound: v })}
            placeholder={t.display.notifySoundPlaceholder}
          />
          <TextField
            label={t.display.notifyRtttl}
            value={notif.rtttl || ""}
            onChange={(v) => upd({ rtttl: v })}
            placeholder={t.display.notifyRtttlPlaceholder}
          />
        </FormRow>
        <div class={styles.actions}>
          <Button variant="primary" onClick={handleSend} loading={sending}>
            {t.display.notifySend}
          </Button>
          <Button onClick={handleDismiss}>
            {t.display.notifyDismiss}
          </Button>
        </div>
      </div>
    </Card>
  );
}
