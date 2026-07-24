import type en from "../en/settingsInfra";

const de: typeof en = {
  wifi: {
    title: "WiFi",
    apSubtitle:
      "Mit dem Heim-WiFi-Netzwerk verbinden. Nach der Verbindung startet das Gerät neu und alle Einstellungen sind verfügbar.",
    scan: "Netzwerke suchen",
    scanning: "Suche läuft...",
    connect: "Verbinden",
    ssid: "SSID",
    ssidPlaceholder: "Netzwerkname",
    password: "Passwort",
    scanFailed: "Suche fehlgeschlagen",
    enterSsid: "SSID eingeben",
    connecting: "Verbindung mit WiFi wird hergestellt...",
    rebooting: "Gerät startet neu... neue IP auf der Matrix prüfen",
    connectionFailed: "Verbindung fehlgeschlagen",
  },
  network: {
    title: "Netzwerk",
    staticIp: "Statische IP",
    localIp: "Lokale IP",
    gateway: "Gateway",
    subnet: "Subnetz",
    primaryDns: "Primärer DNS",
  },
  mqtt: {
    title: "MQTT",
    broker: "Broker",
    port: "Port",
    username: "Benutzername",
    password: "Passwort",
    prefix: "Präfix",
    haDiscovery: "Home Assistant Discovery",
  },
  ntp: {
    title: "NTP & Zeitzone",
    server: "NTP-Server",
    timezone: "Zeitzone",
    hint: "Zeitzone finden unter",
  },
  auth: {
    title: "Authentifizierung",
    username: "Benutzername",
    password: "Passwort",
    hint: "Leer lassen, um die HTTP-Authentifizierung zu deaktivieren.",
  },
  infra: {
    save: "Netzwerk/MQTT/Zeit/Auth speichern",
  },
};

export default de;
