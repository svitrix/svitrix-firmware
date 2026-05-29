export const translations = {
  es: {
    // Common
    save: "Guardar",
    cancel: "Cancelar",
    loading: "Cargando...",
    enabled: "Activado",
    disabled: "Desactivado",
    yes: "Sí",
    no: "No",
    confirm: "Confirmar",
    noFileSelected: "Ningún archivo seleccionado",
    chooseFile: "Seleccionar archivo",

    // Nav
    nav: {
      settings: "Ajustes",
      mqtt: "MQTT",
      display: "Pantalla",
      apps: "Apps",
      datetime: "Fecha/Hora",
      sound: "Sonido",
      screen: "Vista",
      data: "Datos",
      alarms: "Alarmas",
      files: "Archivos",
      icons: "Iconos",
      backup: "Respaldo",
      system: "Sistema",
      update: "Actualizar",
    },

    // Settings page
    settings: {
      title: "Ajustes",
      wifi: "WiFi",
      network: "Red",
      ntp: "NTP",
      weatherApi: "API del Clima",
      saveButton: "Guardar Red/NTP y Zona Horaria",
      // WiFi
      wifiNetwork: "Red WiFi",
      wifiPassword: "Contraseña",
      wifiConfigured: "Configurada",
      wifiScan: "Escanear",
      wifiScanning: "Escaneando...",
      wifiSelectNetwork: "Seleccionar red",
      // Network
      staticIp: "IP Estática",
      ipAddress: "Dirección IP",
      gateway: "Puerta de enlace",
      subnet: "Máscara de subred",
      dns: "DNS",
      // NTP
      ntpServer: "Servidor NTP",
      timezone: "Zona horaria",
      // Weather API
      apiKey: "Clave API (weatherapi.com)",
      locationMethod: "Método de ubicación",
      cityName: "Nombre de ciudad",
      coordinates: "Coordenadas",
      autoIp: "Auto (IP)",
      stationId: "ID de estación",
      city: "Ciudad",
      latitude: "Latitud",
      longitude: "Longitud",
      updateInterval: "Intervalo de actualización",
      saveWeatherApi: "Guardar API del Clima",
      fetchNow: "Obtener ahora",
      currentWeatherData: "Datos del clima actual",
      temperature: "Temperatura",
      humidity: "Humedad",
      pressure: "Presión",
      condition: "Condición",
      uvIndex: "Índice UV",
      noWeatherData: "Sin datos del clima. Haz clic en \"Obtener ahora\" para probar tu clave API.",
    },

    // MQTT page
    mqtt: {
      title: "MQTT",
      broker: "Broker",
      port: "Puerto",
      username: "Usuario",
      password: "Contraseña",
      prefix: "Prefijo",
      homeAssistant: "Home Assistant",
      haDiscovery: "Descubrimiento HA",
      haPrefix: "Prefijo HA",
      auth: "Autenticación",
      authUser: "Usuario web",
      authPass: "Contraseña web",
      saveButton: "Guardar MQTT/Auth",
    },

    // Display page
    display: {
      title: "Pantalla",
      brightness: "Brillo",
      autoBrightness: "Brillo automático",
      minBrightness: "Brillo mínimo",
      maxBrightness: "Brillo máximo",
      nightMode: "Modo nocturno",
      nightModeEnabled: "Activar modo nocturno",
      nightStart: "Hora de inicio",
      nightEnd: "Hora de fin",
      nightBrightness: "Brillo nocturno",
      notifications: "Notificaciones",
      sendNotification: "Enviar notificación",
      notifyText: "Texto",
      notifyDuration: "Duración",
      notifySend: "Enviar",
      notifyDismiss: "Descartar",
    },

    // Apps page
    apps: {
      title: "Apps",
      time: "Hora",
      date: "Fecha",
      temperature: "Temperatura",
      humidity: "Humedad",
      battery: "Batería",
      celsius: "Celsius",
      offset: "Compensación",
      saveApps: "Guardar Apps",
      weatherApps: "Apps del Clima",
      outdoorTemp: "Temp. exterior",
      outdoorHum: "Hum. exterior",
      pressure: "Presión",
      airQuality: "Calidad del aire",
      uvIndex: "Índice UV",
      autoColor: "Auto",
      autoColorHint: "El color automático cambia según el nivel (verde a amarillo a naranja a rojo).",
      saveWeatherApps: "Guardar Apps del Clima",
      transitions: "Transiciones y Navegación",
      alarmsIndicator: "Indicador de alarmas",
      autoTransition: "Transición automática",
      transitionEffect: "Efecto de transición",
      transitionSpeed: "Velocidad de transición",
      scrollSpeed: "Velocidad de desplazamiento",
      blockNavigation: "Bloquear navegación",
      saveTransitions: "Guardar Transiciones",
    },

    // Time/Date page
    datetime: {
      title: "Fecha y Hora",
      timeFormat: "Formato de hora",
      dateFormat: "Formato de fecha",
      timeMode: "Modo de hora",
      startOnMonday: "Iniciar en lunes",
      showWeekday: "Mostrar día de la semana",
      timeColor: "Color de hora",
      dateColor: "Color de fecha",
      weekdayActive: "Día activo",
      weekdayInactive: "Día inactivo",
      calHeader: "Encabezado cal.",
      calText: "Texto cal.",
      calBody: "Cuerpo cal.",
      saveButton: "Guardar Ajustes de Fecha y Hora",
    },

    // Sound page
    sound: {
      title: "Sonido",
      enabled: "Sonido activado",
      volume: "Volumen",
      saveButton: "Guardar Ajustes de Sonido",
    },

    // Screen page
    screen: {
      title: "Vista en vivo",
      power: "Encendido",
      refresh: "Actualizar",
      previousApp: "App anterior",
      nextApp: "App siguiente",
    },

    // Data Fetcher page
    dataFetcher: {
      title: "Fuentes de datos",
      addSource: "Agregar fuente",
      name: "Nombre",
      url: "URL",
      interval: "Intervalo",
      icon: "Icono",
      delete: "Eliminar",
      fetch: "Obtener",
    },

    // Alarms page
    alarms: {
      title: "Alarmas",
      addAlarm: "Agregar alarma",
      time: "Hora",
      days: "Días",
      melody: "Melodía",
      enabled: "Activada",
      delete: "Eliminar",
      snooze: "Posponer",
      dismiss: "Descartar",
    },

    // Files page
    files: {
      title: "Archivos",
      upload: "Subir",
      newFolder: "Nueva carpeta",
      delete: "Eliminar",
      download: "Descargar",
    },

    // Icons page
    icons: {
      title: "Selector de iconos",
      preview: "Vista previa",
      upload: "Subir icono",
    },

    // Backup page
    backup: {
      title: "Respaldo y Restauración",
      backup: "Respaldo",
      backupHint: "Descarga todos los archivos y ajustes del dispositivo como respaldo JSON. Incluye: iconos, melodías, apps personalizadas, alarmas y toda la configuración.",
      downloadBackup: "Descargar Respaldo",
      restore: "Restaurar",
      restoreHint: "Sube un archivo de respaldo descargado previamente. Los archivos y ajustes serán restaurados. El dispositivo se reiniciará después de restaurar.",
    },

    // System page
    system: {
      title: "Sistema",
      hint: "Guardar, restablecer o reiniciar el dispositivo.",
      saveAll: "Guardar Todos los Ajustes",
      resetDefaults: "Restablecer Valores",
      eraseWifi: "Borrar WiFi",
      reboot: "Reiniciar",
      confirmReset: "¿Restablecer todos los ajustes a valores predeterminados? El dispositivo se reiniciará.",
      confirmEraseWifi: "¿Borrar credenciales WiFi? El dispositivo se reiniciará y entrará en modo AP.",
      confirmReboot: "¿Reiniciar dispositivo?",
      allSaved: "Todos los ajustes guardados",
      settingsReset: "Ajustes restablecidos, reiniciando...",
      wifiErased: "WiFi borrado, reiniciando...",
      rebooting: "Reiniciando...",
    },

    // Update page
    update: {
      title: "Actualización de Firmware",
      otaUpdate: "Actualización OTA",
      hint: "Selecciona un archivo de firmware .bin para subir. El dispositivo se reiniciará automáticamente después de una actualización exitosa.",
      uploading: "Subiendo firmware...",
      uploadComplete: "¡Subida completa! El dispositivo se está reiniciando...",
      uploadFailed: "Subida fallida",
    },

    // Time modes
    timeModes: {
      plainText: "Texto simple (día abajo)",
      calendarBelow: "Calendario (día abajo)",
      calendarAbove: "Calendario (día arriba)",
      calendarAltBelow: "Calendario Alt (día abajo)",
      calendarAltAbove: "Calendario Alt (día arriba)",
      bigDigits: "Dígitos grandes",
      binary: "Binario",
    },

    // Transition effects
    transitions: {
      random: "Aleatorio",
      slideDown: "Deslizar abajo",
      slideUp: "Deslizar arriba",
      slideLeft: "Deslizar izquierda",
      slideRight: "Deslizar derecha",
      dim: "Atenuar",
      zoom: "Zoom",
      rotate: "Rotar",
      pixelate: "Pixelar",
      curtain: "Cortina",
      ripple: "Ondas",
      blink: "Parpadeo",
      reload: "Recargar",
      fade: "Desvanecer",
    },

    // Time intervals
    intervals: {
      minutes: "minutos",
    },
  },

  en: {
    // Common
    save: "Save",
    cancel: "Cancel",
    loading: "Loading...",
    enabled: "Enabled",
    disabled: "Disabled",
    yes: "Yes",
    no: "No",
    confirm: "Confirm",
    noFileSelected: "No file selected",
    chooseFile: "Choose File",

    // Nav
    nav: {
      settings: "Settings",
      mqtt: "MQTT",
      display: "Display",
      apps: "Apps",
      datetime: "Time/Date",
      sound: "Sound",
      screen: "Screen",
      data: "Data",
      alarms: "Alarms",
      files: "Files",
      icons: "Icons",
      backup: "Backup",
      system: "System",
      update: "Update",
    },

    // Settings page
    settings: {
      title: "Settings",
      wifi: "WiFi",
      network: "Network",
      ntp: "NTP",
      weatherApi: "Weather API",
      saveButton: "Save Network/NTP & Time Zone",
      // WiFi
      wifiNetwork: "WiFi Network",
      wifiPassword: "Password",
      wifiConfigured: "Configured",
      wifiScan: "Scan",
      wifiScanning: "Scanning...",
      wifiSelectNetwork: "Select network",
      // Network
      staticIp: "Static IP",
      ipAddress: "IP Address",
      gateway: "Gateway",
      subnet: "Subnet Mask",
      dns: "DNS",
      // NTP
      ntpServer: "NTP Server",
      timezone: "Timezone",
      // Weather API
      apiKey: "API Key (weatherapi.com)",
      locationMethod: "Location Method",
      cityName: "City Name",
      coordinates: "Coordinates",
      autoIp: "Auto (IP)",
      stationId: "Station ID",
      city: "City",
      latitude: "Latitude",
      longitude: "Longitude",
      updateInterval: "Update Interval",
      saveWeatherApi: "Save Weather API",
      fetchNow: "Fetch Now",
      currentWeatherData: "Current Weather Data",
      temperature: "Temperature",
      humidity: "Humidity",
      pressure: "Pressure",
      condition: "Condition",
      uvIndex: "UV Index",
      noWeatherData: "No weather data yet. Click \"Fetch Now\" to test your API key.",
    },

    // MQTT page
    mqtt: {
      title: "MQTT",
      broker: "Broker",
      port: "Port",
      username: "Username",
      password: "Password",
      prefix: "Prefix",
      homeAssistant: "Home Assistant",
      haDiscovery: "HA Discovery",
      haPrefix: "HA Prefix",
      auth: "Authentication",
      authUser: "Web Username",
      authPass: "Web Password",
      saveButton: "Save MQTT/Auth",
    },

    // Display page
    display: {
      title: "Display",
      brightness: "Brightness",
      autoBrightness: "Auto Brightness",
      minBrightness: "Min Brightness",
      maxBrightness: "Max Brightness",
      nightMode: "Night Mode",
      nightModeEnabled: "Enable Night Mode",
      nightStart: "Start Time",
      nightEnd: "End Time",
      nightBrightness: "Night Brightness",
      notifications: "Notifications",
      sendNotification: "Send Notification",
      notifyText: "Text",
      notifyDuration: "Duration",
      notifySend: "Send",
      notifyDismiss: "Dismiss",
    },

    // Apps page
    apps: {
      title: "Apps",
      time: "Time",
      date: "Date",
      temperature: "Temperature",
      humidity: "Humidity",
      battery: "Battery",
      celsius: "Celsius",
      offset: "Offset",
      saveApps: "Save Apps",
      weatherApps: "Weather Apps",
      outdoorTemp: "Outdoor Temp",
      outdoorHum: "Outdoor Hum",
      pressure: "Pressure",
      airQuality: "Air Quality",
      uvIndex: "UV Index",
      autoColor: "Auto",
      autoColorHint: "Auto color changes based on level (green to yellow to orange to red).",
      saveWeatherApps: "Save Weather Apps",
      transitions: "Transitions & Navigation",
      alarmsIndicator: "Alarms Indicator",
      autoTransition: "Auto Transition",
      transitionEffect: "Transition Effect",
      transitionSpeed: "Transition Speed",
      scrollSpeed: "Scroll Speed",
      blockNavigation: "Block Navigation",
      saveTransitions: "Save Transitions",
    },

    // Time/Date page
    datetime: {
      title: "Time & Date Format",
      timeFormat: "Time Format",
      dateFormat: "Date Format",
      timeMode: "Time Mode",
      startOnMonday: "Start on Monday",
      showWeekday: "Show Weekday",
      timeColor: "Time Color",
      dateColor: "Date Color",
      weekdayActive: "Weekday Active",
      weekdayInactive: "Weekday Inactive",
      calHeader: "Cal Header",
      calText: "Cal Text",
      calBody: "Cal Body",
      saveButton: "Save Time & Date Settings",
    },

    // Sound page
    sound: {
      title: "Sound",
      enabled: "Sound Enabled",
      volume: "Volume",
      saveButton: "Save Sound Settings",
    },

    // Screen page
    screen: {
      title: "Live View",
      power: "Power",
      refresh: "Refresh",
      previousApp: "Previous App",
      nextApp: "Next App",
    },

    // Data Fetcher page
    dataFetcher: {
      title: "Data Sources",
      addSource: "Add Source",
      name: "Name",
      url: "URL",
      interval: "Interval",
      icon: "Icon",
      delete: "Delete",
      fetch: "Fetch",
    },

    // Alarms page
    alarms: {
      title: "Alarms",
      addAlarm: "Add Alarm",
      time: "Time",
      days: "Days",
      melody: "Melody",
      enabled: "Enabled",
      delete: "Delete",
      snooze: "Snooze",
      dismiss: "Dismiss",
    },

    // Files page
    files: {
      title: "Files",
      upload: "Upload",
      newFolder: "New Folder",
      delete: "Delete",
      download: "Download",
    },

    // Icons page
    icons: {
      title: "Icon Picker",
      preview: "Preview",
      upload: "Upload Icon",
    },

    // Backup page
    backup: {
      title: "Backup & Restore",
      backup: "Backup",
      backupHint: "Download all files and settings from device as a JSON backup. Includes: icons, melodies, custom apps, alarms, and all configuration.",
      downloadBackup: "Download Backup",
      restore: "Restore",
      restoreHint: "Upload a previously downloaded backup file. Files and settings will be restored. Device will reboot after restore.",
    },

    // System page
    system: {
      title: "System",
      hint: "Save, reset, or reboot the device.",
      saveAll: "Save All Settings",
      resetDefaults: "Reset Defaults",
      eraseWifi: "Erase WiFi",
      reboot: "Reboot",
      confirmReset: "Reset all settings to defaults? Device will restart.",
      confirmEraseWifi: "Erase WiFi credentials? Device will restart and enter AP mode.",
      confirmReboot: "Reboot device?",
      allSaved: "All settings saved",
      settingsReset: "Settings reset, rebooting...",
      wifiErased: "WiFi erased, rebooting...",
      rebooting: "Rebooting...",
    },

    // Update page
    update: {
      title: "Firmware Update",
      otaUpdate: "OTA Update",
      hint: "Select a firmware .bin file to upload. The device will reboot automatically after a successful update.",
      uploading: "Uploading firmware...",
      uploadComplete: "Upload complete! Device is rebooting...",
      uploadFailed: "Upload failed",
    },

    // Time modes
    timeModes: {
      plainText: "Plain Text (weekday below)",
      calendarBelow: "Calendar (weekday below)",
      calendarAbove: "Calendar (weekday above)",
      calendarAltBelow: "Calendar Alt (weekday below)",
      calendarAltAbove: "Calendar Alt (weekday above)",
      bigDigits: "Big Digits",
      binary: "Binary",
    },

    // Transition effects
    transitions: {
      random: "Random",
      slideDown: "Slide Down",
      slideUp: "Slide Up",
      slideLeft: "Slide Left",
      slideRight: "Slide Right",
      dim: "Dim",
      zoom: "Zoom",
      rotate: "Rotate",
      pixelate: "Pixelate",
      curtain: "Curtain",
      ripple: "Ripple",
      blink: "Blink",
      reload: "Reload",
      fade: "Fade",
    },

    // Time intervals
    intervals: {
      minutes: "minutes",
    },
  },
} as const;

export type Language = keyof typeof translations;
export type TranslationKeys = typeof translations.es;
