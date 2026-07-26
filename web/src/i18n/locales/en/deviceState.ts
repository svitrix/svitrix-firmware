// Honest device-state strings — reboot/reconnect overlay, offline banner,
// first-run welcome. Reference as t("deviceState.restarting"), etc.
export default {
  // Reboot / reconnect overlay
  restarting: "Restarting…",
  restartingHint: "Reconnecting to your clock. This usually takes a moment.",
  backOnline: "Back online ✓",
  unreachableTitle: "Couldn't reach the clock",
  unreachableBody:
    "It may be on a different address. Check the clock's display for its IP, then try again.",
  retry: "Retry",
  dismiss: "Dismiss",
  // WiFi connect — informational terminal state
  lookAtClockTitle: "Look at the clock",
  lookAtClockBody:
    "Connecting to \"{{ssid}}\". This page will lose contact — the clock will show its new IP on the display. Reconnect there.",
  // Offline banner
  offlineTitle: "Can't reach your clock",
  offlineBody: "Check that the clock is powered on and on the same network.",
  browserOffline: "This device appears to be offline. Check your internet connection.",
  lastSeen: "Last seen at {{time}}.",
  // First-run welcome
  welcomeTitle: "Welcome to Svitrix",
  welcomeSubtitle: "Connect your clock to Wi-Fi to finish setup.",
};
