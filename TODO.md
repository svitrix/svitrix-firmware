# Pendientes - Svitrix Firmware

## UI / Display

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

- [ ] **App de Clima Exterior (WeatherAPI)**
  - Integrar WeatherAPI para obtener clima actual
  - Mostrar temperatura, condición, icono
  - Configuración de ubicación/API key en settings
  - Agregar como app nativa junto a Time, Date, Temp, Hum, Bat

## Documentación

- [ ] **Manual de Interface Web**
  - Documentar todas las secciones de Settings
  - Explicar cada opción y su función
  - Incluir capturas de pantalla
  - Formato: docs/ o README separado
