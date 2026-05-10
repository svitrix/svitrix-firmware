# Pendientes - Svitrix Firmware

## UI / Display

- [ ] **Revisar ajustes Brightness Slider**
  - Checar cómo funciona slider Brightness vs sliders Auto Brightness (Min/Max)
  - Verificar interacción cuando Auto Brightness está activo
  - Posiblemente clarificar UI o lógica

- [ ] **Header fijo en Web UI**
  - Verificar que position:sticky funcione correctamente
  - Si no funciona, revisar overflow del contenedor padre

- [x] **Banner de versión al inicio** ✅
  - Muestra "SVITRIX XE1E V x.x.x" + IP + hostname
  - Pausa 2 segundos después del scroll

- [ ] **Colon más pequeño (1x2) para el reloj**
  - Modificar el renderizado del colon (:) en el modo reloj de texto
  - Que sea 1 pixel de ancho por 2 de alto, en lugar del tamaño actual (1x3)
  - Crear renderizado especial para el separador de tiempo

## Localización

- [ ] **Localización Español/Inglés - Display LED**
  - Agregar soporte de idioma para el display LED
  - Nombres de meses abreviados (Ene, Feb, Mar... vs Jan, Feb, Mar...)
  - Días de la semana si se usan
  - Crear función personalizada en lugar de strftime %b

- [ ] **Localización Español/Inglés - Web UI**
  - Implementar sistema i18n para la interfaz web
  - Archivos JSON de traducción (es.json, en.json)
  - Selector de idioma en settings
  - Traducir todas las etiquetas y textos
  - Español como idioma principal

## WiFi

- [ ] **Seguir checando WiFi Networks**
  - Verificar que los SSIDs configurados se muestren correctamente al cargar
  - Probar guardado y reconexión con las 3 redes
  - Verificar comportamiento del fallback entre redes

## Menú en Pantalla

- [ ] **Analizar menú en pantalla**
  - Revisar funciones actuales del menú físico (botones)
  - Optimizar navegación y opciones
  - Agregar nuevas funciones útiles
  - Documentar opciones disponibles

## Apps Nativas

- [x] **App de Clima Exterior (WeatherAPI)** ✅
  - Integrar WeatherAPI para obtener clima actual
  - Mostrar temperatura, condición, icono
  - Configuración de ubicación/API key en settings
  - Apps: OutdoorTemp, OutdoorHum, Pressure, AirQuality
  - Iconos GIF desde /ICONS/ (53628, 66892, 53778)
  - Icono temperatura según condición climática (sunny/cloudy/rainy)

- [ ] **Iconos animados GIF causan reinicio**
  - Batería: 12835.gif causa reinicio
  - AQI: 53778.gif causa reinicio
  - Investigar compatibilidad del formato GIF
  - OutdoorHum (53628.gif) y Pressure (66892.gif) funcionan bien

## Documentación

- [ ] **Manual de Interface Web**
  - Documentar todas las secciones de Settings
  - Explicar cada opción y su función
  - Incluir capturas de pantalla
  - Formato: docs/ o README separado
