
# Inicio Rápido

Configurar SVITRIX-XE1E toma solo unos minutos. Sigue los pasos a continuación para ir desde el desempaquetado hasta un reloj inteligente funcionando.

## Paso 1. Flashea el firmware

Conecta tu Ulanzi TC001 a una computadora mediante cable USB y abre el [flasher online](./flasher).

::: tip
Si el flasheo no comienza — prueba con un cable USB o puerto diferente. Algunos cables solo soportan carga, no transferencia de datos.
:::

## Paso 2. Conéctate al WiFi

Después de flashear, el dispositivo creará un punto de acceso WiFi:

| Parámetro | Valor |
|-----------|-------|
| Nombre de red | `svitrix_XXXXX` |
| Contraseña | `12345678` |

Conéctate a esta red desde tu teléfono o computadora.

## Paso 3. Configura tu red

Abre un navegador y ve a:

**http://192.168.4.1**

Aparecerá una página de configuración WiFi. Escanea las redes disponibles, selecciona la tuya, ingresa la contraseña y haz clic en Conectar. El dispositivo se reiniciará y se conectará a tu WiFi doméstico.

## Paso 4. Encuentra el dispositivo en tu red

Una vez conectado al WiFi, el dispositivo mostrará:
1. La versión del firmware (ej. `0.4.0`) por unos segundos
2. Un banner desplazable con "SVITRIX XE1E", la dirección IP y el hostname mDNS

Ingresa la dirección IP en tu navegador para abrir la [interfaz web](./webinterface).

::: tip
También puedes acceder al dispositivo usando `http://svitrix_XXXXX.local` (donde XXXXX es el ID de tu dispositivo).
Si te perdiste la dirección IP en pantalla — busca el dispositivo en la lista de dispositivos conectados de tu router.
:::

## Paso 5. Sube la interfaz web (solo desarrolladores)

Si estás compilando desde el código fuente, sube la interfaz web SPA al dispositivo:

```bash
cd web && npm run upload
```

::: tip
Las versiones pre-compiladas del firmware incluyen este paso en el flasher online. Solo necesitas esto si estás compilando desde el código fuente.
:::

## Paso 6. Configura el clima (opcional)

SVITRIX-XE1E incluye apps nativas de clima que muestran temperatura exterior, humedad, presión y calidad del aire. Para habilitarlas:

1. Regístrate para una cuenta gratuita en [WeatherAPI.com](https://www.weatherapi.com)
2. Copia tu API key desde el dashboard
3. En la interfaz web, ve a **Configuración → Clima**
4. Pega tu API key
5. Elige tu método de ubicación:
   - **Nombre de Ciudad** — ej. "Ciudad de México"
   - **Coordenadas** — latitud y longitud
   - **Auto (IP)** — detección automática basada en tu IP
   - **ID de Estación** — para estaciones meteorológicas personales
6. Habilita las apps de clima que quieras (Temp. Exterior, Humedad Exterior, Presión, Calidad del Aire)
7. Haz clic en **Guardar Clima**

::: tip
El plan gratuito de WeatherAPI.com permite 1 millón de llamadas por mes — más que suficiente para un intervalo de actualización de 10 minutos.
:::

## Paso 7. Configura MQTT (opcional)

En la interfaz web, configura la conexión a tu broker MQTT. Esto habilita el control desde HomeAssistant, IOBroker, NodeRed y otros sistemas de hogar inteligente.

Para detalles sobre los comandos disponibles, consulta la sección [API](./api).

## ¡Listo!

SVITRIX-XE1E ya está funcionando con apps preinstaladas: hora, fecha, temperatura, humedad, batería y clima. Para crear tus propias apps personalizadas, consulta la sección [Apps](./apps).
