# Data Fetcher

El Data Fetcher permite que SVITRIX automáticamente obtenga datos de APIs HTTP/HTTPS externas y muestre los resultados en la matriz LED — sin necesidad de un sistema de hogar inteligente.

## Cómo Funciona

1. Configuras una o más **fuentes de datos**, cada una apuntando a una API externa
2. SVITRIX obtiene periódicamente cada fuente y extrae un valor de la respuesta JSON
3. El valor extraído se formatea y muestra como una app personalizada en el ciclo de apps

## Interfaz Web

Navega a `http://[IP]/datafetcher` en tu navegador para gestionar fuentes de datos a través de una interfaz visual. Puedes agregar, editar y eliminar fuentes sin escribir ninguna llamada API.

## Configuración

Cada fuente de datos tiene las siguientes configuraciones:

| Configuración | Descripción |
|---------------|-------------|
| **Nombre** | Identificador único — se convierte en el nombre de la app mostrada en el ciclo |
| **URL** | URL completa HTTP o HTTPS del endpoint de la API |
| **Ruta JSON** | Ruta con notación de punto al valor en la respuesta (ej., `bitcoin.usd` o `data.0.price`) |
| **Formato de Visualización** | Formato opcional estilo printf para un solo valor (ej., `$%.0f` para "$67432", `%.1f°C` para "23.5°C"). Especificadores permitidos: `%d %i %u %o %x %X %f %g %e %E %s` con ancho y precisión opcionales (1–2 dígitos cada uno). Usa `%%` para un `%` literal (ej., `%d%%` → "85%"). |
| **Icono** | Nombre de icono opcional de la carpeta ICONS |
| **Color** | Color de texto opcional como hex `#RRGGBB` |
| **Intervalo** | Intervalo de polling en segundos (mínimo 60, por defecto 900) |

## Ejemplos

### Precio de Bitcoin

- **Nombre**: `btc`
- **URL**: `https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd`
- **Ruta JSON**: `bitcoin.usd`
- **Formato**: `$%.0f`
- **Color**: `#F7931A`
- **Intervalo**: `300`

### Tipo de cambio USD/MXN

- **Nombre**: `USDMXN`
- **URL**: `https://open.er-api.com/v6/latest/USD`
- **Ruta JSON**: `rates.MXN`
- **Formato**: `$%.2f`
- **Color**: `#00AA00`
- **Intervalo**: `3600`

### Precio de Ethereum

- **Nombre**: `eth`
- **URL**: `https://api.coingecko.com/api/v3/simple/price?ids=ethereum&vs_currencies=usd`
- **Ruta JSON**: `ethereum.usd`
- **Formato**: `$%.0f`
- **Color**: `#627EEA`
- **Intervalo**: `300`

### GitHub Stars de un repositorio

- **Nombre**: `stars`
- **URL**: `https://api.github.com/repos/USUARIO/REPO`
- **Ruta JSON**: `stargazers_count`
- **Formato**: `%d ★`
- **Color**: `#FFD700`
- **Intervalo**: `3600`

### Tu IP pública

- **Nombre**: `myip`
- **URL**: `https://api.ipify.org?format=json`
- **Ruta JSON**: `ip`
- **Formato**: `%s`
- **Intervalo**: `86400`

---

## Fuentes de APIs Públicas

Estas colecciones contienen cientos de APIs gratuitas compatibles con SVITRIX:

| Recurso | URL | Descripción |
|---------|-----|-------------|
| **Public APIs** | https://github.com/public-apis/public-apis | 1400+ APIs en 50 categorías, filtrable por autenticación |
| **API List** | https://apilist.fun | Búsqueda por tema y popularidad |
| **Free APIs** | https://free-apis.github.io | Listado curado de APIs gratuitas |
| **No Auth APIs** | https://mixedanalytics.com/blog/list-actually-free-open-no-auth-needed-apis/ | APIs sin autenticación |
| **RapidAPI Free** | https://rapidapi.com/collection/list-of-free-apis | Con ejemplos y documentación |

### Requisitos de compatibilidad

Para que una API funcione con SVITRIX:

- ✅ **Sin autenticación** o API key en la URL (no headers personalizados)
- ✅ **Respuesta JSON** menor a 4 KB
- ✅ **Valor extraíble** con notación de punto (`data.value`, `items.0.price`)
- ✅ **Endpoint público** accesible desde tu red

### Ideas por categoría

| Categoría | APIs sugeridas |
|-----------|----------------|
| **Finanzas** | CoinGecko, ExchangeRate-API, Alpha Vantage |
| **Clima** | Open-Meteo, WeatherAPI (ya integrado) |
| **Monitoreo** | UptimeRobot, Pi-hole, Home Assistant |
| **Desarrollo** | GitHub API, GitLab API |
| **Entretenimiento** | Spotify (con token), Twitch |
| **Utilidades** | ipify, WorldTimeAPI |

---

## Limitaciones

- Máximo **8 fuentes de datos** simultáneamente
- La respuesta de la API debe ser menor a **4 KB**
- Solo se soportan **APIs públicas** (sin encabezados de autenticación personalizados)
- Intervalo mínimo de polling es **60 segundos**
- HTTPS está soportado pero sin validación de certificado

## API

Las fuentes de datos también pueden gestionarse vía el [API HTTP](./api#data-fetcher).
