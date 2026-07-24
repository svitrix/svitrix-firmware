import type en from "../en/settingsInfra";

const nl: typeof en = {
  wifi: {
    title: "WiFi",
    apSubtitle:
      "Verbind met je thuis-WiFi-netwerk. Na het verbinden herstart het apparaat en zijn alle instellingen beschikbaar.",
    scan: "Netwerken zoeken",
    scanning: "Zoeken...",
    connect: "Verbinden",
    ssid: "SSID",
    ssidPlaceholder: "Netwerknaam",
    password: "Wachtwoord",
    scanFailed: "Zoeken mislukt",
    enterSsid: "Voer SSID in",
    connecting: "Verbinden met WiFi...",
    rebooting: "Apparaat herstart... kijk op de matrix voor het nieuwe IP",
    connectionFailed: "Verbinding mislukt",
  },
  network: {
    title: "Netwerk",
    staticIp: "Statisch IP",
    localIp: "Lokaal IP",
    gateway: "Gateway",
    subnet: "Subnet",
    primaryDns: "Primaire DNS",
  },
  mqtt: {
    title: "MQTT",
    broker: "Broker",
    port: "Poort",
    username: "Gebruikersnaam",
    password: "Wachtwoord",
    prefix: "Prefix",
    haDiscovery: "Home Assistant-detectie",
  },
  ntp: {
    title: "NTP & Tijdzone",
    server: "NTP-server",
    timezone: "Tijdzone",
    hint: "Vind je tijdzone op",
  },
  auth: {
    title: "Authenticatie",
    username: "Gebruikersnaam",
    password: "Wachtwoord",
    hint: "Laat leeg om HTTP-authenticatie uit te schakelen.",
  },
  infra: {
    save: "Netwerk/MQTT/Tijd/Auth opslaan",
  },
};

export default nl;
