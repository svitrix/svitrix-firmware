import type en from "../en/settingsInfra";

const de: typeof en = {
  wifi: {
    confirmConnectTitle: "Verbinden und neu starten?",
    confirmConnectBody: "Nach dem Verbinden mit „{{ssid}}“ verliert diese Seite den Kontakt und kehrt unter dieser Adresse möglicherweise nicht zurück. Die Uhr zeigt ihre neue IP auf dem Display an – verbinde dich dort neu.",
    confirmConnect: "Verbinden & neu starten",
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
    invalidIp: "Gib eine gültige IPv4-Adresse ein, z. B. 192.168.1.100",
    title: "Netzwerk",
    staticIp: "Statische IP",
    localIp: "Lokale IP",
    gateway: "Gateway",
    subnet: "Subnetz",
    primaryDns: "Primärer DNS",
  },
  mqtt: {
    invalidPort: "Gib einen Port zwischen 1 und 65535 ein",
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
    save: "Speichern & neu starten",
    requiresRestart: "Neustart erforderlich",
    restartHint: "Das Speichern dieser Einstellungen startet die Uhr neu, wobei sie kurz die Verbindung verliert.",
  },
};

export default de;
