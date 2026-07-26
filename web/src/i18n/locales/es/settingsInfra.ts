import type en from "../en/settingsInfra";

const es: typeof en = {
  wifi: {
    confirmConnectTitle: "¿Conectar y reiniciar?",
    confirmConnectBody: "Tras conectarse a «{{ssid}}», esta página perderá el contacto y puede que no vuelva en esta dirección. El reloj mostrará su nueva IP en la pantalla: vuelve a conectarte allí.",
    confirmConnect: "Conectar y reiniciar",
    title: "WiFi",
    apSubtitle:
      "Conéctate a tu red WiFi doméstica. Tras conectarse, el dispositivo se reiniciará con todos los ajustes disponibles.",
    scan: "Buscar redes",
    scanning: "Buscando...",
    connect: "Conectar",
    ssid: "SSID",
    ssidPlaceholder: "Nombre de la red",
    password: "Contraseña",
    scanFailed: "Error en la búsqueda",
    enterSsid: "Introduce el SSID",
    connecting: "Conectando al WiFi...",
    rebooting: "Reiniciando el dispositivo... consulta la matriz para la nueva IP",
    connectionFailed: "Error de conexión",
  },
  network: {
    invalidIp: "Introduce una dirección IPv4 válida, p. ej. 192.168.1.100",
    title: "Red",
    staticIp: "IP estática",
    localIp: "IP local",
    gateway: "Puerta de enlace",
    subnet: "Subred",
    primaryDns: "DNS primario",
  },
  mqtt: {
    invalidPort: "Introduce un puerto entre 1 y 65535",
    title: "MQTT",
    broker: "Broker",
    port: "Puerto",
    username: "Usuario",
    password: "Contraseña",
    prefix: "Prefijo",
    haDiscovery: "Descubrimiento de Home Assistant",
  },
  ntp: {
    title: "NTP y zona horaria",
    server: "Servidor NTP",
    timezone: "Zona horaria",
    hint: "Encuentra tu zona horaria en",
  },
  auth: {
    title: "Autenticación",
    username: "Usuario",
    password: "Contraseña",
    hint: "Déjalo vacío para desactivar la autenticación HTTP.",
  },
  infra: {
    save: "Guardar y reiniciar",
    requiresRestart: "Requiere reinicio",
    restartHint: "Guardar estos ajustes reinicia el reloj y se desconecta brevemente.",
  },
};

export default es;
