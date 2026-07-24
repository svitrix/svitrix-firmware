import { useState } from "preact/hooks";
import { useTranslation } from "react-i18next";
import { sendNotify, dismissNotify } from "../../../api/client";
import type { Notification } from "../../../api/types";
import { Card, TextField, ColorField, Toggle, Slider, Select, Button, FormRow } from "../../../components/ui";
import { toast } from "../../../components/Toast";
import styles from "./sections.module.css";

export function NotifySection() {
  const { t } = useTranslation();
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
      toast(t("settingsDisplay.notify.enterMessage"));
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
      toast(t("settingsDisplay.notify.sent"));
    } catch {
      toast(t("settingsDisplay.notify.failedSend"));
    }
    setSending(false);
  }

  async function handleDismiss() {
    try {
      await dismissNotify();
      toast(t("settingsDisplay.notify.dismissed"));
    } catch {
      toast(t("settingsDisplay.notify.failedDismiss"));
    }
  }

  return (
    <Card title={t("settingsDisplay.notify.title")}>
      <div class={styles.stack}>
        <TextField
          label={t("settingsDisplay.notify.text")}
          value={notif.text}
          onChange={(v) => upd({ text: v })}
          placeholder={t("settingsDisplay.notify.textPlaceholder")}
        />
        <FormRow>
          <TextField
            label={t("settingsDisplay.notify.icon")}
            value={notif.icon || ""}
            onChange={(v) => upd({ icon: v })}
            placeholder={t("settingsDisplay.notify.iconPlaceholder")}
          />
          <Select
            label={t("settingsDisplay.notify.iconLayout")}
            value={notif.layout || "left"}
            options={[
              { value: "left", label: t("settingsDisplay.notify.layoutLeft") },
              { value: "right", label: t("settingsDisplay.notify.layoutRight") },
              { value: "none", label: t("common.none") },
            ]}
            onChange={(v) => upd({ layout: v as "left" | "right" | "none" })}
          />
        </FormRow>
        <Slider
          label={t("settingsDisplay.notify.duration")}
          min={1}
          max={60}
          value={notif.duration || 5}
          onChange={(v) => upd({ duration: v })}
          unit="s"
        />
        <FormRow>
          <Toggle
            label={t("settingsDisplay.notify.rainbow")}
            checked={notif.rainbow || false}
            onChange={(v) => upd({ rainbow: v })}
          />
          {!notif.rainbow && (
            <ColorField
              label={t("settingsDisplay.notify.color")}
              value={typeof notif.color === "string" ? parseInt(notif.color.replace("#", ""), 16) || 0xffffff : 0xffffff}
              onChange={(v) => upd({ color: "#" + (v & 0xffffff).toString(16).padStart(6, "0") })}
            />
          )}
        </FormRow>
        <FormRow>
          <TextField
            label={t("settingsDisplay.notify.sound")}
            value={notif.sound || ""}
            onChange={(v) => upd({ sound: v })}
            placeholder={t("settingsDisplay.notify.soundPlaceholder")}
          />
          <TextField
            label={t("settingsDisplay.notify.rtttl")}
            value={notif.rtttl || ""}
            onChange={(v) => upd({ rtttl: v })}
            placeholder={t("settingsDisplay.notify.rtttlPlaceholder")}
          />
        </FormRow>
        <div class={styles.actions}>
          <Button variant="primary" onClick={handleSend} loading={sending}>
            {t("settingsDisplay.notify.send")}
          </Button>
          <Button onClick={handleDismiss}>
            {t("settingsDisplay.notify.dismiss")}
          </Button>
        </div>
      </div>
    </Card>
  );
}
